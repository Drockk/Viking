#pragma once

#include <filesystem>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace vi
{
    enum class ShaderType
    {
        INVALID,
        VERTEX,
        FRAGMENT
    };

    class Shader
    {
    public:
        explicit Shader(VkDevice p_device, const std::filesystem::path& p_filename);
        ~Shader();

        [[nodiscard]] VkShaderModule get_shader_module(const ShaderType p_type) const {
            return m_shaders.at(p_type);
        }

        //Old
        [[nodiscard]] VkShaderModule get_shader_module() const
        {
            return m_shader;
        }

        void load_shader(const std::filesystem::path& p_filename);

    private:
        static std::string read_file(const std::filesystem::path& p_filename);
        static std::unordered_map<ShaderType, std::string> pre_process(const std::string& p_source);
        void compile_or_get_vulkan_binaries(const std::unordered_map<ShaderType, std::string>& p_sources);
        void reflect(const std::pair<ShaderType, std::vector<uint32_t>>& p_shader_data);
        void create_shader_modules();

        std::unordered_map<ShaderType, std::vector<uint32_t>> m_vulkan_spirv;
        std::unordered_map<ShaderType, VkShaderModule> m_shaders;
        std::filesystem::path m_file_path{};


        //Old
        void create_shader_module(const std::vector<uint32_t>& p_buffer);
        [[nodiscard]] static std::vector<uint32_t> load_from_binary_file(const std::filesystem::path& p_filename);


        VkDevice m_device{};
        VkShaderModule m_shader{};
    };
} // vi