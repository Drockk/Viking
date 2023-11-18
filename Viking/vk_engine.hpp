#pragma once

#include "vk_types.hpp"
#include "vk_mesh.hpp"
#include "Core/Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
    VkPipelineVertexInputStateCreateInfo m_vertex_input_info{};
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly{};
    VkViewport m_viewport{};
    VkRect2D m_scissor{};
    VkPipelineRasterizationStateCreateInfo m_rasterizer{};
    VkPipelineColorBlendAttachmentState m_color_blend_attachment{};
    VkPipelineMultisampleStateCreateInfo m_multisampling{};
    VkPipelineLayout m_pipeline_layout{};
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil{};
    VkPipeline build_pipeline(VkDevice p_device, VkRenderPass p_pass) const;
};

struct MeshPushConstants {
    glm::vec4 m_data;
    glm::mat4 m_render_matrix;
};

struct Material {
    VkDescriptorSet m_texture_set{VK_NULL_HANDLE}; //texture defaulted to null
    VkPipeline m_pipeline{};
    VkPipelineLayout m_pipeline_layout{};
};

struct RenderObject {
    Mesh* m_mesh;

    Material* m_material;

    glm::mat4 m_transform_matrix;
};

struct FrameData {
    VkSemaphore m_present_semaphore;
    VkSemaphore m_render_semaphore;
    VkFence m_render_fence;

    VkCommandPool m_command_pool;
    VkCommandBuffer m_main_command_buffer;

    AllocatedBuffer m_camera_buffer;
    VkDescriptorSet m_global_descriptor;

    AllocatedBuffer m_object_buffer;
    VkDescriptorSet m_object_descriptor;
};

struct GPUCameraData{
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::mat4 m_view_proj;
};

struct GPUSceneData {
    glm::vec4 m_fog_color; // w is for exponent
    glm::vec4 m_fog_distances; //x for min, y for max, zw unused.
    glm::vec4 m_ambient_color;
    glm::vec4 m_sunlight_direction; //w for sun power
    glm::vec4 m_sunlight_color;
};

struct GPUObjectData {
    glm::mat4 m_model_matrix;
};

struct UploadContext {
    VkFence m_upload_fence{};
    VkCommandPool m_command_pool{};
    VkCommandBuffer m_command_buffer{};
};

struct Texture {
    AllocatedImage m_image;
    VkImageView m_image_view;
};

constexpr uint32_t FRAME_OVERLAP{2};

class ViEngine
{
public:
    bool m_is_initialized{ false };
    int m_frame_number{};

    VkExtent2D m_window_extent{ 1600 , 900 };

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_chosen_gpu;
    VkDevice m_device;

    VkPhysicalDeviceProperties m_gpu_properties;

    FrameData m_frames[FRAME_OVERLAP];

    VkQueue m_graphics_queue;
    uint32_t m_graphics_queue_family;

    VkRenderPass m_render_pass;

    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapchain;
    VkFormat m_swachain_image_format;

    std::vector<VkFramebuffer> m_framebuffers;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;

    VmaAllocator m_allocator; //vma lib allocator

    //depth resources
    VkImageView m_depth_image_view;
    AllocatedImage m_depth_image;

    //the format for the depth image
    VkFormat m_depth_format;

    VkDescriptorPool m_descriptor_pool;

    VkDescriptorSetLayout m_global_set_layout;
    VkDescriptorSetLayout m_object_set_layout;

    GPUSceneData m_scene_parameters;
    AllocatedBuffer m_scene_parameter_buffer;

    UploadContext m_upload_context{};

    //texture hash-map
    std::unordered_map<std::string, Texture> m_loaded_textures;

    VkDescriptorSetLayout m_single_texture_set_layout;

    //initializes everything in the engine
    void init();

    //shuts down the engine
    void cleanup() const;

    //draw loop
    void draw();

    //run the main loop
    void run();

    FrameData& get_current_frame();
    FrameData& get_last_frame();

    //default array of render-able objects
    std::vector<RenderObject> m_renderables;

    std::unordered_map<std::string, Material> m_materials;
    std::unordered_map<std::string, Mesh> m_meshes;
    //functions

    //create material and add it to the map
    Material* create_material(VkPipeline p_pipeline, VkPipelineLayout p_layout, const std::string& p_name);

    //returns nullptr if it cant be found
    Material* get_material(const std::string& p_name);

    //returns nullptr if it cant be found
    Mesh* get_mesh(const std::string& p_name);

    //our draw function
    void draw_objects(VkCommandBuffer p_cmd, const RenderObject* p_first, int p_count);

    [[nodiscard]] AllocatedBuffer create_buffer(size_t p_alloc_size, VkBufferUsageFlags p_usage, VmaMemoryUsage p_memory_usage) const;

    [[nodiscard]] size_t pad_uniform_buffer_size(size_t p_original_size) const;

    void immediate_submit(std::function<void(VkCommandBuffer p_cmd)>&& p_function) const;

    void load_images();

private:

    void init_commands();
    void init_default_renderpass();
    void init_descriptors();
    void init_framebuffers();
    void init_pipelines();
    void init_scene();
    void init_swapchain();
    void init_sync_structures();
    void init_vulkan();

    //loads a shader module from a spir-v file. Returns false if it errors
    void load_meshes();

    void upload_mesh(Mesh& p_mesh) const;

    std::unique_ptr<vi::Window> m_window;
};
