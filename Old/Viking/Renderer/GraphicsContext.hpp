#pragma once

#include "Core/Window.hpp"
#include "Renderer/Buffer.hpp"

#include <string>
#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace vi
{
    constexpr auto FRAME_OVERLAP{ 2 };

    struct FrameData
    {
        VkSemaphore m_present_semaphore{};
        VkSemaphore m_render_semaphore{};
        VkFence m_render_fence{};

        VkCommandPool m_command_pool{};
        VkCommandBuffer m_main_command_buffer{};

        std::unique_ptr<Buffer> m_camera_buffer{};
        VkDescriptorSet m_global_descriptor{};

        std::unique_ptr<Buffer> m_object_buffer{};
        VkDescriptorSet m_object_descriptor{};
    };

    struct UploadContext
    {
        VkFence m_upload_fence{};
        VkCommandPool m_command_pool{};
        VkCommandBuffer m_command_buffer{};
    };

    class GraphicsContext
    {
    public:
        static void init(const std::string& p_app_name, const std::unique_ptr<Window>& p_window);
        static void cleanup();

        //static void immediate_submit(std::function<void(VkCommandBuffer p_cmd)>&& p_function);

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

        static void start_frame();
        static FrameData& get_current_frame();
        static FrameData& get_previous_frame();
        static void end_frame();

        static std::array<FrameData, FRAME_OVERLAP>& get_frames();

        static UploadContext get_upload_context();

    private:
        static void init_commands();
        static void init_sync_structures();

        inline static VmaAllocator                          m_allocator{};
        inline static VkPhysicalDevice                      m_chosen_gpu{};
        inline static VkDebugUtilsMessengerEXT              m_debug_messenger{};
        inline static VkDevice                              m_device{};
        inline static VkInstance                            m_instance{};
        inline static VkPhysicalDeviceProperties            m_gpu_properties{};
        inline static VkQueue                               m_graphics_queue{};
        inline static uint32_t                              m_graphics_queue_family{};
        inline static VkSurfaceKHR                          m_surface{};
        inline static std::array<FrameData, FRAME_OVERLAP>  m_frames{};
        inline static uint32_t                              m_frame_number{0};
        inline static UploadContext                         m_upload_context;
    };
}
