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

    VkQueue m_graphicsQueue; //queue we will submit to
    uint32_t m_graphicsQueueFamily; //family of that queue

    VkCommandPool m_commandPool; //the command pool for our commands
    VkCommandBuffer m_mainCommandBuffer; //the buffer we will record into

    VkRenderPass m_renderPass;
    std::vector<VkFramebuffer> m_framebuffers;

    void init();
    void cleanup();
    void draw();
    void run();

private:
    void initCommands();
    void initSwapchain();
    void initVulkan();
    void initDefaultRenderpass();
    void initFramebuffers();
};
