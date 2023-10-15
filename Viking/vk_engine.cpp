#include "vk_engine.hpp"
#include "vk_initializers.hpp"
#include "vk_types.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#define VK_CHECK(x)                                                 \
    do                                                              \
    {                                                               \
        VkResult err = x;                                           \
        if (err)                                                    \
        {                                                           \
            std::cout <<"Detected Vulkan error: " << err << '\n';   \
            abort();                                                \
        }                                                           \
    } while (0)

void ViEngine::init()
{
    if (!glfwInit()) {
        throw std::runtime_error("Cannot initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(static_cast<int>(m_windowExtent.width), static_cast<int>(m_windowExtent.height), "Vi Engine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Cannot create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    //load the core Vulkan structures
    initVulkan();
    initSwapchain();
    initCommands();
    initDefaultRenderpass();
    initFramebuffers();
    initSyncStructures();
    initPipelines();

    loadMeshes();

    m_isInitialized = true;
}

void ViEngine::cleanup()
{
    if (m_isInitialized) {
        // Make sure the GPU has stopped doing its things
        vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000);

        m_mainDeletionQueue.flush();

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
        vkDestroyInstance(m_instance, nullptr);
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void ViEngine::draw()
{
    // Wait until the GPU has finished rendering the last frame. Timeout of 1 second
    VK_CHECK(vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000));
    VK_CHECK(vkResetFences(m_device, 1, &m_renderFence));

    // Request image from the swapchain, one second timeout
    uint32_t swapchainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_presentSemaphore, nullptr, &swapchainImageIndex));

    // Now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(m_mainCommandBuffer, 0));

    //Naming it cmd for shorter writing
    VkCommandBuffer cmd = m_mainCommandBuffer;

    // Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;

    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    // Make a clear-color from frame number. This will flash with a 120*pi frame period.
    VkClearValue clearValue;
    float flash = abs(sin(static_cast<float>(m_frameNumber) / 120.f));
    clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

    // Start the main renderpass.
    // We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    VkRenderPassBeginInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.pNext = nullptr;

    rpInfo.renderPass = m_renderPass;
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = m_windowExtent;
    rpInfo.framebuffer = m_framebuffers[swapchainImageIndex];

    // Connect clear values
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_meshPipeline);

    //bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &m_triangleMesh.m_vertexBuffer.m_buffer, &offset);

    //we can now draw the mesh
    vkCmdDraw(cmd, m_triangleMesh.m_vertices.size(), 1, 0, 0);

    //finalize the render pass
    vkCmdEndRenderPass(cmd);

    // Finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    // Prepare the submission to the queue.
    // We want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
    // We will signal the _renderSemaphore, to signal that rendering has finished

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &m_presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &m_renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    // Submit command buffer to the queue and execute it.
    // m_renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

    // This will put the image we just rendered into the visible window.
    // We want to wait on the _renderSemaphore for that, as it's necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &m_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

    // Increase the number of frames drawn
    m_frameNumber++;
}

void ViEngine::run()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        draw();
    }
}

void ViEngine::initCommands()
{
    //create a command pool for commands submitted to the graphics queue.
    //we also want the pool to allow for resetting of individual command buffers
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::commandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool));

    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::commandBufferAllocateInfo(m_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_mainCommandBuffer));

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        });
}

void ViEngine::initSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder{ m_device2, m_surface };
    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .use_default_format_selection()
        //use vsync present mode
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(m_windowExtent.width, m_windowExtent.height)
        .build()
        .value();

    //store swapchain and its related images
    m_swapchain = vkbSwapchain.swapchain;
    m_swapchainImages = vkbSwapchain.get_images().value();
    m_swapchainImageViews = vkbSwapchain.get_image_views().value();

    m_swapchainImageFormat = vkbSwapchain.image_format;

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    });
}

void ViEngine::initVulkan()
{
    vkb::InstanceBuilder builder;
    //make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("ViEngine")
        .request_validation_layers(true)
        .require_api_version(1, 1, 0)
        .use_default_debug_messenger()
        .build();

    auto& vkb_inst = inst_ret.value();

    //store the debug messenger
    m_instance = vkb_inst.instance;

    //store the debug messenger
    m_debug_messenger = vkb_inst.debug_messenger;

    // get the surface of the window we opened with glfw
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface)) {
        throw std::runtime_error("Cannot create GLFW window surface");
    }

    //use vkbootstrap to select a GPU.
    //We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 1)
        .set_surface(m_surface)
        .select()
        .value();

    //create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };

    m_device2 = deviceBuilder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    m_device = m_device2.device;
    m_chosenGPU = physicalDevice.physical_device;

    // use vkbootstrap to get a Graphics queue
    m_graphicsQueue = m_device2.get_queue(vkb::QueueType::graphics).value();
    m_graphicsQueueFamily = m_device2.get_queue_index(vkb::QueueType::graphics).value();

    //Initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_chosenGPU;
    allocatorInfo.device = m_device;
    allocatorInfo.instance = m_instance;
    vmaCreateAllocator(&allocatorInfo, &m_allocator);

    m_mainDeletionQueue.push_function([=, this]() {
        vmaDestroyAllocator(m_allocator);
    });
}

void ViEngine::initDefaultRenderpass()
{
    // The renderpass will use this color attachment.
    VkAttachmentDescription colorAttachment = {};
    // The attachment will have the format needed by the swapchain
    colorAttachment.format = m_swapchainImageFormat;
    // 1 sample, we won't be doing MSAA
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // We Clear when this attachment is loaded
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // We keep the attachment stored when the renderpass ends
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // We don't care about stencil
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // We don't know or care about the starting layout of the attachment
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // After the renderpass ends, the image has to be on a layout ready for display
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference coloraAttachmentRef = {};
    // Attachment number will index into the pAttachments array in the parent renderpass itself
    coloraAttachmentRef.attachment = 0;
    coloraAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // We are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &coloraAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    //connect the color attachment to the info
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    //connect the subpass to the info
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    });
}

void ViEngine::initFramebuffers()
{
    // Create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.pNext = nullptr;

    fbInfo.renderPass = m_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.width = m_windowExtent.width;
    fbInfo.height = m_windowExtent.height;
    fbInfo.layers = 1;

    //grab how many images we have in the swapchain
    const uint32_t swapchain_imagecount = m_swapchainImages.size();
    m_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    //create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++) {
        fbInfo.pAttachments = &m_swapchainImageViews[i];
        VK_CHECK(vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]));

        m_mainDeletionQueue.push_function([=, this]() {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
            vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
            });
    }
}

void ViEngine::initSyncStructures()
{
    VkFenceCreateInfo fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

    VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_renderFence));

    //enqueue the destruction of the fence
    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyFence(m_device, m_renderFence, nullptr);
        });

    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphoreCreateInfo();

    VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore));

    //enqueue the destruction of semaphores
    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
        vkDestroySemaphore(m_device, m_renderSemaphore, nullptr);
        });
}

void ViEngine::initPipelines()
{
    VkShaderModule triangleFragShader;
    if (!loadShaderModule(R"(D:\projekty\Viking\Shaders\triangle.frag.spv)", &triangleFragShader)) {
        std::cout << "Error when building the triangle fragment shader module" << std::endl;
    }
    else {
        std::cout << "Triangle fragment shader successfully loaded" << std::endl;
    }

    VkShaderModule triangleVertexShader;
    if (!loadShaderModule(R"(D:\projekty\Viking\Shaders\triangle.vert.spv)", &triangleVertexShader)) {
        std::cout << "Error when building the triangle vertex shader module" << std::endl;

    }
    else {
        std::cout << "Triangle vertex shader successfully loaded" << std::endl;
    }

    // Build the pipeline layout that controls the inputs/outputs of the shader
    // We are not using descriptor sets or other systems yet, so no need to use anything other than empty default
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();

    VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_trianglePipelineLayout));

    // Build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
    PipelineBuilder pipelineBuilder;

    pipelineBuilder.m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));
    pipelineBuilder.m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

    // Vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    pipelineBuilder.m_vertexInputInfo = vkinit::vertexInputStateCreateInfo();

    // Input assembly is the configuration for drawing triangle lists, strips, or individual points.
    // We are just going to draw triangle list
    pipelineBuilder.m_inputAssembly = vkinit::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    // Build viewport and scissor from the swapchain extents
    pipelineBuilder.m_viewport.x = 0.0f;
    pipelineBuilder.m_viewport.y = 0.0f;
    pipelineBuilder.m_viewport.width = (float)m_windowExtent.width;
    pipelineBuilder.m_viewport.height = (float)m_windowExtent.height;
    pipelineBuilder.m_viewport.minDepth = 0.0f;
    pipelineBuilder.m_viewport.maxDepth = 1.0f;

    pipelineBuilder.m_scissor.offset = { 0, 0 };
    pipelineBuilder.m_scissor.extent = m_windowExtent;

    // Configure the rasterizer to draw filled triangles
    pipelineBuilder.m_rasterizer = vkinit::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

    // We don't use multisampling, so just run the default one
    pipelineBuilder.m_multisampling = vkinit::multisamplingStateCreateInfo();

    // A single blend attachment with no blending and writing to RGBA
    pipelineBuilder.m_colorBlendAttachment = vkinit::colorBlendAttachmentState();

    // Use the triangle layout we created
    pipelineBuilder.m_pipelineLayout = m_trianglePipelineLayout;

    // Finally build the pipeline
    m_trianglePipeline = pipelineBuilder.buildPipeline(m_device, m_renderPass);

    //build the mesh pipeline

    VertexInputDescription vertexDescription = Vertex::getVertexDescription();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipelineBuilder.m_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.m_attributes.data();
    pipelineBuilder.m_vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.m_attributes.size();

    pipelineBuilder.m_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.m_bindings.data();
    pipelineBuilder.m_vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.m_bindings.size();

    //clear the shader stages for the builder
    pipelineBuilder.m_shaderStages.clear();

    //compile mesh vertex shader
    VkShaderModule meshVertShader;
    if (!loadShaderModule(R"(D:\projekty\Viking\Shaders\tri_mesh.vert.spv)", &meshVertShader))
    {
        std::cout << "Error when building the triangle vertex shader module" << std::endl;
    }
    else {
        std::cout << "Red Triangle vertex shader successfully loaded" << std::endl;
    }

    //add the other shaders
    pipelineBuilder.m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

    //make sure that triangleFragShader is holding the compiled colored_triangle.frag
    pipelineBuilder.m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

    //build the mesh triangle pipeline
    m_meshPipeline = pipelineBuilder.buildPipeline(m_device, m_renderPass);

    //deleting all of the vulkan shaders
    vkDestroyShaderModule(m_device, meshVertShader, nullptr);
//    vkDestroyShaderModule(_device, redTriangleVertShader, nullptr);
//    vkDestroyShaderModule(_device, redTriangleFragShader, nullptr);
    vkDestroyShaderModule(m_device, triangleFragShader, nullptr);
    vkDestroyShaderModule(m_device, triangleVertexShader, nullptr);

    //adding the pipelines to the deletion queue
    m_mainDeletionQueue.push_function([=]() {
//        vkDestroyPipeline(m_device, m_redTrianglePipeline, nullptr);
        vkDestroyPipeline(m_device, m_trianglePipeline, nullptr);
        vkDestroyPipeline(m_device, m_meshPipeline, nullptr);

        vkDestroyPipelineLayout(m_device, m_trianglePipelineLayout, nullptr);
    });
}

bool ViEngine::loadShaderModule(const char* t_filePath, VkShaderModule* t_outShaderModule) const
{
    // Open the file. With cursor at the end
    std::ifstream file(t_filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // Find what the size of the file is by looking up the location of the cursor
    //because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = (size_t)file.tellg();

    // Spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // Put file cursor at beginning
    file.seekg(0);

    // Load the entire file into the buffer
    file.read((char*)buffer.data(), static_cast<std::streamsize>(fileSize));

    // Now that the file is loaded into the buffer, we can close it
    file.close();

    // Create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // CodeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    // Check that the creation goes well.
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }
    *t_outShaderModule = shaderModule;
    return true;
}

void ViEngine::loadMeshes()
{
    // Make the array 3 vertices long
    m_triangleMesh.m_vertices.resize(3);

    // Vertex positions
    m_triangleMesh.m_vertices[0].position = { 1.f, 1.f, 0.0f };
    m_triangleMesh.m_vertices[1].position = {-1.f, 1.f, 0.0f };
    m_triangleMesh.m_vertices[2].position = { 0.f,-1.f, 0.0f };

    //vertex colors, all green
    m_triangleMesh.m_vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
    m_triangleMesh.m_vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
    m_triangleMesh.m_vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green

    // We don't care about the vertex normals

    uploadMesh(m_triangleMesh);
}

void ViEngine::uploadMesh(Mesh &t_mesh)
{
    // Allocate vertex buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // This is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = t_mesh.m_vertices.size() * sizeof(Vertex);
    // This buffer is going to be used as a Vertex Buffer
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


    // Let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    // Allocate the buffer
    VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaAllocInfo,
                             &t_mesh.m_vertexBuffer.m_buffer,
                             &t_mesh.m_vertexBuffer.m_allocation,
                             nullptr));

    // Add the destruction of triangle mesh buffer to the deletion queue
    m_mainDeletionQueue.push_function([=, this]() {
        vmaDestroyBuffer(m_allocator, t_mesh.m_vertexBuffer.m_buffer, t_mesh.m_vertexBuffer.m_allocation);
    });

    void* data;
    vmaMapMemory(m_allocator, t_mesh.m_vertexBuffer.m_allocation, &data);

    memcpy(data, t_mesh.m_vertices.data(), t_mesh.m_vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(m_allocator, t_mesh.m_vertexBuffer.m_allocation);
}

VkPipeline PipelineBuilder::buildPipeline(VkDevice t_device, VkRenderPass t_pass)
{
    // Make viewport state from our stored viewport and scissor.
    // at the moment we won't support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &m_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &m_scissor;

    // Setup dummy color blending. We aren't using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &m_colorBlendAttachment;

    // Build the actual pipeline
    // We now use all the info structs we have been writing into into this one to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = m_shaderStages.size();
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &m_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizer;
    pipelineInfo.pMultisampleState = &m_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = t_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    //it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(t_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
        std::cout << "failed to create pipeline\n";
        return VK_NULL_HANDLE; // failed to create graphics pipeline
    }
    else {
        return newPipeline;
    }
}
