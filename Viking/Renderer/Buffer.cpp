#include "Buffer.hpp"

#include "GraphicsContext.hpp"

#include <format>
#include <vulkan/vk_enum_string_helper.h>

namespace vi
{
    Buffer::Buffer(const size_t p_alloc_size, const Usage p_usage, const MemoryUsage p_memory_usage)
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
    }

    Buffer::~Buffer()
    {
        //vmaDestroyBuffer();
    }

    VkBufferUsageFlags Buffer::usage_to_vulkan_usage(Usage p_usage)
    {
        if (p_usage == Usage::UNIFORM_BUFFER) {
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        throw std::invalid_argument(std::format("Unknown buffer usage: {:d}", static_cast<int>(p_usage)));
    }

    VmaMemoryUsage Buffer::memory_usage_to_vma_memory_usage(MemoryUsage p_memory_usage)
    {
        if (p_memory_usage == MemoryUsage::CPU_TO_GPU) {
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        }

        throw std::invalid_argument(std::format("Unknown memory usage: {:d}", static_cast<int>(p_memory_usage)));
    }
}
