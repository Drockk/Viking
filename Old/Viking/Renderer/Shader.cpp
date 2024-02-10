#include "Shader.hpp"
#include "Core/Log.hpp"

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <stdexcept>
#include <fmt/format.h>

#include <spdlog/fmt/bin_to_hex.h>

namespace vi
{
    namespace fs = std::filesystem;
    namespace utils
    {
        namespace {
            fs::path get_cache_directory()
            {
                return "assets/cache/shader/vulkan";
            }

            void create_cache_directory_if_needed()
            {
                if (const auto cache_directory = get_cache_directory(); !exists(cache_directory)) {
                    create_directories(cache_directory);
                }
            }

            ShaderType shader_type_from_string(const std::string& p_type)
            {
                if (p_type == "vertex") {
                    return ShaderType::VERTEX;
                }

                if (p_type == "fragment" || p_type == "pixel") {
                    return ShaderType::FRAGMENT;
                }

                VI_CORE_ERROR("Unknown shader type");
                return ShaderType::INVALID;
            }

            std::string shader_type_to_string(const ShaderType p_stage)
            {
                switch (p_stage)
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

            std::string shader_stage_cached_vulkan_file_extension(const ShaderType p_stage)
            {
                switch (p_stage)
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

            shaderc_shader_kind glsl_shader_stage_to_shaderc(const ShaderType p_stage)
            {
                switch (p_stage) {
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
    }

    Shader::Shader(const VkDevice p_device, const fs::path& p_filename): m_file_path{p_filename}, m_device{p_device}
    {
        utils::create_cache_directory_if_needed();

        const auto source = read_file(p_filename);
        const auto shader_sources = pre_process(source);
        compile_or_get_vulkan_binaries(shader_sources);
        create_shader_modules();
    }

    Shader::~Shader()
    {
        std::ranges::for_each(m_shaders, [this](const std::pair<ShaderType, VkShaderModule>&& p_shader) {
            const auto& [type, shader] = p_shader;
            vkDestroyShaderModule(m_device, shader, nullptr);
        });
    }

    std::string Shader::read_file(const std::filesystem::path &p_filename)
    {
        if (not exists(p_filename)) {
            throw std::invalid_argument(fmt::format("File {} doesn't exist", p_filename.string()));
        }

        const auto file_size = fs::file_size(p_filename);

        std::string buffer;
        buffer.resize(file_size);

        std::ifstream shader_file(p_filename, std::ios::binary);

        if (not shader_file.is_open()) {
            throw std::invalid_argument(fmt::format("Cannot open: {}", p_filename.string()));
        }

        shader_file.read(buffer.data(), static_cast<std::streamsize>(file_size));

        return buffer;
    }

    std::unordered_map<ShaderType, std::string> Shader::pre_process(const std::string& p_source)
    {
        std::unordered_map<ShaderType, std::string> shader_sources;

        const std::string type_token = "#type";
        const auto type_token_length = type_token.size();
        auto position = p_source.find(type_token, 0);

        while (position != std::string::npos) {
            const auto eol = p_source.find_first_of("\r\n", position);
            if (eol == std::string::npos) {
                throw std::runtime_error("Shader syntax error");
            }

            const auto begin = position + type_token_length + 1; //Start of shader type name (after "#type " keyword)
            std::string type = p_source.substr(begin, eol - begin);
            auto shader_type = utils::shader_type_from_string(type);
            if (shader_type == ShaderType::INVALID) {
                throw std::runtime_error("Invalid shader type specified");
            }

            const auto next_line_position = p_source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
            if (next_line_position == std::string::npos) {
                throw std::runtime_error("Shader syntax error");
            }

            position = p_source.find(type_token, next_line_position);

            shader_sources[shader_type] = (position == std::string::npos) ? p_source.substr(next_line_position) : p_source.substr(next_line_position, position - next_line_position);
        }

        return shader_sources;
    }

    void Shader::compile_or_get_vulkan_binaries(const std::unordered_map<ShaderType, std::string>& p_sources)
    {
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        auto cache_directory = utils::get_cache_directory();

        auto& shader_data = m_vulkan_spirv;
        shader_data.clear();
        for (auto&& [stage, source]: p_sources) {
            if (auto cached_path = cache_directory / (m_file_path.filename().string() + utils::shader_stage_cached_vulkan_file_extension(stage)); exists(cached_path)) {
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
                shaderc::Compiler compiler;
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
                cached_shader_file.write(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()) * sizeof(uint32_t));
                cached_shader_file.flush();
                cached_shader_file.close();
            }
        }

        std::ranges::for_each(shader_data, [this](const std::pair<ShaderType, std::vector<uint32_t>>& p_shader_data){
            reflect(p_shader_data);
        });
    }

    void Shader::reflect(const std::pair<ShaderType, std::vector<uint32_t>>& p_shader_data) {
        const auto& [stage, data] = p_shader_data;

        const spirv_cross::Compiler compiler(data);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        VI_CORE_TRACE("{0} {1}", utils::shader_type_to_string(stage), m_file_path);
        VI_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
        VI_CORE_TRACE("    {0} resources", resources.sampled_images.size());
        VI_CORE_TRACE("Uniform buffers:");
        for (const auto& [id, type_id, base_type_id, name] : resources.uniform_buffers) {
            const auto& buffer_type = compiler.get_type(base_type_id);
            auto buffer_size = compiler.get_declared_struct_size(buffer_type);
            auto binding = compiler.get_decoration(id, spv::DecorationBinding);
            auto member_count = buffer_type.member_types.size();

            VI_CORE_TRACE("  {0}", name);
            VI_CORE_TRACE("    Size = {0}", buffer_size);
            VI_CORE_TRACE("    Binding = {0}", binding);
            VI_CORE_TRACE("    Members = {0}", member_count);
        }
    }

    void Shader::create_shader_modules()
    {
        std::ranges::for_each(m_vulkan_spirv, [this](const std::pair<ShaderType, std::vector<uint32_t>>&& p_spriv_output) {
            const auto& [type, data] = p_spriv_output;

            VkShaderModuleCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.pNext = nullptr;
            create_info.codeSize = data.size() * sizeof(uint32_t);
            create_info.pCode = data.data();

            VkShaderModule shader{};
            if (const auto result = vkCreateShaderModule(m_device, &create_info, nullptr, &shader); result != VK_SUCCESS) {
                throw std::runtime_error(fmt::format("Cannot create shader module, error: {}", static_cast<int>(result)));
            }

            m_shaders.emplace(type, shader);
        });
    }
} // vi
