//
// Created by batzi on 29.10.2023.
//

#include "Shader.hpp"

#include <fstream>
#include <stdexcept>
#include <fmt/format.h>

namespace vi
{
    Shader::Shader(VkDevice t_device, const std::filesystem::path& t_filename): m_device{t_device}
    {
        load_from_file(t_filename);
    }

    Shader::~Shader()
    {
        vkDestroyShaderModule(m_device, m_shader, nullptr);
    }

    void Shader::load_from_file(const std::filesystem::path& t_filename)
    {
        create_shader_module(load_from_binary_file(t_filename));
    }

    std::vector<uint32_t> Shader::load_from_binary_file(const std::filesystem::path& t_filename)
    {
        if (not std::filesystem::exists(t_filename)) {
            throw std::invalid_argument(fmt::format("Binary Shader file {} doesn't exist", t_filename.string()));
        }

        const auto file_size = std::filesystem::file_size(t_filename);

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
} // vi