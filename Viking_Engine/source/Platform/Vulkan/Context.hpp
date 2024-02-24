#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP
#include "Viking/renderer/Context.hpp"

#include "Viking/core/Window.hpp"
#include <vulkan/vulkan.hpp>

namespace vulkan
{
    class Context final: public vi::Context
    {
    public:
        void init(std::string_view p_app_name, const std::unique_ptr<vi::Window>& p_window) override;
        void cleanup() override;

    private:
        VkDebugUtilsMessengerEXT  m_debug_messenger{};
        VkInstance                m_instance{};
        VkSurfaceKHR              m_surface{};
    };
}

#endif
