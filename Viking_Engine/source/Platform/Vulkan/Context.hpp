#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP
#include "Platform/Vulkan/Swapchain.hpp"

#include "Viking/core/Window.hpp"
#include "Viking/renderer/Context.hpp"

#include <vulkan/vulkan.hpp>

namespace vulkan
{
    class Context final: public vi::Context
    {
    public:
        void init(std::string_view p_app_name, const std::shared_ptr<vi::Window>& p_window) override;
        void cleanup() override;

    private:
        VkPhysicalDevice            m_chosen_gpu{};
        VkDebugUtilsMessengerEXT    m_debug_messenger{};
        VkDevice                    m_device{};
        VkInstance                  m_instance{};
        VkSurfaceKHR                m_surface{};

        Swapchain m_swapchain{};
    };
}

#endif
