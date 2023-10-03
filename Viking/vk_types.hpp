#pragma once
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

struct AllocatedBuffer {
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};
