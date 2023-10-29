#pragma once

#include "vk_types.hpp"
#include "vk_mesh.hpp"
#include "Core/Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
    VkViewport _viewport;
    VkRect2D _scissor;
    VkPipelineRasterizationStateCreateInfo _rasterizer;
    VkPipelineColorBlendAttachmentState _colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo _multisampling;
    VkPipelineLayout _pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo _depthStencil;
    VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
};

struct MeshPushConstants {
    glm::vec4 data;
    glm::mat4 render_matrix;
};


struct Material {
    VkDescriptorSet textureSet{VK_NULL_HANDLE}; //texture defaulted to null
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};

struct RenderObject {
    Mesh* mesh;

    Material* material;

    glm::mat4 transformMatrix;
};

struct FrameData {
    VkSemaphore _presentSemaphore;
    VkSemaphore _renderSemaphore;
    VkFence _renderFence;

    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    AllocatedBuffer cameraBuffer;
    VkDescriptorSet globalDescriptor;

    AllocatedBuffer objectBuffer;
    VkDescriptorSet objectDescriptor;
};

struct GPUCameraData{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
};


struct GPUSceneData {
    glm::vec4 fogColor; // w is for exponent
    glm::vec4 fogDistances; //x for min, y for max, zw unused.
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection; //w for sun power
    glm::vec4 sunlightColor;
};

struct GPUObjectData {
    glm::mat4 modelMatrix;
};

struct UploadContext {
    VkFence _uploadFence{};
    VkCommandPool _commandPool{};
    VkCommandBuffer _commandBuffer{};
};

struct Texture {
    AllocatedImage image;
    VkImageView imageView;
};

constexpr uint32_t FRAME_OVERLAP{2};

class ViEngine
{
public:
    bool _isInitialized{ false };
    int _frameNumber {0};

    VkExtent2D _windowExtent{ 1700 , 900 };

    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkPhysicalDevice _chosenGPU;
    VkDevice _device;

    VkPhysicalDeviceProperties _gpuProperties;

    FrameData _frames[FRAME_OVERLAP];

    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    VkRenderPass _renderPass;

    VkSurfaceKHR _surface;
    VkSwapchainKHR _swapchain;
    VkFormat _swachainImageFormat;

    std::vector<VkFramebuffer> _framebuffers;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    VmaAllocator _allocator; //vma lib allocator

    //depth resources
    VkImageView _depthImageView;
    AllocatedImage _depthImage;

    //the format for the depth image
    VkFormat _depthFormat;

    VkDescriptorPool _descriptorPool;

    VkDescriptorSetLayout _globalSetLayout;
    VkDescriptorSetLayout _objectSetLayout;

    GPUSceneData _sceneParameters;
    AllocatedBuffer _sceneParameterBuffer;

    UploadContext _uploadContext{};

    //texture hashmap
    std::unordered_map<std::string, Texture> _loadedTextures;

    VkDescriptorSetLayout _singleTextureSetLayout;

    //initializes everything in the engine
    void init();

    //shuts down the engine
    void cleanup();

    //draw loop
    void draw();

    //run the main loop
    void run();

    FrameData& get_current_frame();
    FrameData& get_last_frame();

    //default array of renderable objects
    std::vector<RenderObject> _renderables;

    std::unordered_map<std::string, Material> _materials;
    std::unordered_map<std::string, Mesh> _meshes;
    //functions

    //create material and add it to the map
    Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);

    //returns nullptr if it cant be found
    Material* get_material(const std::string& name);

    //returns nullptr if it cant be found
    Mesh* get_mesh(const std::string& name);

    //our draw function
    void draw_objects(VkCommandBuffer cmd, RenderObject* first, int count);

    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

    size_t pad_uniform_buffer_size(size_t originalSize);

    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

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

    void upload_mesh(Mesh& mesh);

    std::unique_ptr<vi::Window> m_window;
};