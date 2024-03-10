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

        [[nodiscard]] VkDevice get_device() const { return m_device; }
        [[nodiscard]] uint32_t get_graphics_queue_family() const { return m_graphics_queue_family; }
        [[nodiscard]] VkQueue get_graphics_queue() const { return m_graphics_queue; }
        [[nodiscard]] Swapchain& get_swapchain() { return m_swapchain; }

    private:
        VkPhysicalDevice            m_chosen_gpu{};
        VkDebugUtilsMessengerEXT    m_debug_messenger{};
        VkDevice                    m_device{};
        VkInstance                  m_instance{};
        VkSurfaceKHR                m_surface{};

        VkQueue m_graphics_queue{};
        uint32_t m_graphics_queue_family{};

        Swapchain m_swapchain{};
    };
}

#endif
