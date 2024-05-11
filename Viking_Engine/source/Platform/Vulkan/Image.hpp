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

        VkImage get_image() { return m_image.image; }
        AllocatedImage get_allocated_image() { return m_image; }

    private:
        VkDevice m_device{};
        VmaAllocator m_allocator{};
        AllocatedImage m_image{};
    };

    void copy_image_to_image(VkCommandBuffer p_command, VkImage p_source, VkImage p_destination, VkExtent2D p_source_size, VkExtent2D
                             p_destination_size);
}
