//
// Created by batzi on 29.10.2023.
//

#ifndef VIKING_SHADER_HPP
#define VIKING_SHADER_HPP

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
        explicit Shader(VkDevice t_device, const std::filesystem::path& t_filename);
        ~Shader();

        //Old
        inline VkShaderModule get_shader_module()
        {
            return m_shader;
        }

        void load_shader(const std::filesystem::path& t_filename);

    private:
        std::string read_file(const std::filesystem::path& t_filename);
        std::unordered_map<ShaderType, std::string> pre_process(const std::string& t_source);
        void compile_or_get_vulkan_binaries(const std::unordered_map<ShaderType, std::string>& t_sources);
        void reflect(const std::pair<ShaderType, std::vector<uint32_t>>& t_shader_data);

        std::unordered_map<ShaderType, std::vector<uint32_t>> m_vulkanSPIRV;
        std::filesystem::path m_file_path{};


        //Old
        void create_shader_module(const std::vector<uint32_t>& t_buffer);
        [[nodiscard]] static std::vector<uint32_t> load_from_binary_file(const std::filesystem::path& t_filename);


        VkDevice m_device{};
        VkShaderModule m_shader{};
    };
} // vi

#endif //VIKING_SHADER_HPP
