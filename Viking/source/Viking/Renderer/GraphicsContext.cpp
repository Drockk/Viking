#include "GraphicsContext.hpp"

#include "Core/Log.hpp"

#include <VkBootstrap.h>

#include "Debug/Profiler.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

constexpr bool USE_VALIDATION_LAYERS{ true };

namespace vi
{
    void GraphicsContext::init(const std::string& p_app_name, const std::unique_ptr<Window>& p_window)
    {
        PROFILER_EVENT();
        vkb::InstanceBuilder builder;

        // Make the vulkan instance, with basic debug features
        auto instance_return = builder.set_app_name(p_app_name.c_str())
            .request_validation_layers(USE_VALIDATION_LAYERS)
            .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT p_message_severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void*) -> VkBool32 {
                switch (p_message_severity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    VI_CORE_TRACE("[VULKAN]: {}", p_callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    VI_CORE_INFO("[VULKAN]: {}", p_callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    VI_CORE_WARN("[VULKAN]: {}", p_callback_data->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    VI_CORE_ERROR("[VULKAN]: {}", p_callback_data->pMessage);
                    break;
                default:
                    VI_CORE_ERROR("Unknown message severity");
                    VI_CORE_TRACE("[VULKAN]: {}", p_callback_data->pMessage);
                    break;
                }

                return VK_FALSE;
            })
            .require_api_version(1, 3, 0)
            .build();

        auto vkb_instance = instance_return.value();

        m_instance = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        m_surface = p_window->create_surface(m_instance);

        // Use vk-bootstrap to select a gpu.
        // We want a gpu that can write to the GLFW surface and supports vulkan 1.3
        vkb::PhysicalDeviceSelector selector{ vkb_instance };
        auto physical_device = selector
            .set_minimum_version(1, 3)
            .set_surface(m_surface)
            .select()
            .value();

        // Create the final vulkan device
        vkb::DeviceBuilder device_builder{ physical_device };

        auto vkb_device = device_builder.build().value();

        // Get the VkDevice handle used in the rest of a vulkan application
        m_device = vkb_device.device;
        m_chosen_gpu = physical_device.physical_device;

        // Use vk-bootstrap to get a Graphics queue
        m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // Initialize the memory allocator
        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_chosen_gpu;
        allocator_info.device = m_device;
        allocator_info.instance = m_instance;
        vmaCreateAllocator(&allocator_info, &m_allocator);

        vkGetPhysicalDeviceProperties(m_chosen_gpu, &m_gpu_properties);
        PROFILER_GPU_INIT_VULKAN(&m_device, &m_chosen_gpu, &m_graphics_queue, &m_graphics_queue_family);
    }

    void GraphicsContext::cleanup()
    {
        PROFILER_EVENT();
        // Make sure the gpu has stopped doing its things
        wait_for_device();

        vmaDestroyAllocator(m_allocator);

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyDevice(m_device, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }

    void GraphicsContext::wait_for_device()
    {
        PROFILER_EVENT();
        vkDeviceWaitIdle(m_device);
    }
}
