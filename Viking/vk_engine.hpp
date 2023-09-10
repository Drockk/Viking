#pragma once
#include "vk_types.hpp"

#include <VkBootstrap.h>

struct GLFWwindow;

class ViEngine
{
public:
    bool m_isInitialized{ false };
    int m_frameNumber{ 0 };

    VkExtent2D m_windowExtent{ 1600, 900 };
    GLFWwindow* m_window{ nullptr };

    VkSwapchainKHR m_swapchain; // from other articles

    // image format expected by the windowing system
    VkFormat m_swapchainImageFormat;

    //array of images from the swapchain
    std::vector<VkImage> m_swapchainImages;

    //array of image-views from the swapchain
    std::vector<VkImageView> m_swapchainImageViews;

    VkInstance m_instance; // Vulkan library handle
    VkDebugUtilsMessengerEXT m_debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice m_chosenGPU; // GPU chosen as the default device
    VkDevice m_device; // Vulkan device for commands
    vkb::Device m_device2;
    VkSurfaceKHR m_surface; // Vulkan window surface

    void init();
    void cleanup();
    void draw();
    void run();

private:
    void initSwapchain();
    void initVulkan();
};
