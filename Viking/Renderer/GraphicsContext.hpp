#pragma once

#include "Core/Window.hpp"

#include <string>
#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace vi
{
    class GraphicsContext
    {
    public:
        static void init(const std::string& p_app_name, const std::unique_ptr<Window>& p_window);
        static void cleanup();

        static void wait_for_device();

        static VkDevice get_device()
        {
            return m_device;
        }

        static VmaAllocator get_allocator()
        {
            return m_allocator;
        }

        static VkQueue get_graphics_queue()
        {
            return m_graphics_queue;
        }

        static uint32_t get_graphics_queue_family()
        {
            return m_graphics_queue_family;
        }

        static VkPhysicalDevice get_chosen_gpu()
        {
            return m_chosen_gpu;
        }

        static VkSurfaceKHR get_surface()
        {
            return m_surface;
        }

        static VkPhysicalDeviceProperties get_gpu_properties()
        {
            return m_gpu_properties;
        }

    private:
        inline static VmaAllocator                m_allocator{};
        inline static VkPhysicalDevice            m_chosen_gpu{};
        inline static VkDebugUtilsMessengerEXT    m_debug_messenger{};
        inline static VkDevice                    m_device{};
        inline static VkInstance                  m_instance{};
        inline static VkPhysicalDeviceProperties  m_gpu_properties{};
        inline static VkQueue                     m_graphics_queue{};
        inline static uint32_t                    m_graphics_queue_family{};
        inline static VkSurfaceKHR                m_surface{};
    };
}
