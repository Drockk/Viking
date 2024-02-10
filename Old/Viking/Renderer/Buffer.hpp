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
            STORAGE_BUFFER,
            STAGING_BUFFER,
            TRANSFER_SOURCE,
            INVALID
        };

        enum class MemoryUsage
        {
            CPU_ONLY,
            CPU_TO_GPU,
            INVALID
        };

        struct AllocatedBuffer
        {
            VkBuffer m_buffer;
            VmaAllocation m_allocation;
        };

        Buffer(size_t p_alloc_size, Usage p_usage, MemoryUsage p_memory_usage);
        Buffer(Buffer& p_other) = delete; // Deleted until usage will be found
        Buffer(Buffer&& p_other) = delete; // Deleted until usage will be found
        ~Buffer();

        Buffer& operator=(Buffer& p_other) = delete; //Deleted until usage will be found
        Buffer& operator=(Buffer&& p_other) = delete; //Deleted until usage will be found

        void copy_data_to_buffer(const void* p_data, size_t p_size, size_t p_offset = 0);

        AllocatedBuffer& get_buffer();

        void map_memory(void** p_data) const;
        void unmap_memory();

    private:
        void destroy();

        void map_memory();

        [[nodiscard]] static VkBufferUsageFlags usage_to_vulkan_usage(Usage p_usage);
        [[nodiscard]] static VmaMemoryUsage memory_usage_to_vma_memory_usage(MemoryUsage p_memory_usage);

        VmaAllocator m_allocator{};
        AllocatedBuffer m_buffer{};

        size_t m_allocated_size{ 0 };
        bool m_destroyed{ false };
        void* m_data{ nullptr };
    };
}
