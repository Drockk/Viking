#include "vk_engine.hpp"
#include "vk_initializers.hpp"
#include "vk_textures.hpp"
#include "vk_types.hpp"

#include "Core/DeletionQueue.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Shader.hpp"

#include "VkBootstrap.h"

#include "Core/Log.hpp"

#include <iostream>

//We want to immediately abort when there is an error.
// In normal engines, this would give an error message to the user, or perform a dump of state.
using namespace std;
#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            std::cout <<"Detected Vulkan error: " << err << std::endl;  \
            abort();                                                    \
        }                                                               \
    } while (0)


void ViEngine::init()
{
    vi::Log::init();

    m_window = std::make_unique<vi::Window>("Vi Engine", std::pair{m_window_extent.width, m_window_extent.height});

    vi::GraphicsContext::init("Viking Engine", m_window);

    m_swapchain = std::make_unique<vi::Swapchain>(std::pair{ m_window_extent.width, m_window_extent.height });

    //init_swapchain();
    init_default_renderpass();
    init_framebuffers();
    init_descriptors();
    init_pipelines();

    load_images();
    load_meshes();
    init_scene();

    //everything went fine
    m_is_initialized = true;
}
void ViEngine::cleanup() const {
    if (m_is_initialized) {
        vi::GraphicsContext::wait_for_device();

        vi::DeletionQueue::flush();

        vi::GraphicsContext::cleanup();
    }
}

void ViEngine::draw()
{
    vi::GraphicsContext::start_frame();
    const auto device = vi::GraphicsContext::get_device();
    const auto& frame = vi::GraphicsContext::get_current_frame();

    //Wait until the gpu has finished rendering the last frame. Timeout of 1 second
    VK_CHECK(vkWaitForFences(device, 1, &frame.m_render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(device, 1, &frame.m_render_fence));

    //now that we are sure that the commands finished executing,
    // we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(frame.m_main_command_buffer, 0));

    //request image from the swapchain
    uint32_t swapchain_image_index{};
    const auto swapchain = m_swapchain->get_swapchain();
    VK_CHECK(vkAcquireNextImageKHR(device, swapchain, 1000000000, frame.m_present_semaphore, nullptr, &swapchain_image_index));

    //Naming it cmd for shorter writing
    const auto cmd = frame.m_main_command_buffer;

    //begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
    const auto cmd_begin_info = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    //make a clear-color from frame number. This will flash with a 120 frame period.
    VkClearValue clear_value;
    const auto flash = abs(sin(static_cast<float>(m_frame_number) / 120.f));
    clear_value.color = { { 0.0f, 0.0f, flash, 1.0f } };

    //clear depth at 1
    VkClearValue depth_clear;
    depth_clear.depthStencil.depth = 1.f;

    //start the main renderpass.
    //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    auto rp_info = vkinit::renderpass_begin_info(m_render_pass, m_window_extent, m_framebuffers[swapchain_image_index]);

    //connect clear values
    rp_info.clearValueCount = 2;

    const VkClearValue clear_values[] = { clear_value, depth_clear };

    rp_info.pClearValues = &clear_values[0];

    vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    draw_objects(cmd, m_renderables.data(), static_cast<int>(m_renderables.size()));

    //finalize the render pass
    vkCmdEndRenderPass(cmd);
    //finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    //prepare the submission to the queue.
    //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
    //we will signal the _renderSemaphore, to signal that rendering has finished

    auto submit = vkinit::submit_info(&cmd);
    constexpr VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &wait_stage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &frame.m_present_semaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &frame.m_render_semaphore;

    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    const auto graphics_queue = vi::GraphicsContext::get_graphics_queue();
    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit, frame.m_render_fence));

    //prepare present
    // this will put the image we just rendered to into the visible window.
    // we want to wait on the _renderSemaphore for that,
    // as its necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR present_info = vkinit::present_info();

    present_info.pSwapchains = &swapchain;
    present_info.swapchainCount = 1;

    present_info.pWaitSemaphores = &frame.m_render_semaphore;
    present_info.waitSemaphoreCount = 1;

    present_info.pImageIndices = &swapchain_image_index;

    VK_CHECK(vkQueuePresentKHR(graphics_queue, &present_info));

    vi::GraphicsContext::end_frame();
}

void ViEngine::run()
{
    while (!m_window->should_close()) {
        m_window->on_update();
        draw();
    }
}

//void ViEngine::init_swapchain()
//{
//    const auto device = vi::GraphicsContext::get_device();
//    const auto chosen_gpu = vi::GraphicsContext::get_chosen_gpu();
//    const auto surface = vi::GraphicsContext::get_surface();
//
//    vkb::SwapchainBuilder swapchain_builder{ chosen_gpu, device, surface };
//
//    auto vkb_swapchain = swapchain_builder
//            .use_default_format_selection()
//                    //use vsync present mode
//            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
//            .set_desired_extent(m_window_extent.width, m_window_extent.height)
//            .build()
//            .value();
//
//    //store swapchain and its related images
//    m_swapchain = vkb_swapchain.swapchain;
//    m_swapchain_images = vkb_swapchain.get_images().value();
//    m_swapchain_image_views = vkb_swapchain.get_image_views().value();
//
//    m_swachain_image_format = vkb_swapchain.image_format;
//
//    vi::DeletionQueue::push_function([this, device] {
//        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
//    });
//
//    //depth image size will match the window
//    VkExtent3D depth_image_extent = {
//            m_window_extent.width,
//            m_window_extent.height,
//            1
//    };
//
//    //hardcoding the depth format to 32 bit float
//    m_depth_format = VK_FORMAT_D32_SFLOAT;
//
//    //the depth image will be a image with the format we selected and Depth Attachment usage flag
//    auto dimg_info = vkinit::image_create_info(m_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depth_image_extent);
//
//    //for the depth image, we want to allocate it from gpu local memory
//    VmaAllocationCreateInfo dimg_allocinfo{};
//    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//    dimg_allocinfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//    //allocate and create the image
//    auto allocator = vi::GraphicsContext::get_allocator();
//    vmaCreateImage(allocator, &dimg_info, &dimg_allocinfo, &m_depth_image.m_image, &m_depth_image.m_allocation, nullptr);
//
//    //build a image-view for the depth image to use for rendering
//    VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(m_depth_format, m_depth_image.m_image, VK_IMAGE_ASPECT_DEPTH_BIT);
//
//    VK_CHECK(vkCreateImageView(device, &dview_info, nullptr, &m_depth_image_view));
//
//    vi::DeletionQueue::push_function([this, allocator, device] {
//        vkDestroyImageView(device, m_depth_image_view, nullptr);
//        vmaDestroyImage(allocator, m_depth_image.m_image, m_depth_image.m_allocation);
//    });
//}

void ViEngine::init_default_renderpass()
{
    //we define an attachment description for our main color image
    //the attachment is loaded as "clear" when renderpass start
    //the attachment is stored when renderpass ends
    //the attachment layout starts as "undefined", and transitions to "Present" so its possible to display it
    //we don't care about stencil, and don't use multisampling

    const auto& swapchain_format = m_swapchain->get_swapchain_format();

    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapchain_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    const auto& depth_format = m_swapchain->get_depth_format();

    VkAttachmentDescription depth_attachment{};
    depth_attachment.flags = 0;
    depth_attachment.format = depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    //hook the depth attachment into the subpass
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    //1 dependency, which is from "outside" into the subpass. And we can read or write color
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //dependency from outside to the subpass, making this subpass dependent on the previous renderpasses
    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass = 0;
    depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //array of 2 dependencies, one for color, two for depth
    VkSubpassDependency dependencies[2] = { dependency, depth_dependency };

    //array of 2 attachments, one for the color, and other for depth
    VkAttachmentDescription attachments[2] = { color_attachment,depth_attachment };

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    //2 attachments from attachment array
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0];
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    //2 dependencies from dependency array
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = &dependencies[0];

    const auto device = vi::GraphicsContext::get_device();
    VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &m_render_pass));

    vi::DeletionQueue::push_function([this, device] {
        vkDestroyRenderPass(device, m_render_pass, nullptr);
    });
}

void ViEngine::init_framebuffers()
{
    //create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    auto fb_info = vkinit::framebuffer_create_info(m_render_pass, m_window_extent);

    const auto& swapchain_images = m_swapchain->get_swapchain_images();
    const auto& swapchain_images_views = m_swapchain->get_swapchain_images_views();
    const auto swapchain_image_count = swapchain_images.size();
    m_framebuffers = std::vector<VkFramebuffer>(swapchain_image_count);

    for (auto i = 0; i < swapchain_image_count; i++) {

        VkImageView attachments[2];
        attachments[0] = swapchain_images_views[i];
        attachments[1] = m_swapchain->get_depth_image_view();

        fb_info.pAttachments = attachments;
        fb_info.attachmentCount = 2;
        const auto device = vi::GraphicsContext::get_device();
        VK_CHECK(vkCreateFramebuffer(device, &fb_info, nullptr, &m_framebuffers[i]));

        vi::DeletionQueue::push_function([=, this] {
            vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
            vkDestroyImageView(device, swapchain_images_views[i], nullptr);
        });
    }
}

void ViEngine::init_pipelines()
{
    const auto device = vi::GraphicsContext::get_device();
    auto colored_mesh_shader = std::make_unique<vi::Shader>(device, R"(C:\Users\batzi\source\repos\Viking\Shaders\colored_mesh.shader)");
    auto textured_mesh_shader = std::make_unique<vi::Shader>(device, R"(C:\Users\batzi\source\repos\Viking\Shaders\textured_mesh.shader)");

    //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
    PipelineBuilder pipeline_builder;
    pipeline_builder.m_shader_stages.emplace_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, colored_mesh_shader->get_shader_module(vi::ShaderType::VERTEX)));
    pipeline_builder.m_shader_stages.emplace_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, colored_mesh_shader->get_shader_module(vi::ShaderType::FRAGMENT)));

    //we start from just the default empty pipeline layout info
    VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

    //setup push constants
    VkPushConstantRange push_constant;
    //offset 0
    push_constant.offset = 0;
    //size of a MeshPushConstant struct
    push_constant.size = sizeof(MeshPushConstants);
    //for the vertex shader
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;
    mesh_pipeline_layout_info.pushConstantRangeCount = 1;

    VkDescriptorSetLayout set_layouts[] = { m_global_set_layout, m_object_set_layout };

    mesh_pipeline_layout_info.setLayoutCount = 2;
    mesh_pipeline_layout_info.pSetLayouts = set_layouts;

    VkPipelineLayout mesh_pipeline_layout{nullptr};
    VK_CHECK(vkCreatePipelineLayout(device, &mesh_pipeline_layout_info, nullptr, &mesh_pipeline_layout));

    //we start from the normal mesh layout
    VkPipelineLayoutCreateInfo textured_pipeline_layout_info = mesh_pipeline_layout_info;

    VkDescriptorSetLayout textured_set_layouts[] = { m_global_set_layout, m_object_set_layout,m_single_texture_set_layout };

    textured_pipeline_layout_info.setLayoutCount = 3;
    textured_pipeline_layout_info.pSetLayouts = textured_set_layouts;

    VkPipelineLayout textured_pipeline_layout;
    VK_CHECK(vkCreatePipelineLayout(device, &textured_pipeline_layout_info, nullptr, &textured_pipeline_layout));

    //hook the push constants layout
    pipeline_builder.m_pipeline_layout = mesh_pipeline_layout;

    //vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    pipeline_builder.m_vertex_input_info = vkinit::vertex_input_state_create_info();

    //input assembly is the configuration for drawing triangle lists, strips, or individual points.
    //we are just going to draw triangle list
    pipeline_builder.m_input_assembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    //build viewport and scissor from the swapchain extents
    pipeline_builder.m_viewport.x = 0.0f;
    pipeline_builder.m_viewport.y = 0.0f;
    pipeline_builder.m_viewport.width = static_cast<float>(m_window_extent.width);
    pipeline_builder.m_viewport.height = static_cast<float>(m_window_extent.height);
    pipeline_builder.m_viewport.minDepth = 0.0f;
    pipeline_builder.m_viewport.maxDepth = 1.0f;

    pipeline_builder.m_scissor.offset = { 0, 0 };
    pipeline_builder.m_scissor.extent = m_window_extent;

    //configure the rasterizer to draw filled triangles
    pipeline_builder.m_rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    //we don't use multisampling, so just run the default one
    pipeline_builder.m_multisampling = vkinit::multisampling_state_create_info();

    //a single blend attachment with no blending and writing to RGBA
    pipeline_builder.m_color_blend_attachment = vkinit::color_blend_attachment_state();

    //default depthtesting
    pipeline_builder.m_depth_stencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    //build the mesh pipeline

    auto [bindings, attributes, flags] = Vertex::get_vertex_description();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipeline_builder.m_vertex_input_info.pVertexAttributeDescriptions = attributes.data();
    pipeline_builder.m_vertex_input_info.vertexAttributeDescriptionCount = attributes.size();

    pipeline_builder.m_vertex_input_info.pVertexBindingDescriptions = bindings.data();
    pipeline_builder.m_vertex_input_info.vertexBindingDescriptionCount = bindings.size();

    //build the mesh triangle pipeline
    auto mesh_pipeline = pipeline_builder.build_pipeline(device, m_render_pass);

    create_material(mesh_pipeline, mesh_pipeline_layout, "defaultmesh");

    pipeline_builder.m_shader_stages.clear();

    pipeline_builder.m_shader_stages.emplace_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, textured_mesh_shader->get_shader_module(vi::ShaderType::VERTEX)));

    pipeline_builder.m_shader_stages.emplace_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, textured_mesh_shader->get_shader_module(vi::ShaderType::FRAGMENT)));

    pipeline_builder.m_pipeline_layout = textured_pipeline_layout;
    VkPipeline texPipeline = pipeline_builder.build_pipeline(device, m_render_pass);
    create_material(texPipeline, textured_pipeline_layout, "texturedmesh");

    vi::DeletionQueue::push_function([=, this] {
        vkDestroyPipeline(device, mesh_pipeline, nullptr);
        vkDestroyPipeline(device, texPipeline, nullptr);

        vkDestroyPipelineLayout(device, mesh_pipeline_layout, nullptr);
        vkDestroyPipelineLayout(device, textured_pipeline_layout, nullptr);
    });
}

VkPipeline PipelineBuilder::build_pipeline(const VkDevice p_device, const VkRenderPass p_pass) const {
    //make viewport state from our stored viewport and scissor.
    //at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &m_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &m_scissor;

    //setup dummy color blending. We arent using transparent objects yet
    //the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &m_color_blend_attachment;

    //build the actual pipeline
    //we now use all of the info structs we have been writing into into this one to create the pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = m_shader_stages.size();
    pipelineInfo.pStages = m_shader_stages.data();
    pipelineInfo.pVertexInputState = &m_vertex_input_info;
    pipelineInfo.pInputAssemblyState = &m_input_assembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &m_rasterizer;
    pipelineInfo.pMultisampleState = &m_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &m_depth_stencil;
    pipelineInfo.layout = m_pipeline_layout;
    pipelineInfo.renderPass = p_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    //its easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(
            p_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
        VI_CORE_ERROR("Failed to create pipeline");
        return VK_NULL_HANDLE; // failed to create graphics pipeline
    }

    return newPipeline;
}

void ViEngine::load_meshes()
{
    Mesh triMesh{};
    //make the array 3 vertices long
    triMesh.vertices.resize(3);

    //vertex positions
    triMesh.vertices[0].position = { 1.f,1.f, 0.0f };
    triMesh.vertices[1].position = { -1.f,1.f, 0.0f };
    triMesh.vertices[2].position = { 0.f,-1.f, 0.0f };

    //vertex colors, all green
    triMesh.vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
    triMesh.vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
    triMesh.vertices[2].color = { 0.f,1.f, 0.0f }; //pure green
    //we don't care about the vertex normals

    //load the monkey
    Mesh monkeyMesh{};
    monkeyMesh.load_from_obj(R"(C:\Users\batzi\source\repos\Viking\Assets\monkey_smooth.obj)");

    Mesh lostEmpire{};
    lostEmpire.load_from_obj(R"(C:\Users\batzi\source\repos\Viking\Assets\lost_empire.obj)");

    upload_mesh(triMesh);
    upload_mesh(monkeyMesh);
    upload_mesh(lostEmpire);

    m_meshes["monkey"] = monkeyMesh;
    m_meshes["triangle"] = triMesh;
    m_meshes["empire"] = lostEmpire;
}


void ViEngine::load_images()
{
    Texture lostEmpire;
    const auto device = vi::GraphicsContext::get_device();

    vkutil::load_image_from_file(*this, R"(C:\Users\batzi\source\repos\Viking\Assets\lost_empire-RGBA.png)", lostEmpire.m_image);

    const auto imageinfo = vkinit::imageview_create_info(VK_FORMAT_R8G8B8A8_SRGB, lostEmpire.m_image.m_image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(device, &imageinfo, nullptr, &lostEmpire.m_image_view);

    vi::DeletionQueue::push_function([=, this] {
        vkDestroyImageView(device, lostEmpire.m_image_view, nullptr);
    });

    m_loaded_textures["empire_diffuse"] = lostEmpire;
}

void ViEngine::upload_mesh(Mesh& p_mesh) const {
    const auto bufferSize = p_mesh.vertices.size() * sizeof(Vertex);
    //allocate vertex buffer
    VkBufferCreateInfo stagingBufferInfo{};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.pNext = nullptr;
    //this is the total size, in bytes, of the buffer we are allocating
    stagingBufferInfo.size = bufferSize;
    //this buffer is going to be used as a Vertex Buffer
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;


    //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo{};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    AllocatedBuffer stagingBuffer;

    //allocate the buffer
    const auto allocator = vi::GraphicsContext::get_allocator();
    VK_CHECK(vmaCreateBuffer(allocator, &stagingBufferInfo, &vmaallocInfo,
                             &stagingBuffer.m_buffer,
                             &stagingBuffer.m_allocation,
                             nullptr));

    //copy vertex data
    void* data;
    vmaMapMemory(allocator, stagingBuffer.m_allocation, &data);

    memcpy(data, p_mesh.vertices.data(), p_mesh.vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(allocator, stagingBuffer.m_allocation);


    //allocate vertex buffer
    VkBufferCreateInfo vertexBufferInfo{};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.pNext = nullptr;
    //this is the total size, in bytes, of the buffer we are allocating
    vertexBufferInfo.size = bufferSize;
    //this buffer is going to be used as a Vertex Buffer
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    //let the VMA library know that this data should be gpu native
    vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    //allocate the buffer
    VK_CHECK(vmaCreateBuffer(allocator, &vertexBufferInfo, &vmaallocInfo,
                             &p_mesh.vertexBuffer.m_buffer,
                             &p_mesh.vertexBuffer.m_allocation,
                             nullptr));
    //add the destruction of triangle mesh buffer to the deletion queue
    vi::DeletionQueue::push_function([=, this] {
        vmaDestroyBuffer(allocator, p_mesh.vertexBuffer.m_buffer, p_mesh.vertexBuffer.m_allocation);
    });

    immediate_submit([=](const VkCommandBuffer cmd) {
        VkBufferCopy copy;
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size = bufferSize;
        vkCmdCopyBuffer(cmd, stagingBuffer.m_buffer, p_mesh.vertexBuffer.m_buffer, 1, & copy);
    });

    vmaDestroyBuffer(allocator, stagingBuffer.m_buffer, stagingBuffer.m_allocation);
}


Material* ViEngine::create_material(const VkPipeline p_pipeline, const VkPipelineLayout p_layout, const std::string& p_name)
{
    Material mat;
    mat.m_pipeline = p_pipeline;
    mat.m_pipeline_layout = p_layout;
    m_materials[p_name] = mat;
    return &m_materials[p_name];
}

Material* ViEngine::get_material(const std::string& p_name)
{
    //search for the object, and return nullpointer if not found
    const auto it = m_materials.find(p_name);
    if (it == m_materials.end()) {
        return nullptr;
    }

    return &it->second;
}


Mesh* ViEngine::get_mesh(const std::string& p_name)
{
    const auto it = m_meshes.find(p_name);
    if (it == m_meshes.end()) {
        return nullptr;
    }

    return &it->second;
}


void ViEngine::draw_objects(const VkCommandBuffer p_cmd, const RenderObject* p_first, const int p_count)
{
    //make a model view matrix for rendering the object
    //camera view
    constexpr glm::vec3 camPos = { 0.f,-6.f,-10.f };

    const auto view = translate(glm::mat4(1.f), camPos);
    //camera projection
    auto projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;

    GPUCameraData camData;
    camData.m_proj = projection;
    camData.m_view = view;
    camData.m_view_proj = projection * view;

    const auto& frame = vi::GraphicsContext::get_current_frame();
    const auto& camera_buffer = frame.m_camera_buffer;
    camera_buffer->copy_data_to_buffer(&camData, sizeof(GPUCameraData));

    const auto framed = static_cast<float>(m_frame_number) / 120.f;

    m_scene_parameters.m_ambient_color = { sin(framed),0,cos(framed),1 };

    const auto frameIndex = m_frame_number % FRAME_OVERLAP;

    m_scene_parameter_buffer->copy_data_to_buffer(&m_scene_parameters, sizeof(GPUSceneData), pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex);

    void* objectData;
    auto& object_buffer = frame.m_object_buffer;
    object_buffer->map_memory(&objectData);

    auto objectSSBO = static_cast<GPUObjectData*>(objectData);

    for (auto i = 0; i < p_count; i++)
    {
        const RenderObject& object = p_first[i];
        objectSSBO[i].m_model_matrix = object.m_transform_matrix;
    }

    object_buffer->unmap_memory();

    const Mesh* lastMesh = nullptr;
    const Material* lastMaterial = nullptr;

    for (auto i = 0; i < p_count; i++)
    {
        const auto& object = p_first[i];

        //only bind the pipeline if it doesnt match with the already bound one
        if (object.m_material != lastMaterial) {

            vkCmdBindPipeline(p_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.m_material->m_pipeline);
            lastMaterial = object.m_material;

            uint32_t uniform_offset = pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;
            vkCmdBindDescriptorSets(p_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.m_material->m_pipeline_layout, 0, 1, &frame.m_global_descriptor, 1, &uniform_offset);

            //object data descriptor
            vkCmdBindDescriptorSets(p_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.m_material->m_pipeline_layout, 1, 1, &frame.m_object_descriptor, 0, nullptr);

            if (object.m_material->m_texture_set != VK_NULL_HANDLE) {
                //texture descriptor
                vkCmdBindDescriptorSets(p_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.m_material->m_pipeline_layout, 2, 1, &object.m_material->m_texture_set, 0, nullptr);

            }
        }

        const auto model = object.m_transform_matrix;
        //final render matrix, that we are calculating on the cpu
        const auto mesh_matrix = model;

        MeshPushConstants constants;
        constants.m_render_matrix = mesh_matrix;

        //upload the mesh to the gpu via pushconstants
        vkCmdPushConstants(p_cmd, object.m_material->m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

        //only bind the mesh if its a different one from last bind
        if (object.m_mesh != lastMesh) {
            //bind the mesh vertex buffer with offset 0
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(p_cmd, 0, 1, &object.m_mesh->vertexBuffer.m_buffer, &offset);
            lastMesh = object.m_mesh;
        }
        //we can now draw
        vkCmdDraw(p_cmd, object.m_mesh->vertices.size(), 1,0 , i);
    }
}

void ViEngine::init_scene()
{
    const auto device = vi::GraphicsContext::get_device();
    RenderObject monkey;
    monkey.m_mesh = get_mesh("monkey");
    monkey.m_material = get_material("defaultmesh");
    monkey.m_transform_matrix = glm::mat4{ 1.0f };

    m_renderables.push_back(monkey);

    RenderObject map;
    map.m_mesh = get_mesh("empire");
    map.m_material = get_material("texturedmesh");
    map.m_transform_matrix = translate(glm::vec3{ 5,-10,0 }); //glm::mat4{ 1.0f };

    m_renderables.push_back(map);

    for (auto x = -20; x <= 20; x++) {
        for (auto y = -20; y <= 20; y++) {

            RenderObject tri;
            tri.m_mesh = get_mesh("triangle");
            tri.m_material = get_material("defaultmesh");
            glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
            glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
            tri.m_transform_matrix = translation * scale;

            m_renderables.push_back(tri);
        }
    }

    Material* texturedMat=	get_material("texturedmesh");

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptor_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_single_texture_set_layout;

    vkAllocateDescriptorSets(device, &allocInfo, &texturedMat->m_texture_set);

    const auto samplerInfo = vkinit::sampler_create_info(VK_FILTER_NEAREST);

    VkSampler blockySampler;
    vkCreateSampler(device, &samplerInfo, nullptr, &blockySampler);

    vi::DeletionQueue::push_function([=, this] {
        vkDestroySampler(device, blockySampler, nullptr);
    });

    VkDescriptorImageInfo imageBufferInfo;
    imageBufferInfo.sampler = blockySampler;
    imageBufferInfo.imageView = m_loaded_textures["empire_diffuse"].m_image_view;
    imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    const auto texture1 = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texturedMat->m_texture_set, &imageBufferInfo, 0);

    vkUpdateDescriptorSets(device, 1, &texture1, 0, nullptr);
}

size_t ViEngine::pad_uniform_buffer_size(const size_t p_original_size) const {
    // Calculate required alignment based on minimum device offset alignment
    const auto gpu_properties = vi::GraphicsContext::get_gpu_properties();
    const auto min_ubo_alignment = gpu_properties.limits.minUniformBufferOffsetAlignment;
    auto aligned_size = p_original_size;
    if (min_ubo_alignment > 0) {
        aligned_size = (aligned_size + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
    }
    return aligned_size;
}


void ViEngine::immediate_submit(std::function<void(VkCommandBuffer p_cmd)>&& p_function) const {
    const auto device = vi::GraphicsContext::get_device();

    const auto& upload_context = vi::GraphicsContext::get_upload_context();
    const auto cmd = upload_context.m_command_buffer;
    const auto& upload_fence = upload_context.m_upload_fence;
    const auto& command_pool = upload_context.m_command_pool;

    //Begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
    const auto cmd_begin_info = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    p_function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    const auto submit = vkinit::submit_info(&cmd);

    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    const auto graphics_queue = vi::GraphicsContext::get_graphics_queue();
    VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit, upload_fence));

    vkWaitForFences(device, 1, &upload_fence, true, 9999999999);
    vkResetFences(device, 1, &upload_fence);

    vkResetCommandPool(device, command_pool, 0);
}

void ViEngine::init_descriptors()
{
    const auto device = vi::GraphicsContext::get_device();
    //create a descriptor pool that will hold 10 uniform buffers
    std::vector<VkDescriptorPoolSize> sizes =
            {
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
            };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = 0;
    pool_info.maxSets = 10;
    pool_info.poolSizeCount = static_cast<uint32_t>(sizes.size());
    pool_info.pPoolSizes = sizes.data();

    vkCreateDescriptorPool(device, &pool_info, nullptr, &m_descriptor_pool);

    auto camera_bind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0);
    auto scene_bind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);

    VkDescriptorSetLayoutBinding bindings[] = { camera_bind,scene_bind };

    VkDescriptorSetLayoutCreateInfo set_info{};
    set_info.bindingCount = 2;
    set_info.flags = 0;
    set_info.pNext = nullptr;
    set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set_info.pBindings = bindings;

    vkCreateDescriptorSetLayout(device, &set_info, nullptr, &m_global_set_layout);

    auto object_bind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

    VkDescriptorSetLayoutCreateInfo set2_info{};
    set2_info.bindingCount = 1;
    set2_info.flags = 0;
    set2_info.pNext = nullptr;
    set2_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set2_info.pBindings = &object_bind;

    vkCreateDescriptorSetLayout(device, &set2_info, nullptr, &m_object_set_layout);

    auto texture_bind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

    VkDescriptorSetLayoutCreateInfo set3_info{};
    set3_info.bindingCount = 1;
    set3_info.flags = 0;
    set3_info.pNext = nullptr;
    set3_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set3_info.pBindings = &texture_bind;

    vkCreateDescriptorSetLayout(device, &set3_info, nullptr, &m_single_texture_set_layout);

    const auto scene_param_buffer_size = FRAME_OVERLAP * pad_uniform_buffer_size(sizeof(GPUSceneData));
    m_scene_parameter_buffer = std::make_unique<vi::Buffer>(scene_param_buffer_size, vi::Buffer::Usage::UNIFORM_BUFFER, vi::Buffer::MemoryUsage::CPU_TO_GPU);

    for (auto& [presentSemaphore, renderSemaphore, renderFence, commandPool, mainCommandBuffer, cameraBuffer, globalDescriptor, objectBuffer, objectDescriptor] : vi::GraphicsContext::get_frames()) {
        cameraBuffer = std::make_unique<vi::Buffer>(sizeof(GPUCameraData), vi::Buffer::Usage::UNIFORM_BUFFER, vi::Buffer::MemoryUsage::CPU_TO_GPU);

        constexpr auto max_objects = 10000;
        objectBuffer = std::make_unique<vi::Buffer>(sizeof(GPUObjectData) * max_objects, vi::Buffer::Usage::STORAGE_BUFFER, vi::Buffer::MemoryUsage::CPU_TO_GPU);

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.pNext = nullptr;
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = m_descriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &m_global_set_layout;

        vkAllocateDescriptorSets(device, &alloc_info, &globalDescriptor);

        VkDescriptorSetAllocateInfo object_set_alloc{};
        object_set_alloc.pNext = nullptr;
        object_set_alloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        object_set_alloc.descriptorPool = m_descriptor_pool;
        object_set_alloc.descriptorSetCount = 1;
        object_set_alloc.pSetLayouts = &m_object_set_layout;

        vkAllocateDescriptorSets(device, &object_set_alloc, &objectDescriptor);

        VkDescriptorBufferInfo camera_info{};
        camera_info.buffer = cameraBuffer->get_buffer().m_buffer;
        camera_info.offset = 0;
        camera_info.range = sizeof(GPUCameraData);

        VkDescriptorBufferInfo scene_info{};
        scene_info.buffer = m_scene_parameter_buffer->get_buffer().m_buffer;
        scene_info.offset = 0;
        scene_info.range = sizeof(GPUSceneData);

        VkDescriptorBufferInfo object_buffer_info{};
        object_buffer_info.buffer = objectBuffer->get_buffer().m_buffer;
        object_buffer_info.offset = 0;
        object_buffer_info.range = sizeof(GPUObjectData) * max_objects;

        auto camera_write = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, globalDescriptor,&camera_info,0);
        auto scene_write = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                                                                          globalDescriptor, &scene_info, 1);
        auto object_write = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, objectDescriptor, &object_buffer_info, 0);
        VkWriteDescriptorSet set_writes[] = { camera_write,scene_write,object_write };

        vkUpdateDescriptorSets(device, 3, set_writes, 0, nullptr);
    }

    vi::DeletionQueue::push_function([this, device] {
        const auto allocator = vi::GraphicsContext::get_allocator();

        vkDestroyDescriptorSetLayout(device, m_object_set_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, m_global_set_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, m_single_texture_set_layout, nullptr);

        vkDestroyDescriptorPool(device, m_descriptor_pool, nullptr);
    });
}
