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
        Shader(Shader& p_other) = delete; // Deleted until usage will be found
        Shader(Shader&& p_other) = delete; // Deleted until usage will be found
        ~Shader();

        Shader& operator=(Shader& p_other) = delete; //Deleted until usage will be found
        Shader& operator=(Shader&& p_other) = delete; //Deleted until usage will be found

        [[nodiscard]] VkShaderModule get_shader_module(const ShaderType p_type) const {
            return m_shaders.at(p_type);
        }

    private:
        static std::string read_file(const std::filesystem::path& p_filename);
        static std::unordered_map<ShaderType, std::string> pre_process(const std::string& p_source);
        void compile_or_get_vulkan_binaries(const std::unordered_map<ShaderType, std::string>& p_sources);
        void reflect(const std::pair<ShaderType, std::vector<uint32_t>>& p_shader_data);
        void create_shader_modules();

        std::unordered_map<ShaderType, std::vector<uint32_t>> m_vulkan_spirv;
        std::unordered_map<ShaderType, VkShaderModule> m_shaders;
        std::filesystem::path m_file_path{};

        VkDevice m_device{};
    };
} // vi
