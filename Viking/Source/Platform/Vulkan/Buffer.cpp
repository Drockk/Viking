#include "vipch.hpp"

#include "Viking/Renderer/Mesh.hpp"

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

    void Buffer::copy(Buffer& source, const VkDeviceSize size) const {
        const auto commandBuffer = m_Device->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, source.getBuffer(), m_Buffer, 1, &copyRegion);

        m_Device->endSingleTimeCommands(commandBuffer);
    }

    VkBuffer& Buffer::getBuffer() {
        return m_Buffer;
    }

    VkDeviceMemory& Buffer::getBufferMemory() {
        return m_BufferMemory;
    }

    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices) {
        const VkDeviceSize bufferSize = sizeof indices[0] * indices.size();

        auto stagingBuffer = Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        const auto device = Context::get()->getDevice()->get();
        void* data;
        vkMapMemory(device, stagingBuffer.getBufferMemory(), 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), bufferSize);
        vkUnmapMemory(device, stagingBuffer.getBufferMemory());

        m_Buffer = Viking::createRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_Buffer->copy(stagingBuffer, bufferSize);
    }

    VkBuffer& IndexBuffer::getBuffer() const {
        return m_Buffer->getBuffer();
    }

    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices) {
        const VkDeviceSize bufferSize = sizeof vertices[0] * vertices.size();

        auto stagingBuffer = Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        const auto device = Context::get()->getDevice()->get();
        vkMapMemory(device, stagingBuffer.getBufferMemory(), 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), bufferSize);
        vkUnmapMemory(device, stagingBuffer.getBufferMemory());

        m_Buffer = Viking::createRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_Buffer->copy(stagingBuffer, bufferSize);
    }

    VkBuffer& VertexBuffer::getBuffer() const {
        return m_Buffer->getBuffer();
    }
}
