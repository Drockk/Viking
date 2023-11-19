#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vi
{
    class Buffer
    {
    public:
        enum class Usage
        {
            UNIFORM_BUFFER,
            INVALID
        };

        enum class MemoryUsage
        {
            CPU_TO_GPU,
            INVALID
        };

        struct AllocatedBuffer
        {
            VkBuffer m_buffer;
            VmaAllocation m_allocation;
        };

        Buffer(size_t p_alloc_size, Usage p_usage, MemoryUsage p_memory_usage);
        ~Buffer();



    private:
        [[nodiscard]] static VkBufferUsageFlags usage_to_vulkan_usage(Usage p_usage);
        [[nodiscard]] static VmaMemoryUsage memory_usage_to_vma_memory_usage(MemoryUsage p_memory_usage);

        VmaAllocator m_allocator{};
        AllocatedBuffer m_buffer{};
    };
}
