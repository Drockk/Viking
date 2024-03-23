#include "Context.hpp"

#include "Platform/Windows/Window.hpp"
#include "Viking/core/Log.hpp"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>


namespace
{
#ifndef NDEBUG
    constexpr bool USE_VALIDATION_LAYERS{ true };
#else
    constexpr bool USE_VALIDATION_LAYERS{ false };
#endif
}

namespace vulkan
{
    void Context::init(const std::string_view p_app_name, const std::shared_ptr<vi::Window>& p_window)
    {
        vkb::InstanceBuilder builder;

        // Make the vulkan instance, with basic debug features
        auto instance_return = builder.set_app_name(std::string{ p_app_name }.c_str())
            .request_validation_layers(USE_VALIDATION_LAYERS)
            .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT p_message_severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void*) -> VkBool32 {
                switch (p_message_severity)
                {
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

        const auto vkb_instance = instance_return.value();

        m_instance = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        const auto* windows_window = dynamic_cast<windows::Window*>(p_window.get());
        m_surface = windows_window->create_surface(m_instance);

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        //use vk-bootstrap to select a gpu. 
        //We want a gpu that can write to the GLFW surface and supports vulkan 1.3 with the correct features
        vkb::PhysicalDeviceSelector selector{ vkb_instance };
        auto physical_device = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_surface(m_surface)
            .select()
            .value();

        //create the final vulkan device
        vkb::DeviceBuilder device_builder{ physical_device };
        auto vkb_device = device_builder.build().value();

        m_device = vkb_device.device;
        m_chosen_gpu = vkb_device.physical_device;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_chosen_gpu;
        allocator_info.device = m_device;
        allocator_info.instance = m_instance;
        allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &m_allocator);

        m_deletion_queue.push_function([&]() {
            vmaDestroyAllocator(m_allocator);
        });

        m_swapchain.init(m_chosen_gpu, m_device, m_surface, p_window->get_size(), m_allocator, m_deletion_queue);

        m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void Context::cleanup()
    {
        vkDeviceWaitIdle(m_device);
        m_deletion_queue.flush();

        m_swapchain.cleanup();

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyDevice(m_device, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }
}
