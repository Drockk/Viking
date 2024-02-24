#include "Context.hpp"

#include "Viking/core/Log.hpp"

#include <VkBootstrap.h>

namespace
{
//#ifdef DEBUG
    constexpr bool USE_VALIDATION_LAYERS{ true };
//#else
//    constexpr bool USE_VALIDATION_LAYERS{ false };
//#endif
}

namespace vulkan
{
    void Context::init(const std::string_view p_app_name, const std::unique_ptr<vi::Window>& p_window)
    {
        vkb::InstanceBuilder builder;

        // Make the vulkan instance, with basic debug features
        auto instance_return = builder.set_app_name(std::string{ p_app_name }.c_str())
            .request_validation_layers(USE_VALIDATION_LAYERS)
            .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT p_message_severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void*) -> VkBool32
            {
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

        auto vkb_instance = instance_return.value();

        m_instance = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        //m_surface = p_window->create_surface(m_instance);
    }

    void Context::cleanup()
    {
        //wait_for_device();

        //vmaDestroyAllocator(m_allocator);

        //vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        //vkDestroyDevice(m_device, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }
}
