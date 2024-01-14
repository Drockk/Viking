#include "Buffer.hpp"

#include "GraphicsContext.hpp"

#include <format>
#include <vulkan/vk_enum_string_helper.h>

#include "Core/DeletionQueue.hpp"

namespace vi
{
    Buffer::Buffer(const size_t p_alloc_size, const Usage p_usage, const MemoryUsage p_memory_usage): m_allocated_size{ p_alloc_size }
    {
        m_allocator = GraphicsContext::get_allocator();

        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.pNext = nullptr;
        buffer_info.size = p_alloc_size;
        buffer_info.usage = usage_to_vulkan_usage(p_usage);

        VmaAllocationCreateInfo vma_alloc_info{};
        vma_alloc_info.usage = memory_usage_to_vma_memory_usage(p_memory_usage);

        if (const auto result = vmaCreateBuffer(GraphicsContext::get_allocator(), &buffer_info, &vma_alloc_info, &m_buffer.m_buffer, &m_buffer.m_allocation, nullptr); result != VK_SUCCESS) {
            throw std::runtime_error(string_VkResult(result));
        }

        DeletionQueue::push_function([this]
        {
            destroy();
        });
    }

    Buffer::~Buffer()
    {
        destroy();
    }

    void Buffer::copy_data_to_buffer(const void* p_data, size_t p_size, size_t p_offset)
    {
        map_memory();

        m_data = static_cast<char*>(m_data) + p_offset;

        if(const auto error = memcpy_s(m_data, m_allocated_size, p_data, p_size); error != 0)
        {
            throw std::runtime_error(std::format("Cannot copy data to buffer, error code: {}", error));
        }

        unmap_memory();
    }

    void Buffer::destroy()
    {
        if(m_destroyed)
        {
            return;
        }

        auto& [buffer, allocation] = m_buffer;
        vmaDestroyBuffer(m_allocator, buffer, allocation);
        m_destroyed = true;
    }

    Buffer::AllocatedBuffer& Buffer::get_buffer()
    {
        return m_buffer;
    }

    void Buffer::map_memory()
    {
        vmaMapMemory(m_allocator, m_buffer.m_allocation, &m_data);
    }

    void Buffer::map_memory(void** p_data) const
    {
        vmaMapMemory(m_allocator, m_buffer.m_allocation, p_data);
    }

    void Buffer::unmap_memory()
    {
        vmaUnmapMemory(m_allocator, m_buffer.m_allocation);
        m_data = nullptr;
    }

    VkBufferUsageFlags Buffer::usage_to_vulkan_usage(Usage p_usage)
    {
        switch(p_usage)
        {
        case Usage::UNIFORM_BUFFER:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case Usage::STORAGE_BUFFER:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        case Usage::TRANSFER_SOURCE:
            return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        case Usage::INVALID:
            break;
        }

        throw std::invalid_argument(std::format("Unknown buffer usage: {:d}", static_cast<int>(p_usage)));
    }

    VmaMemoryUsage Buffer::memory_usage_to_vma_memory_usage(MemoryUsage p_memory_usage)
    {
        switch (p_memory_usage)
        {
        case MemoryUsage::CPU_ONLY:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        case MemoryUsage::CPU_TO_GPU:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case MemoryUsage::INVALID:
            break;
        }

        throw std::invalid_argument(std::format("Unknown memory usage: {:d}", static_cast<int>(p_memory_usage)));
    }
}
