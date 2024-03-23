#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "Viking/core/DeletionQueue.hpp"

namespace vulkan
{
    struct AllocatedImage
    {
        VkImage image;
        VkImageView image_view;
        VmaAllocation allocation;
        VkExtent3D image_extent;
        VkFormat image_format;
    };

    class Image
    {
    public:
        Image(VkExtent3D p_extent, VkFormat p_format, VkImageUsageFlags p_usage_flags, VmaAllocator p_allocator, VkDevice p_device, vi::DeletionQueue& p_deletion_queue);

    private:
        VkDevice m_device{};
        VmaAllocator m_allocator{};
        AllocatedImage m_image{};
    };
}
