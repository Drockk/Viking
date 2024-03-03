#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <utility>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vulkan
{
    class Swapchain
    {
    public:
        Swapchain() = default;
        ~Swapchain() = default;
        Swapchain(Swapchain&) = delete;
        Swapchain(Swapchain&&) = delete;

        Swapchain& operator=(Swapchain&) = delete;
        Swapchain& operator=(Swapchain&&) = delete;

        void init(VkPhysicalDevice p_physical_device, VkDevice p_device, VkSurfaceKHR p_surface, const std::pair<uint32_t, uint32_t>& p_resolution);
        void cleanup();

    private:
        VkDevice                    m_device{};
        VkSwapchainKHR              m_swapchain{};
        VkFormat                    m_swapchain_image_format{};
        std::vector<VkImage>        m_swapchain_images{};
        std::vector<VkImageView>    m_swapchain_image_views{};
        VkExtent2D                  m_swapchain_extent{};
    };
}

#endif // !VULKAN_SWAPCHAIN_HPP