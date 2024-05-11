#include "Platform/Vulkan/Swapchain.hpp"

#include <VkBootstrap.h>

namespace vulkan
{
    void Swapchain::init(const VkPhysicalDevice p_physical_device, const VkDevice p_device, const VkSurfaceKHR p_surface, const std::pair<uint32_t, uint32_t>& p_resolution, VmaAllocator p_allocator, vi::DeletionQueue& p_deletion_queue)
    {
        vkb::SwapchainBuilder swapchain_builder{ p_physical_device, p_device, p_surface };
        m_swapchain_image_format = VK_FORMAT_R8G8B8A8_UNORM;

        const auto [width, height] = p_resolution;
        auto vkb_swapchain = swapchain_builder
            .set_desired_format(VkSurfaceFormatKHR{ .format = m_swapchain_image_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        m_swapchain_extent = vkb_swapchain.extent;
        m_swapchain = vkb_swapchain.swapchain;
        m_swapchain_images = vkb_swapchain.get_images().value();
        m_swapchain_image_views = vkb_swapchain.get_image_views().value();

        m_device = p_device;

        create_draw_image(p_resolution, p_allocator, p_deletion_queue);
    }

    void Swapchain::cleanup()
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        std::ranges::for_each(m_swapchain_image_views, [this](const VkImageView p_image_view)
        {
            vkDestroyImageView(m_device, p_image_view, nullptr);
        });
    }

    void Swapchain::create_draw_image(const std::pair<uint32_t, uint32_t>& p_resolution, VmaAllocator p_allocator, vi::DeletionQueue& p_deletion_queue)
    {
        //draw image size will match the window
        const auto [width, height] = p_resolution;
        VkExtent3D draw_image_extent = {
            width,
            height,
            1
        };

        VkImageUsageFlags draw_image_usages{};
        draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
        draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        m_draw_image = std::make_shared<Image>(draw_image_extent, VK_FORMAT_R16G16B16A16_SFLOAT, draw_image_usages, p_allocator, m_device, p_deletion_queue);
    }
}
