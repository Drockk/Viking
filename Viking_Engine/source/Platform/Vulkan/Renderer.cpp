#include "Platform/Vulkan/Renderer.hpp"
#include "Platform/Vulkan/Context.hpp"

#include "Viking/core/Log.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>

#include <Viking/core/DeletionQueue.hpp>

namespace utils
{
    constexpr uint64_t ONE_SECOND_IN_NS{ 1000000000 };

    [[nodiscard]] VkCommandPoolCreateInfo command_pool_create_info(const uint32_t p_queue_family_index, const VkCommandPoolCreateFlags p_flags)
    {
        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = p_queue_family_index;
        info.flags = p_flags;
        return info;
    }

    [[nodiscard]] VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool p_pool, uint32_t p_count)
    {
        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = p_pool;
        info.commandBufferCount = p_count;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        return info;
    }

    [[nodiscard]] VkFenceCreateInfo fence_create_info(const VkFenceCreateFlags p_flags)
    {
        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = p_flags;

        return info;
    }

    [[nodiscard]] VkSemaphoreCreateInfo semaphore_create_info(const VkSemaphoreCreateFlags p_flags)
    {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = p_flags;
        return info;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info(const VkCommandBufferUsageFlags p_flags)
    {
        VkCommandBufferBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;

        info.pInheritanceInfo = nullptr;
        info.flags = p_flags;
        return info;
    }

    VkImageSubresourceRange image_subresource_range(const VkImageAspectFlags p_aspect_mask)
    {
        VkImageSubresourceRange sub_image;
        sub_image.aspectMask = p_aspect_mask;
        sub_image.baseMipLevel = 0;
        sub_image.levelCount = VK_REMAINING_MIP_LEVELS;
        sub_image.baseArrayLayer = 0;
        sub_image.layerCount = VK_REMAINING_ARRAY_LAYERS;

        return sub_image;
    }

    void transition_image(const VkCommandBuffer p_cmd, const VkImage p_image, const VkImageLayout p_current_layout, const VkImageLayout p_new_layout)
    {
        VkImageMemoryBarrier2 image_barrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        image_barrier.pNext = nullptr;

        image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        image_barrier.oldLayout = p_current_layout;
        image_barrier.newLayout = p_new_layout;

        const auto aspect_mask = (p_new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        image_barrier.subresourceRange = image_subresource_range(aspect_mask);
        image_barrier.image = p_image;

        VkDependencyInfo dep_info{};
        dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dep_info.pNext = nullptr;

        dep_info.imageMemoryBarrierCount = 1;
        dep_info.pImageMemoryBarriers = &image_barrier;

        vkCmdPipelineBarrier2(p_cmd, &dep_info);
    }

    VkSemaphoreSubmitInfo semaphore_submit_info(const VkPipelineStageFlags2 p_stage_mask, const VkSemaphore p_semaphore)
    {
        VkSemaphoreSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.semaphore = p_semaphore;
        submit_info.stageMask = p_stage_mask;
        submit_info.deviceIndex = 0;
        submit_info.value = 1;

        return submit_info;
    }

    VkCommandBufferSubmitInfo command_buffer_submit_info(const VkCommandBuffer p_cmd)
    {
        VkCommandBufferSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        info.pNext = nullptr;
        info.commandBuffer = p_cmd;
        info.deviceMask = 0;

        return info;
    }

    VkSubmitInfo2 submit_info(const VkCommandBufferSubmitInfo* p_cmd, const VkSemaphoreSubmitInfo* p_signal_semaphore_info, const VkSemaphoreSubmitInfo* p_wait_semaphore_info)
    {
        VkSubmitInfo2 info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.pNext = nullptr;

        info.waitSemaphoreInfoCount = p_wait_semaphore_info == nullptr ? 0 : 1;
        info.pWaitSemaphoreInfos = p_wait_semaphore_info;

        info.signalSemaphoreInfoCount = p_signal_semaphore_info == nullptr ? 0 : 1;
        info.pSignalSemaphoreInfos = p_signal_semaphore_info;

        info.commandBufferInfoCount = 1;
        info.pCommandBufferInfos = p_cmd;

        return info;
    }
}

namespace
{
    struct FrameData
    {
        VkCommandPool m_command_pool{};
        VkCommandBuffer m_main_command_buffer{};
        VkSemaphore m_swapchain_semaphore{};
        VkSemaphore m_render_semaphore{};
        VkFence m_render_fence{};
        vi::DeletionQueue m_deletion_queue{};
    };

    constexpr auto FRAME_OVERLAP{ 2 };

    class InternalRenderer
    {
    public:
        static void init(const std::shared_ptr<vi::Context>& p_context)
        {
            const auto context = std::dynamic_pointer_cast<vulkan::Context>(p_context);
            m_device = context->get_device();
            m_swapchain = context->get_swapchain().get_swapchain();
            m_swapchain_images = context->get_swapchain().get_images();
            m_graphics_queue = context->get_graphics_queue();
            init_commands(context);
            init_sync_structures();
        }

        static void cleanup()
        {
            vkDeviceWaitIdle(m_device);
            std::ranges::for_each(m_frames, [](const FrameData& p_frame)
            {
                vkDestroyCommandPool(m_device, p_frame.m_command_pool, nullptr);

                //destroy sync objects
                vkDestroyFence(m_device, p_frame.m_render_fence, nullptr);
                vkDestroySemaphore(m_device, p_frame.m_render_semaphore, nullptr);
                vkDestroySemaphore(m_device, p_frame.m_swapchain_semaphore, nullptr);
            });
        }

        static void begin_frame()
        {
            //Wait until the gpu has finished rendering the last frame. Timeout of 1 second
            if (const auto result = vkWaitForFences(m_device, 1, &get_current_frame().m_render_fence, true, utils::ONE_SECOND_IN_NS); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Something wrong occured when waiting for finish rendering last frame: {}", string_VkResult(result)));
            }

            get_current_frame().m_deletion_queue.flush();

            if (const auto result = vkResetFences(m_device, 1, &get_current_frame().m_render_fence); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Something wrong occured when resetting render fence: {}", string_VkResult(result)));
            }

            //Request image from swapchain
            if (const auto result = vkAcquireNextImageKHR(m_device, m_swapchain, utils::ONE_SECOND_IN_NS, get_current_frame().m_swapchain_semaphore, nullptr, &m_swapchain_image_index); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Something wrong occured when requesting image from swapchain: {}", string_VkResult(result)));
            }

            //naming it cmd for shorter writing
            const auto cmd = get_current_frame().m_main_command_buffer;

            // now that we are sure that the commands finished executing, we can safely
            // reset the command buffer to begin recording again.
            if (const auto result = vkResetCommandBuffer(cmd, 0); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot reset command buffer: {}", string_VkResult(result)));
            }

            //begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
            const auto cmd_begin_info = utils::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            //start the command buffer recording
            if (const auto result = vkBeginCommandBuffer(cmd, &cmd_begin_info); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot begin command buffer: {}", string_VkResult(result)));
            }

            //make the swapchain image into writeable mode before rendering
            utils::transition_image(cmd, m_swapchain_images[m_swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

            draw_background();
        }

        static void end_frame()
        {
            //naming it cmd for shorter writing
            const auto cmd = get_current_frame().m_main_command_buffer;

            //make the swapchain image into presentable mode
            utils::transition_image(cmd, m_swapchain_images[m_swapchain_image_index], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            if (const auto result = vkEndCommandBuffer(cmd); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot end command buffer: {}", string_VkResult(result)));
            }

            //prepare the submission to the queue.
            //we want to wait on the m_present_semaphore, as that semaphore is signaled when the swapchain is ready
            //we will signal the m_render_semaphore, to signal that rendering has finished
            const auto cmd_info = utils::command_buffer_submit_info(cmd);

            const auto wait_info = utils::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().m_swapchain_semaphore);
            const auto signal_info = utils::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().m_render_semaphore);

            const auto submit = utils::submit_info(&cmd_info, &signal_info, &wait_info);

            //submit command buffer to the queue and execute it.
            // m_render_fence will now block until the graphic commands finish execution
            if (const auto result = vkQueueSubmit2(m_graphics_queue, 1, &submit, get_current_frame().m_render_fence); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot submit queue: {}", string_VkResult(result)));
            }

            //prepare present
            // this will put the image we just rendered to into the visible window.
            // we want to wait on the _renderSemaphore for that, 
            // as its necessary that drawing commands have finished before the image is displayed to the user
            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pNext = nullptr;
            present_info.pSwapchains = &m_swapchain;
            present_info.swapchainCount = 1;

            present_info.pWaitSemaphores = &get_current_frame().m_render_semaphore;
            present_info.waitSemaphoreCount = 1;

            present_info.pImageIndices = &m_swapchain_image_index;

            if (const auto result = vkQueuePresentKHR(m_graphics_queue, &present_info); result != VK_SUCCESS)
            {
                throw std::runtime_error(std::format("Cannot present queue: {}", string_VkResult(result)));
            }

            //increase the number of frames drawn
            ++m_frame_number;
        }

    private:
        static void init_commands(const std::shared_ptr<vulkan::Context>& p_context)
        {
            const auto command_pool_info = utils::command_pool_create_info(p_context->get_graphics_queue_family(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

            std::ranges::for_each(m_frames, [command_pool_info](FrameData& p_frame)
                {
                    if (const auto result = vkCreateCommandPool(m_device, &command_pool_info, nullptr, &p_frame.m_command_pool); result != VK_SUCCESS)
                    {
                        throw std::runtime_error(std::format("Cannot create command pool: {}", string_VkResult(result)));
                    }

                    const auto cmd_alloc_info = utils::command_buffer_allocate_info(p_frame.m_command_pool, 1);
                    vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &p_frame.m_main_command_buffer);
                });
        }

        static void init_sync_structures()
        {
            //Create synchronization structures
            //one fence to control when the gpu has finished rendering the frame,
            //and 2 semaphores to synchronize rendering with swapchain
            //we want the fence to start signalled, so we can wait on it on the first frame
            auto fence_info = utils::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
            auto semaphore_info = utils::semaphore_create_info(0);

            std::ranges::for_each(m_frames, [fence_info, semaphore_info](FrameData& p_frame)
            {
                if (const auto result = vkCreateFence(m_device, &fence_info, nullptr, &p_frame.m_render_fence); result != VK_SUCCESS)
                {
                    throw std::runtime_error(std::format("Cannot create render fence: {}", string_VkResult(result)));
                }

                if (const auto result = vkCreateSemaphore(m_device, &semaphore_info, nullptr, &p_frame.m_swapchain_semaphore); result != VK_SUCCESS)
                {
                    throw std::runtime_error(std::format("Cannot create swapchain semaphore: {}", string_VkResult(result)));
                }

                if (const auto result = vkCreateSemaphore(m_device, &semaphore_info, nullptr, &p_frame.m_render_semaphore); result != VK_SUCCESS)
                {
                    throw std::runtime_error(std::format("Cannot create render semaphore: {}", string_VkResult(result)));
                }
            });
        }

        static FrameData& get_current_frame() { return m_frames.at(m_frame_number % FRAME_OVERLAP); }

        static void draw_background()
        {
            //make a clear-color from frame number. This will flash with a 120 frame period.
            const auto flash = abs(sin(static_cast<float>(m_frame_number) / 120.f));
            const VkClearColorValue clear_value = { { 0.0f, 0.0f, flash, 1.0f } };
            const auto clear_range = utils::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
            //clear image
            const auto cmd = get_current_frame().m_main_command_buffer;
            vkCmdClearColorImage(cmd, m_swapchain_images[m_swapchain_image_index], VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);
        }

        inline static VkDevice m_device{};
        inline static VkSwapchainKHR m_swapchain{};
        inline static std::vector<VkImage> m_swapchain_images;
        inline static VkQueue m_graphics_queue{};

        inline static uint32_t m_frame_number{};
        inline static std::array<FrameData, FRAME_OVERLAP> m_frames;

        inline static uint32_t m_swapchain_image_index{};
    };
}

namespace vulkan
{
    void Renderer::init(const std::shared_ptr<vi::Context>& p_context)
    {
        InternalRenderer::init(p_context);
    }

    void Renderer::cleanup()
    {
        InternalRenderer::cleanup();
    }

    void Renderer::begin_frame()
    {
        InternalRenderer::begin_frame();
    }

    void Renderer::end_frame()
    {
        InternalRenderer::end_frame();
    }
}
