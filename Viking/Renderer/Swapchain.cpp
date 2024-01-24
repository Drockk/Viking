#include "Renderer/Swapchain.hpp"

#include "Core/DeletionQueue.hpp"
#include "Renderer/GraphicsContext.hpp"

#include <vulkan/vk_enum_string_helper.h>
#include <VkBootstrap.h>

namespace
{
    VkImageCreateInfo image_create_info(const VkFormat format, const VkImageUsageFlags usage_flags, const VkExtent3D extent)
    {
        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = format;
        info.extent = extent;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = usage_flags;

        return info;
    }

    VkImageViewCreateInfo image_view_create_info(const VkFormat format, const VkImage image, const VkImageAspectFlags aspect_flags)
    {
        //build an image-view for the depth image to use for rendering
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.image = image;
        info.format = format;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = aspect_flags;

        return info;
    }

}

namespace vi
{
    Swapchain::Swapchain(std::pair<uint32_t, uint32_t> p_resolution)
    {
        const auto& chosen_gpu = GraphicsContext::get_chosen_gpu();
        const auto& device = GraphicsContext::get_device();
        const auto& surface = GraphicsContext::get_surface();

        vkb::SwapchainBuilder swapchain_builder{ chosen_gpu, device, surface };

        auto swapchain = swapchain_builder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(p_resolution.first, p_resolution.second)
            .build()
            .value();

        // Store swapchain and its related images
        m_swapchain = swapchain.swapchain;
        m_swapchain_images = swapchain.get_images().value();
        m_swapchain_image_views = swapchain.get_image_views().value();

        m_swapchain_image_format = swapchain.image_format;

        DeletionQueue::push_function([this, device] {
            vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        });

        // Depth image size will match the window
        VkExtent3D depth_image_extent = {
            p_resolution.first,
            p_resolution.second,
            1
        };

        // Hard-coding the depth format to 32-bit float
        m_depth_format = VK_FORMAT_D32_SFLOAT;

        // The depth image will be an image with the format we selected and Depth Attachment usage flag
        auto depth_image_info = image_create_info(m_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depth_image_extent);

        // for the depth image, we want to allocate it from gpu local memory
        VmaAllocationCreateInfo depth_image_allocation_info{};
        depth_image_allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        depth_image_allocation_info.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Allocate and create the image
        auto allocator = GraphicsContext::get_allocator();
        vmaCreateImage(allocator, &depth_image_info, &depth_image_allocation_info, &m_depth_image.m_image, &m_depth_image.m_allocation, nullptr);

        //build an image-view for the depth image to use for rendering
        auto depth_view_info = image_view_create_info(m_depth_format, m_depth_image.m_image, VK_IMAGE_ASPECT_DEPTH_BIT);

        if (const auto error = vkCreateImageView(device, &depth_view_info, nullptr, &m_depth_image_view); error)
        {
            throw std::runtime_error(std::format("Cannot create image view, error code: {}", string_VkResult(error)));
        }

        DeletionQueue::push_function([this, allocator, device] {
            vkDestroyImageView(device, m_depth_image_view, nullptr);
            vmaDestroyImage(allocator, m_depth_image.m_image, m_depth_image.m_allocation);
        });
    }

    VkSwapchainKHR Swapchain::get_swapchain() const
    {
        return m_swapchain;
    }

    VkFormat Swapchain::get_swapchain_format() const
    {
        return m_swapchain_image_format;
    }

    std::vector<VkImage>& Swapchain::get_swapchain_images()
    {
        return m_swapchain_images;
    }

    std::vector<VkImageView>& Swapchain::get_swapchain_images_views()
    {
        return m_swapchain_image_views;
    }

    VkFormat Swapchain::get_depth_format() const
    {
        return m_depth_format;
    }

    VkImageView& Swapchain::get_depth_image_view()
    {
        return m_depth_image_view;
    }
}
