//
// Created by batzi on 29.10.2023.
//

#include "Shader.hpp"
#include "Core/Log.hpp"

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <stdexcept>
#include <fmt/format.h>

namespace vi
{
    namespace fs = std::filesystem;
    namespace utils
    {
        static fs::path getCacheDirectory()
        {
            return "assets/cache/shader/vulkan";
        }

        static void createCacheDirectoryIfNeeded()
        {
            if (auto cacheDirectory = getCacheDirectory(); !fs::exists(cacheDirectory)) {
                fs::create_directories(cacheDirectory);
            }
        }

        static ShaderType shader_type_from_string(const std::string& t_type)
        {
            if (t_type == "vertex") {
                return ShaderType::VERTEX;
            }

            if (t_type == "fragment" || t_type == "pixel") {
                return ShaderType::FRAGMENT;
            }

            VI_CORE_ERROR("Unknown shader type");
            return ShaderType::INVALID;
        }

        static std::string shader_type_to_string(ShaderType t_stage)
        {
            switch (t_stage)
            {
                case ShaderType::VERTEX:
                    return "vertex";
                case ShaderType::FRAGMENT:
                    return "fragment";
                case ShaderType::INVALID:
                    [[fallthrough]];
            }

            VI_CORE_ERROR("Unknown shader type");
            return {};
        }

        static std::string shader_stage_cached_vulkan_file_extension(ShaderType t_stage)
        {
            switch (t_stage)
            {
                case ShaderType::VERTEX:
                    return ".cached_vulkan.vert";
                case ShaderType::FRAGMENT:
                    return ".cached_vulkan.frag";
                case ShaderType::INVALID:
                    [[fallthrough]];
            }

            VI_CORE_ERROR("Unknown shader type");
            return {};
        }

        static shaderc_shader_kind glsl_shader_stage_to_shaderc(ShaderType t_stage)
        {
            switch (t_stage) {
            case ShaderType::VERTEX:
                return shaderc_glsl_vertex_shader;
            case ShaderType::FRAGMENT:
                return shaderc_glsl_fragment_shader;
            case ShaderType::INVALID:
                [[fallthrough]];
            }

            VI_CORE_ERROR("Unknown shader type");
            return {};
        }
    }

    Shader::Shader(VkDevice t_device, const fs::path& t_filename): m_file_path{t_filename}, m_device{t_device}
    {
        utils::createCacheDirectoryIfNeeded();

        if (t_filename.extension() == ".spv") {
            load_shader(t_filename);
            return;
        }

        auto source = read_file(t_filename);
        auto shader_sources = pre_process(source);
        compile_or_get_vulkan_binaries(shader_sources);
    }

    Shader::~Shader()
    {
        vkDestroyShaderModule(m_device, m_shader, nullptr);
    }

    void Shader::load_shader(const fs::path &t_filename)
    {
        if (t_filename.extension() == ".spv") {
            create_shader_module(load_from_binary_file(t_filename));
        }

        VI_CORE_TRACE("Loaded shader: {}", t_filename.stem());
    }

    std::vector<uint32_t> Shader::load_from_binary_file(const fs::path& t_filename)
    {
        if (not fs::exists(t_filename)) {
            throw std::invalid_argument(fmt::format("Binary Shader file {} doesn't exist", t_filename.string()));
        }

        const auto file_size = fs::file_size(t_filename);

        std::vector<uint32_t> buffer;
        buffer.resize(file_size / sizeof(uint32_t));

        std::ifstream shader_file(t_filename, std::ios::binary);

        if (not shader_file.is_open()) {
            throw std::invalid_argument(fmt::format("Cannot open: {}", t_filename.string()));
        }

        shader_file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));

        shader_file.close();

        return buffer;
    }

    void Shader::create_shader_module(const std::vector<uint32_t>& t_buffer)
    {
        VkShaderModuleCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.codeSize = t_buffer.size() * sizeof(uint32_t);
        createInfo.pCode = t_buffer.data();

        if (auto result = vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shader); result != VK_SUCCESS) {
            throw std::runtime_error(fmt::format("Cannot create shader module, error: {}", static_cast<int>(result)));
        }
    }

    std::string Shader::read_file(const std::filesystem::path &t_filename)
    {
        if (not fs::exists(t_filename)) {
            throw std::invalid_argument(fmt::format("File {} doesn't exist", t_filename.string()));
        }

        const auto file_size = fs::file_size(t_filename);

        std::string buffer;
        buffer.resize(file_size);

        std::ifstream shader_file(t_filename, std::ios::binary);

        if (not shader_file.is_open()) {
            throw std::invalid_argument(fmt::format("Cannot open: {}", t_filename.string()));
        }

        shader_file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));

        return buffer;
    }

    std::unordered_map<ShaderType, std::string> Shader::pre_process(const std::string& t_source)
    {
        std::unordered_map<ShaderType, std::string> shader_sources;

        std::string type_token = "#type";
        const auto type_token_length = type_token.size();
        auto position = t_source.find(type_token, 0);

        while (position != std::string::npos) {
            auto eol = t_source.find_first_of("\r\n", position);
            if (eol == std::string::npos) {
                throw std::runtime_error("Shader syntax error");
            }

            auto begin = position + type_token_length + 1; //Start of shader type name (after "#type " keyword)
            std::string type = t_source.substr(begin, eol - begin);
            auto shader_type = utils::shader_type_from_string(type);
            if (shader_type == ShaderType::INVALID) {
                throw std::runtime_error("Invalid shader type specified");
            }

            auto next_line_position = t_source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            if (next_line_position == std::string::npos) {
                throw std::runtime_error("Shader syntax error");
            }

            position = t_source.find(type_token, next_line_position);

            shader_sources[shader_type] = (position == std::string::npos) ? t_source.substr(next_line_position) : t_source.substr(next_line_position, position - next_line_position);
        }

        return shader_sources;
    }

    void Shader::compile_or_get_vulkan_binaries(const std::unordered_map<ShaderType, std::string>& t_sources)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        std::filesystem::path cache_directory = utils::getCacheDirectory();

        auto& shader_data = m_vulkanSPIRV;
        shader_data.clear();
        for (auto&& [stage, source]: t_sources) {
            auto cached_path = cache_directory / (m_file_path.filename().string() + utils::shader_stage_cached_vulkan_file_extension(stage));

            if (fs::exists(cached_path)) {
                const auto file_size = fs::file_size(cached_path);

                std::ifstream cached_shader_file(cached_path, std::ios::in | std::ios::binary);

                if (not cached_shader_file.is_open()) {
                    throw std::invalid_argument(fmt::format("Cannot open: {}", cached_path.string()));
                }

                auto& data = shader_data[stage];
                data.resize(file_size / sizeof(uint32_t));

                cached_shader_file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(file_size));
            }
            else {
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, utils::glsl_shader_stage_to_shaderc(stage), m_file_path.string().c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    VI_CORE_ERROR(module.GetErrorMessage());
                    return;
                }

                shader_data[stage] = { module.cbegin(), module.cend() };

                std::ofstream cached_shader_file(cached_path, std::ios::out | std::ios::binary);
                if (not cached_shader_file.is_open()) {
                    throw std::invalid_argument(fmt::format("Cannot create: {}", cached_path.string()));
                }

                auto& data = shader_data[stage];
                cached_shader_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(uint32_t));
                cached_shader_file.flush();
                cached_shader_file.close();
            }
        }

        std::ranges::for_each(shader_data, [this](const auto& t_shader_data){
            reflect(t_shader_data);
        });
    }

    void Shader::reflect(const std::pair<ShaderType, std::vector<uint32_t>>& t_shader_data) {
        const auto& [stage, data] = t_shader_data;

        spirv_cross::Compiler compiler(data);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        VI_CORE_TRACE("{0} {1}", utils::shader_type_to_string(stage), m_file_path);
        VI_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
        VI_CORE_TRACE("    {0} resources", resources.sampled_images.size());
        VI_CORE_TRACE("Uniform buffers:");
        for (const auto& resource : resources.uniform_buffers) {
            const auto& bufferType = compiler.get_type(resource.base_type_id);
            uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            int memberCount = bufferType.member_types.size();

            VI_CORE_TRACE("  {0}", resource.name);
            VI_CORE_TRACE("    Size = {0}", bufferSize);
            VI_CORE_TRACE("    Binding = {0}", binding);
            VI_CORE_TRACE("    Members = {0}", memberCount);
        }
    }
} // vi