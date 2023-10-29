//
// Created by batzi on 29.10.2023.
//

#ifndef VIKING_SHADER_HPP
#define VIKING_SHADER_HPP

#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace vi
{
    class Shader
    {
    public:
        explicit Shader(VkDevice t_device, const std::filesystem::path& t_filename);
        ~Shader();

        inline VkShaderModule get_shader_module()
        {
            return m_shader;
        }

        void load_from_file(const std::filesystem::path& t_filename);

    private:
        void create_shader_module(const std::vector<uint32_t>& t_buffer);
        [[nodiscard]] static std::vector<uint32_t> load_from_binary_file(const std::filesystem::path& t_filename);

        VkDevice m_device{};
        VkShaderModule m_shader{};
    };
} // vi

#endif //VIKING_SHADER_HPP
