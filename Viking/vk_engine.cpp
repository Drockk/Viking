#include "vk_engine.hpp"
#include "vk_initializers.hpp"
#include "vk_types.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#define VK_CHECK(x)                                                 \
    do                                                              \
    {                                                               \
        VkResult err = x;                                           \
        if (err)                                                    \
        {                                                           \
            std::cout <<"Detected Vulkan error: " << err << '\n';   \
            abort();                                                \
        }                                                           \
    } while (0)

void ViEngine::init()
{
    if (!glfwInit()) {
        throw std::runtime_error("Cannot initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(m_windowExtent.width, m_windowExtent.height, "Vi Engine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Cannot create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    //load the core Vulkan structures
    initVulkan();
    initSwapchain();
    initCommands();

    m_isInitialized = true;
}

void ViEngine::cleanup()
{
    if (m_isInitialized) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        //destroy swapchain resources
        for (int i = 0; i < m_swapchainImageViews.size(); i++) {

            vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void ViEngine::draw()
{
    //TODO: Nothing yet
}

void ViEngine::run()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        draw();
    }
}

void ViEngine::initCommands()
{
    //create a command pool for commands submitted to the graphics queue.
    //we also want the pool to allow for resetting of individual command buffers
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::commandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool));

    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::commandBufferAllocateInfo(m_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_mainCommandBuffer));
}

void ViEngine::initSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder{ m_device2, m_surface };
    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .use_default_format_selection()
        //use vsync present mode
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(m_windowExtent.width, m_windowExtent.height)
        .build()
        .value();

    //store swapchain and its related images
    m_swapchain = vkbSwapchain.swapchain;
    m_swapchainImages = vkbSwapchain.get_images().value();
    m_swapchainImageViews = vkbSwapchain.get_image_views().value();

    m_swapchainImageFormat = vkbSwapchain.image_format;
}

void ViEngine::initVulkan()
{
    vkb::InstanceBuilder builder;
    //make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("ViEngine")
        .request_validation_layers(true)
        .require_api_version(1, 1, 0)
        .use_default_debug_messenger()
        .build();

    auto& vkb_inst = inst_ret.value();

    //store the debug messenger
    m_instance = vkb_inst.instance;

    //store the debug messenger
    m_debug_messenger = vkb_inst.debug_messenger;

    // get the surface of the window we opened with glfw
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface)) {
        throw std::runtime_error("Cannot create GLFW window surface");
    }

    //use vkbootstrap to select a GPU.
    //We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 1)
        .set_surface(m_surface)
        .select()
        .value();

    //create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };

    m_device2 = deviceBuilder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    m_device = m_device2.device;
    m_chosenGPU = physicalDevice.physical_device;

    // use vkbootstrap to get a Graphics queue
    m_graphicsQueue = m_device2.get_queue(vkb::QueueType::graphics).value();
    m_graphicsQueueFamily = m_device2.get_queue_index(vkb::QueueType::graphics).value();
}
