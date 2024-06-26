#include "Platform/Vulkan/Image.hpp"

namespace 
{
    VkImageCreateInfo image_create_info(const VkFormat p_format, const VkImageUsageFlags p_usage_flags, const VkExtent3D p_extent)
    {
        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.imageType = VK_IMAGE_TYPE_2D;

        info.format = p_format;
        info.extent = p_extent;

        info.mipLevels = 1;
        info.arrayLayers = 1;

        //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
        info.samples = VK_SAMPLE_COUNT_1_BIT;

        //optimal tiling, which means the image is stored on the best gpu format
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = p_usage_flags;

        return info;
    }

    VkImageViewCreateInfo imageview_create_info(const VkFormat p_format, const VkImage p_image, const VkImageAspectFlags p_aspect_flags)
    {
        // build an image-view for the depth image to use for rendering
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;

        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.image = p_image;
        info.format = p_format;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = p_aspect_flags;

        return info;
    }
}

namespace vulkan
{
    Image::Image(const VkExtent3D p_extent, const VkFormat p_format, const VkImageUsageFlags p_usage_flags, const VmaAllocator p_allocator, const VkDevice p_device, vi::DeletionQueue& p_deletion_queue): m_device{p_device}, m_allocator{p_allocator}
    {
        m_image.image_format = p_format;
        m_image.image_extent = p_extent;

        const auto image_info = image_create_info(p_format, p_usage_flags, p_extent);

        //for the draw image, we want to allocate it from gpu local memory
        VmaAllocationCreateInfo image_alloc_info{};
        image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        image_alloc_info.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        //allocate and create the image
        vmaCreateImage(p_allocator, &image_info, &image_alloc_info, &m_image.image, &m_image.allocation, nullptr);

        const auto view_info = imageview_create_info(p_format, m_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

        if (const auto result = vkCreateImageView(p_device, &view_info, nullptr, &m_image.image_view); result != VK_SUCCESS)
        {
            throw std::runtime_error("Cannot create image view");
        }

        p_deletion_queue.push_function([this]()
        {
            vkDestroyImageView(m_device, m_image.image_view, nullptr);
            vmaDestroyImage(m_allocator, m_image.image, m_image.allocation);
        });
    }

    void copy_image_to_image(const VkCommandBuffer p_command, const VkImage p_source, const VkImage p_destination, const VkExtent2D p_source_size, const VkExtent2D
                             p_destination_size)
    {
        VkImageBlit2 blit_region{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

        blit_region.srcOffsets[1].x = static_cast<int32_t>(p_source_size.width);
        blit_region.srcOffsets[1].y = static_cast<int32_t>(p_source_size.height);
        blit_region.srcOffsets[1].z = 1;

        blit_region.dstOffsets[1].x = static_cast<int32_t>(p_destination_size.width);
        blit_region.dstOffsets[1].y = static_cast<int32_t>(p_destination_size.height);
        blit_region.dstOffsets[1].z = 1;

        blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.srcSubresource.baseArrayLayer = 0;
        blit_region.srcSubresource.layerCount = 1;
        blit_region.srcSubresource.mipLevel = 0;

        blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.dstSubresource.baseArrayLayer = 0;
        blit_region.dstSubresource.layerCount = 1;
        blit_region.dstSubresource.mipLevel = 0;

        VkBlitImageInfo2 blit_info{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
        blit_info.dstImage = p_destination;
        blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blit_info.srcImage = p_source;
        blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blit_info.filter = VK_FILTER_LINEAR;
        blit_info.regionCount = 1;
        blit_info.pRegions = &blit_region;

        vkCmdBlitImage2(p_command, &blit_info);
    }
}
