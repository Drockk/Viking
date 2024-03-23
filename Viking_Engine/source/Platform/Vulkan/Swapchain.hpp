#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include "Platform/Vulkan/Image.hpp"
#include "Viking/core/DeletionQueue.hpp"

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

        void init(VkPhysicalDevice p_physical_device, VkDevice p_device, VkSurfaceKHR p_surface, const std::pair<uint32_t, uint32_t>& p_resolution, VmaAllocator p_allocator, vi::DeletionQueue& p_deletion_queue);
        void cleanup();

        [[nodiscard]] VkSwapchainKHR get_swapchain() const { return m_swapchain; }
        [[nodiscard]] std::vector<VkImage>& get_images() { return m_swapchain_images; }

    private:
        VkDevice                    m_device{};
        VkSwapchainKHR              m_swapchain{};
        VkFormat                    m_swapchain_image_format{};
        std::vector<VkImage>        m_swapchain_images{};
        std::vector<VkImageView>    m_swapchain_image_views{};
        VkExtent2D                  m_swapchain_extent{};

        std::shared_ptr<Image>      m_draw_image{};
    };
}

#endif // !VULKAN_SWAPCHAIN_HPP
