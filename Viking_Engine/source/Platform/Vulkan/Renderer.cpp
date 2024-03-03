#include "Platform/Vulkan/Renderer.hpp"

#include <vulkan/vk_enum_string_helper.h>

namespace
{
    VkCommandPoolCreateInfo command_pool_create_info(const uint32_t p_queue_family_index, const VkCommandPoolCreateFlags p_flags)
    {
        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = p_queue_family_index;
        info.flags = p_flags;
        return info;
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool p_pool, uint32_t p_count)
    {
        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = p_pool;
        info.commandBufferCount = p_count;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        return info;
    }
}

namespace vulkan
{
    void Renderer::init(const std::shared_ptr<vi::Context>& p_context)
    {
        const auto context = std::dynamic_pointer_cast<Context>(p_context);
        m_device = context->get_device();
        init_commands(context);
    }

    void Renderer::cleanup()
    {
        std::ranges::for_each(m_frames, [this](const FrameData& p_frame)
        {
            vkDestroyCommandPool(m_device, p_frame.m_command_pool, nullptr);
        });
    }

    void Renderer::init_commands(const std::shared_ptr<Context>& p_context)
    {
        const auto command_pool_info = command_pool_create_info(p_context->get_graphics_queue_family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        std::ranges::for_each(m_frames, [this, command_pool_info](FrameData& p_frame)
        {
            if (const auto result = vkCreateCommandPool(m_device, &command_pool_info, nullptr, &p_frame.m_command_pool); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot create command pool: {}", string_VkResult(result)));
            }

            const auto cmd_alloc_nfo = command_buffer_allocate_info(p_frame.m_command_pool, 1);
            vkAllocateCommandBuffers(m_device, &cmd_alloc_nfo, &p_frame.m_main_command_buffer);
        });
    }
}
