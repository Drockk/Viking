#pragma once
#include "Viking/Renderer/Buffer.hpp"

#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
    class Buffer {
    public:
        Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();

        void copy(Buffer& source, VkDeviceSize size) const;

        VkBuffer& getBuffer();
        VkDeviceMemory& getBufferMemory();
    private:
        Viking::Ref<Device> m_Device;

        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
    };

    class IndexBuffer: public Viking::IndexBuffer {
    public:
        IndexBuffer(const std::vector<uint32_t>& indices);
        ~IndexBuffer() override = default;

        [[nodiscard]] VkBuffer& getBuffer() const;

    private:
        Viking::Ref<Buffer> m_Buffer{};
    };

    class VertexBuffer: public Viking::VertexBuffer {
    public:
        VertexBuffer(const std::vector<Vertex>& vertices);

        [[nodiscard]] VkBuffer& getBuffer() const;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    private:
        Viking::Ref<Buffer> m_Buffer{};
    };
}
