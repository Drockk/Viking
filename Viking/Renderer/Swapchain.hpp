#pragma once

#include <utility>

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vi
{
    struct AllocatedImage
    {
        VkImage m_image;
        VmaAllocation m_allocation;
    };

    class Swapchain
    {
    public:
        Swapchain(std::pair<uint32_t, uint32_t> p_resolution);

        [[nodiscard]] VkSwapchainKHR get_swapchain() const;
        [[nodiscard]] VkFormat get_swapchain_format() const;
        [[nodiscard]] std::vector<VkImage>& get_swapchain_images();
        [[nodiscard]] std::vector<VkImageView>& get_swapchain_images_views();

        [[nodiscard]] VkFormat get_depth_format() const;
        [[nodiscard]] VkImageView& get_depth_image_view();


    private:
        VkFormat m_depth_format{};
        AllocatedImage m_depth_image{};
        VkImageView m_depth_image_view{};
        VkSwapchainKHR m_swapchain{};
        VkFormat m_swapchain_image_format;
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
    };
}
