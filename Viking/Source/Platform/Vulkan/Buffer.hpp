#pragma once

#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
    class Buffer {
    public:
        Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();

        VkBuffer& getBuffer();
        VkDeviceMemory& getBufferMemory();
    private:
        Viking::Ref<Device> m_Device;

        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
    };
}
