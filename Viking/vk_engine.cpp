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

#include <glm/gtx/transform.hpp>

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
    //Wait until the gpu has finished rendering the last frame. Timeout of 1 second
    VK_CHECK(vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000));
    VK_CHECK(vkResetFences(m_device, 1, &m_renderFence));

    //now that we are sure that the commands finished executing,
    // we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(m_mainCommandBuffer, 0));

    //request image from the swap-chain
    uint32_t swapchainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_presentSemaphore, nullptr, &swapchainImageIndex));

    //naming it cmd for shorter writing
    auto cmd = m_mainCommandBuffer;

    //Begin the command buffer recording.
    // We will use this command buffer exactly once, so we want to let vulkan know that
    auto cmdBeginInfo = vkinit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    //Make a clear-color from frame number. This will flash with a 120-frame period.
    VkClearValue clearValue;
    auto flash = abs(sin(static_cast<float>(m_frameNumber) / 120.f));
    clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

    //clear depth at 1
    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;

    //Start the main render-pass.
    //We will use the clear color from above, and the framebuffer of the index the swap-chain gave us
    auto rpInfo = vkinit::renderpassBeginInfo(m_renderPass, m_windowExtent, m_framebuffers[swapchainImageIndex]);

    //connect clear values
    rpInfo.clearValueCount = 2;

    VkClearValue clearValues[] = { clearValue, depthClear };

    rpInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);


    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_meshPipeline);

    //bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &m_monkeyMesh.m_vertexBuffer.m_buffer, &offset);

    //make a model view matrix for rendering the object
    //camera position
    glm::vec3 camPos = { 0.f,0.f,-2.f };

    auto view = glm::translate(glm::mat4(1.f), camPos);
    //camera projection
    auto projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;
    //model rotation
    auto model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(static_cast<float>(m_frameNumber) * 0.4f), glm::vec3(0, 1, 0));

    //calculate final mesh matrix
    auto mesh_matrix = projection * view * model;

    MeshPushConstants constants{};
    constants.m_rendererMatrix = mesh_matrix;

    //upload the matrix to the gpu via pushconstants
    vkCmdPushConstants(cmd, m_meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

    //we can now draw the mesh
    vkCmdDraw(cmd, static_cast<uint32_t>(m_monkeyMesh.m_vertices.size()), 1, 0, 0);

    //finalize the render pass
    vkCmdEndRenderPass(cmd);
    //finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    //prepare the submission to the queue.
    //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
    //we will signal the _renderSemaphore, to signal that rendering has finished

    auto submit = vkinit::submitInfo(&cmd);
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &m_presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &m_renderSemaphore;

    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

    //prepare present
    // this will put the image we just rendered to into the visible window.
    // we want to wait on the _renderSemaphore for that,
    // as it's necessary that drawing commands have finished before the image is displayed to the user
    auto presentInfo = vkinit::presentInfo();

    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &m_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

    //increase the number of frames drawn
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
    auto commandPoolInfo = vkinit::commandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool));

    //allocate the default command buffer that we will use for rendering
    auto cmdAllocInfo = vkinit::commandBufferAllocateInfo(m_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_mainCommandBuffer));

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        });
}

void ViEngine::initSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder{m_chosenGPU, m_device, m_surface };

    auto vkbSwapchain = swapchainBuilder
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

    //depth image size will match the window
    VkExtent3D depthImageExtent = {
            m_windowExtent.width,
            m_windowExtent.height,
            1
    };

    //hard coding the depth format to 32-bit float
    m_depthFormat = VK_FORMAT_D32_SFLOAT;

    //the depth image will be an image with the format we selected and Depth Attachment usage flag
    auto dimgInfo = vkinit::imageCreateInfo(m_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    //for the depth image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo dimgAllocinfo = {};
    dimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimgAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    vmaCreateImage(m_allocator, &dimgInfo, &dimgAllocinfo, &m_depthImage.m_image, &m_depthImage.m_allocation, nullptr);

    //build an image-view for the depth image to use for rendering
    auto dviewInfo = vkinit::imageviewCreateInfo(m_depthFormat, m_depthImage.m_image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(m_device, &dviewInfo, nullptr, &m_depthImageView));

    //add to deletion queues
    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyImageView(m_device, m_depthImageView, nullptr);
        vmaDestroyImage(m_allocator, m_depthImage.m_image, m_depthImage.m_allocation);
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

    //Use vkbootstrap to select a GPU.
    //We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto physicalDevice = selector
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
    //we define an attachment description for our main color image
    //the attachment is loaded as "clear" when renderpass start
    //the attachment is stored when renderpass ends
    //the attachment layout starts as "undefined", and transitions to "Present" so it's possible to display it
    //we don't care about stencil, and don't use multisampling

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    // Depth attachment
    depthAttachment.flags = 0;
    depthAttachment.format = m_depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    //hook the depth attachment into the subpass
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    //1 dependency, which is from "outside" into the subpass. And we can read or write color
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //dependency from outside to the subpass, making this subpass dependent on the previous renderpasses
    VkSubpassDependency depthDependency = {};
    depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDependency.dstSubpass = 0;
    depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.srcAccessMask = 0;
    depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //array of 2 dependencies, one for color, two for depth
    VkSubpassDependency dependencies[2] = {dependency, depthDependency };

    //array of 2 attachments, one for the color, and the other for depth
    VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment };

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    //2 attachments from an attachment array
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0];
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    //2 dependencies from dependency array
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = &dependencies[0];

    VK_CHECK(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_renderPass));

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    });
}

void ViEngine::initFramebuffers()
{
    // Create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    auto fbInfo = vkinit::framebufferCreateInfo(m_renderPass, m_windowExtent);

    const auto swapchainImagecount = m_swapchainImages.size();
    m_framebuffers = std::vector<VkFramebuffer>(swapchainImagecount);

    for (int i = 0; i < swapchainImagecount; i++) {

        VkImageView attachments[2];
        attachments[0] = m_swapchainImageViews[i];
        attachments[1] = m_depthImageView;

        fbInfo.pAttachments = attachments;
        fbInfo.attachmentCount = 2;
        VK_CHECK(vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]));

        m_mainDeletionQueue.push_function([=, this]() {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
            vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
        });
    }
}

void ViEngine::initSyncStructures()
{
    auto fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

    VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_renderFence));

    //enqueue the destruction of the fence
    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyFence(m_device, m_renderFence, nullptr);
        });

    auto semaphoreCreateInfo = vkinit::semaphoreCreateInfo();

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

    //build the pipeline layout that controls the inputs/outputs of the shader
    //we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
    auto pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();

    VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_trianglePipelineLayout));

    //Build the stage-create-info for both vertex and fragment stages.
    // This lets the pipeline know the shader modules per stage
    PipelineBuilder pipelineBuilder;

    pipelineBuilder.m_shaderStages.emplace_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));
    pipelineBuilder.m_shaderStages.emplace_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));


    //Vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    pipelineBuilder.m_vertexInputInfo = vkinit::vertexInputStateCreateInfo();

    //input assembly is the configuration for drawing triangle lists, strips, or individual points.
    //we are just going to draw a triangle list
    pipelineBuilder.m_inputAssembly = vkinit::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    //build viewport and scissor from the swapchain extents
    pipelineBuilder.m_viewport.x = 0.0f;
    pipelineBuilder.m_viewport.y = 0.0f;
    pipelineBuilder.m_viewport.width = (float)m_windowExtent.width;
    pipelineBuilder.m_viewport.height = (float)m_windowExtent.height;
    pipelineBuilder.m_viewport.minDepth = 0.0f;
    pipelineBuilder.m_viewport.maxDepth = 1.0f;

    pipelineBuilder.m_scissor.offset = { 0, 0 };
    pipelineBuilder.m_scissor.extent = m_windowExtent;

    //configure the rasterizer to draw filled triangles
    pipelineBuilder.m_rasterizer = vkinit::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

    //we don't use multisampling, so run the default one
    pipelineBuilder.m_multisampling = vkinit::multisamplingStateCreateInfo();

    //a single blend attachment with no blending and writing to RGBA
    pipelineBuilder.m_colorBlendAttachment = vkinit::colorBlendAttachmentState();

    //use the triangle layout we created
    pipelineBuilder.m_pipelineLayout = m_trianglePipelineLayout;


    //default depth-testing
    pipelineBuilder.m_depthStencil = vkinit::depthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    //finally, build the pipeline
    m_trianglePipeline = pipelineBuilder.buildPipeline(m_device, m_renderPass);

    //clear the shader stages for the builder
    pipelineBuilder.m_shaderStages.clear();

    //build the mesh pipeline
    auto vertexDescription = Vertex::getVertexDescription();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipelineBuilder.m_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.m_attributes.data();
    pipelineBuilder.m_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexDescription.m_attributes.size());

    pipelineBuilder.m_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.m_bindings.data();
    pipelineBuilder.m_vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexDescription.m_bindings.size());

    //clear the shader stages for the builder
    pipelineBuilder.m_shaderStages.clear();

    //compile mesh vertex shader
    VkShaderModule meshVertShader;
    if (!loadShaderModule(R"(D:\projekty\Viking\Shaders\tri_mesh.vert.spv)", &meshVertShader)) {
        std::cout << "Error when building the triangle vertex shader module" << std::endl;
    }
    else {
        std::cout << "Red Triangle vertex shader successfully loaded" << std::endl;
    }

    //add the other shaders
    pipelineBuilder.m_shaderStages.emplace_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

    //make sure that triangleFragShader is holding the compiled colored_triangle.frag
    pipelineBuilder.m_shaderStages.emplace_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

    //we start from just the default empty pipeline layout info
    VkPipelineLayoutCreateInfo meshPipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();

    //setup push constants
    VkPushConstantRange pushConstant;
    //offset 0
    pushConstant.offset = 0;
    //size of a MeshPushConstant struct
    pushConstant.size = sizeof(MeshPushConstants);
    //for the vertex shader
    pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    meshPipelineLayoutInfo.pPushConstantRanges = &pushConstant;
    meshPipelineLayoutInfo.pushConstantRangeCount = 1;

    VK_CHECK(vkCreatePipelineLayout(m_device, &meshPipelineLayoutInfo, nullptr, &m_meshPipelineLayout));

    //hook the push constants layout
    pipelineBuilder.m_pipelineLayout = m_meshPipelineLayout;
    //build the mesh triangle pipeline
    m_meshPipeline = pipelineBuilder.buildPipeline(m_device, m_renderPass);

    vkDestroyShaderModule(m_device, meshVertShader, nullptr);
    vkDestroyShaderModule(m_device, triangleFragShader, nullptr);
    vkDestroyShaderModule(m_device, triangleVertexShader, nullptr);

    m_mainDeletionQueue.push_function([=, this]() {
        vkDestroyPipeline(m_device, m_trianglePipeline, nullptr);
        vkDestroyPipeline(m_device, m_meshPipeline, nullptr);

        vkDestroyPipelineLayout(m_device, m_trianglePipelineLayout, nullptr);
        vkDestroyPipelineLayout(m_device, m_meshPipelineLayout, nullptr);
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

    // Put file cursor at the beginning
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

    //Load the monkey
    m_monkeyMesh.loadFromObj(R"(D:\projekty\Viking\Assets\monkey_smooth.obj)");

    uploadMesh(m_triangleMesh);
    uploadMesh(m_monkeyMesh);
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
    //make viewport state from our stored viewport and scissor.
    //at the moment, we won't support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &m_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &m_scissor;

    //Setup placeholder color blending.
    // We aren't using transparent objects, yet
    //the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &m_colorBlendAttachment;

    //build the actual pipeline
    //we now use all the info structs we have been writing into this one to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineInfo.pStages = m_shaderStages.data();
    pipelineInfo.pVertexInputState = &m_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizer;
    pipelineInfo.pMultisampleState = &m_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &m_depthStencil;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = t_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    //it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(t_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
        std::cout << "failed to create pipeline\n";
        return VK_NULL_HANDLE; // failed to create a graphics pipeline
    }
    else {
        return newPipeline;
    }
}
