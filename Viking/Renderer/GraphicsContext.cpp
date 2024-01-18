#include "GraphicsContext.hpp"

#include "Core/DeletionQueue.hpp"
#include "Core/Log.hpp"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

#include "vk_initializers.hpp" //TODO: To remove

namespace
{
    constexpr bool USE_VALIDATION_LAYERS{ true };
}

namespace vi
{
    void GraphicsContext::init(const std::string& p_app_name, const std::unique_ptr<Window>& p_window)
    {
        vkb::InstanceBuilder builder;

        // Make the vulkan instance, with basic debug features
        auto instance_return = builder.set_app_name(p_app_name.c_str())
            .request_validation_layers(USE_VALIDATION_LAYERS)
            .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT p_message_severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void*) -> VkBool32 {
                switch (p_message_severity) {
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

        m_surface = p_window->create_surface(m_instance);

        // Use vk-bootstrap to select a gpu.
        // We want a gpu that can write to the GLFW surface and supports vulkan 1.3
        vkb::PhysicalDeviceSelector selector{ vkb_instance };
        auto physical_device = selector
            .set_minimum_version(1, 3)
            .set_surface(m_surface)
            .select()
            .value();

        // Create the final vulkan device
        vkb::DeviceBuilder device_builder{ physical_device };

        auto vkb_device = device_builder.build().value();

        // Get the VkDevice handle used in the rest of a vulkan application
        m_device = vkb_device.device;
        m_chosen_gpu = physical_device.physical_device;

        // Use vk-bootstrap to get a Graphics queue
        m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // Initialize the memory allocator
        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_chosen_gpu;
        allocator_info.device = m_device;
        allocator_info.instance = m_instance;
        vmaCreateAllocator(&allocator_info, &m_allocator);

        vkGetPhysicalDeviceProperties(m_chosen_gpu, &m_gpu_properties);

        init_commands();
        init_sync_structures();
    }

    void GraphicsContext::cleanup()
    {
        // Make sure the gpu has stopped doing its things
        wait_for_device();

        vmaDestroyAllocator(m_allocator);

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyDevice(m_device, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
    }

    //void GraphicsContext::immediate_submit(std::function<void(VkCommandBuffer p_cmd)>&& p_function)
    //{
    //}

    void GraphicsContext::wait_for_device()
    {
        vkDeviceWaitIdle(m_device);
    }

    void GraphicsContext::start_frame()
    {
        ++m_frame_number;
    }

    FrameData& GraphicsContext::get_current_frame()
    {
        return m_frames.at(m_frame_number);
    }

    FrameData& GraphicsContext::get_previous_frame()
    {
        if (m_frame_number > 0)
        {
            return m_frames.at(m_frame_number - 1);
        }

        return m_frames.at(m_frame_number);
    }

    void GraphicsContext::end_frame()
    {
        if (m_frame_number == FRAME_OVERLAP - 1)
        {
            m_frame_number = 0;
        }
    }

    std::array<FrameData, FRAME_OVERLAP>& GraphicsContext::get_frames()
    {
        return m_frames;
    }

    UploadContext GraphicsContext::get_upload_context()
    {
        return m_upload_context;
    }

    void GraphicsContext::init_commands()
    {
        // Create a command pool for commands submitted to the graphics queue.
        // We also want the pool to allow for resetting of individual command buffers
        const auto command_pool_info = vkinit::command_pool_create_info(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (auto& frame: m_frames)
        {
            if (const auto result = vkCreateCommandPool(m_device, &command_pool_info, nullptr, &frame.m_command_pool); result)
            {
                throw std::runtime_error(std::format("Cannot create command pool: {}", string_VkResult(result)));
            }

            // Allocate the default command buffer that we will use for rendering.
            auto cmd_alloc_info = vkinit::command_buffer_allocate_info(frame.m_command_pool, 1);

            if(const auto result = vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &frame.m_main_command_buffer); result)
            {
                throw std::runtime_error(std::format("Cannot allocate command buffer: {}", string_VkResult(result)));
            }

            DeletionQueue::push_function([&] {
                vkDestroyCommandPool(m_device, frame.m_command_pool, nullptr);
            });
        }

        const auto upload_command_pool_info = vkinit::command_pool_create_info(m_graphics_queue_family);
        if (const auto result = vkCreateCommandPool(m_device, &upload_command_pool_info, nullptr, &m_upload_context.m_command_pool); result)
        {
            throw std::runtime_error(std::format("Cannot create command pool: {}", string_VkResult(result)));
        }

        DeletionQueue::push_function([&] {
            vkDestroyCommandPool(m_device, m_upload_context.m_command_pool, nullptr);
        });

        //allocate the default command buffer that we will use for rendering
        const auto cmd_alloc_info = vkinit::command_buffer_allocate_info(m_upload_context.m_command_pool, 1);
        if (const auto result = vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &m_upload_context.m_command_buffer); result)
        {
            throw std::runtime_error(std::format("Cannot allocate command buffer: {}", string_VkResult(result)));
        }
    }

    void GraphicsContext::init_sync_structures()
    {
        //create synchronization structures
        //one fence to control when the gpu has finished rendering the frame,
        //and 2 semaphores to synchronize rendering with swap-chain
        //we want the fence to start signaled, so we can wait on it on the first frame
        const auto fence_create_info = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
        const auto semaphore_create_info = vkinit::semaphore_create_info();

        for (auto& frame : m_frames)
        {
            if (const auto result = vkCreateFence(m_device, &fence_create_info, nullptr, &frame.m_render_fence); result)
            {
                throw std::runtime_error(std::format("Cannot create fence: {}", string_VkResult(result)));
            }

            //enqueue the destruction of the fence
            DeletionQueue::push_function([&] {
                vkDestroyFence(m_device, frame.m_render_fence, nullptr);
            });

            if (const auto result = vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &frame.m_present_semaphore); result)
            {
                throw std::runtime_error(std::format("Cannot create semaphore: {}", string_VkResult(result)));
            }

            if (const auto result = vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &frame.m_render_semaphore); result)
            {
                throw std::runtime_error(std::format("Cannot create semaphore: {}", string_VkResult(result)));
            }

            //enqueue the destruction of semaphores
            DeletionQueue::push_function([&] {
                vkDestroySemaphore(m_device, frame.m_present_semaphore, nullptr);
                vkDestroySemaphore(m_device, frame.m_render_semaphore, nullptr);
            });
        }

        const auto upload_fence_create_info = vkinit::fence_create_info();

        if (const auto result = vkCreateFence(m_device, &upload_fence_create_info, nullptr, &m_upload_context.m_upload_fence); result)
        {
            throw std::runtime_error(std::format("Cannot create fence: {}", string_VkResult(result)));
        }

        DeletionQueue::push_function([&] {
            vkDestroyFence(m_device, m_upload_context.m_upload_fence, nullptr);
        });
    }
}
