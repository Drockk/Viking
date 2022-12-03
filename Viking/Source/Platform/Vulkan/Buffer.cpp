#include "vipch.hpp"
#include "Platform/Vulkan/Buffer.hpp"
#include "Platform/Vulkan/Context.hpp"

namespace Vulkan {
    Buffer::Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_Device = Context::get()->getDevice();
        const auto device = m_Device->get();

        if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Context::get()->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
    }

    Buffer::~Buffer() {
        m_Device = Context::get()->getDevice();
        const auto device = m_Device->get();

        vkDestroyBuffer(device, m_Buffer, nullptr);
        vkFreeMemory(device, m_BufferMemory, nullptr);
    }

    VkBuffer& Buffer::getBuffer() {
        return m_Buffer;
    }

    VkDeviceMemory& Buffer::getBufferMemory() {
        return m_BufferMemory;
    }
}
