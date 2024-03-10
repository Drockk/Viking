#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "Platform/Vulkan/Context.hpp"
#include "Viking/renderer/Context.hpp"

#include <vulkan/vulkan.hpp>

namespace vulkan
{
    struct FrameData
    {
        VkCommandPool m_command_pool{};
        VkCommandBuffer m_main_command_buffer{};
        VkSemaphore m_swapchain_semaphore{};
        VkSemaphore m_render_semaphore{};
        VkFence m_render_fence{};
    };

    constexpr auto FRAME_OVERLAP{ 2 };

    class Renderer
    {
    public:
        void init(const std::shared_ptr<vi::Context>& p_context);
        void cleanup();

        void draw(); //TODO: Split into `begin_frame` and `end_frame`

        FrameData& get_current_frame() { return m_frames.at(m_frame_number % FRAME_OVERLAP); }
    private:
        void init_commands(const std::shared_ptr<Context>& p_context);
        void init_sync_structures();

        VkDevice m_device{};

        uint32_t m_frame_number{};
        std::array<FrameData, FRAME_OVERLAP> m_frames;

        VkQueue m_graphics_queue{};
        uint32_t m_graphics_queue_family{};

        VkSwapchainKHR m_swapchain{};
        std::vector<VkImage> m_swapchain_images;
    };
}

#endif // VULKAN_RENDERER_HPP
