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
    };

    constexpr auto FRAME_OVERLAP{ 2 };

    class Renderer
    {
    public:
        void init(const std::shared_ptr<vi::Context>& p_context);
        void cleanup();

        FrameData& get_current_frame() { return m_frames.at(m_frame_number % FRAME_OVERLAP); }
    private:
        void init_commands(const std::shared_ptr<Context>& p_context);

        VkDevice m_device{};

        uint32_t m_frame_number{};
        std::array<FrameData, FRAME_OVERLAP> m_frames;

        VkQueue m_graphics_queue{};
        uint32_t m_graphics_queue_family{};
    };
}

#endif // VULKAN_RENDERER_HPP
