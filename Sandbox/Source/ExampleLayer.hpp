#pragma once

#include <Viking.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

template<> struct std::hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

const std::vector DEVICE_EXTENSION = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class ExampleLayer: public Viking::Layer {
public:
    ExampleLayer();
    ~ExampleLayer() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(Viking::TimeStep timeStep) override;

private:
    static bool checkValidationLayerSupport();
    static std::vector<const char*> getRequiredExtensions();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void initWindow();
    void initVulkan();

    //Init Vulkan Methods
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
    [[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;
    void createLogicalDevice();
    void createSwapChain();
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
    void createImageViews();
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;
    void createRenderPass();
    VkFormat findDepthFormat() const;
    [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createCommandPool();
    void createColorResources();
    void createDepthResources();
    void createFramebuffers();
    void createTextureImage();

    static std::vector<char> readFile(const std::string& filename);
    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& code) const;
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;
    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const;
    [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;
    void createTextureImageView();
    void createTextureSampler();
    void loadModel();
    void createVertexBuffer();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    void cleanup();
    void cleanupSwapChain();
    void destroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    bool m_FramebufferResized{ false };

    GLFWwindow* m_Window{ nullptr };

    VkDevice m_Device{ nullptr };
    VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
    VkInstance m_Instance{ nullptr };
    VkSampleCountFlagBits m_MsaaSamples{ VK_SAMPLE_COUNT_1_BIT };
    VkPhysicalDevice m_PhysicalDevice;
    VkSurfaceKHR m_Surface{ nullptr };

    VkQueue m_GraphicsQueue{ nullptr };
    VkQueue m_PresentQueue{ nullptr };

    VkSwapchainKHR m_SwapChain{ nullptr };
    std::vector<VkImage> m_SwapChainImages;
    VkFormat m_SwapChainImageFormat{ VK_FORMAT_UNDEFINED };
    VkExtent2D m_SwapChainExtent = { 0, 0 };
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;

    VkRenderPass m_RenderPass{ nullptr };
    VkDescriptorSetLayout m_DescriptorSetLayout{ nullptr };

    VkPipelineLayout m_PipelineLayout{ nullptr };
    VkPipeline m_GraphicsPipeline{ nullptr };

    VkCommandPool m_CommandPool{ nullptr };

    VkImage m_ColorImage{ nullptr };
    VkDeviceMemory m_ColorImageMemory{ nullptr };
    VkImageView m_ColorImageView{ nullptr };

    VkImage m_DepthImage{ nullptr };
    VkDeviceMemory m_DepthImageMemory{ nullptr };
    VkImageView m_DepthImageView{ nullptr };

    uint32_t m_MipLevels{ 0 };

    VkImage m_TextureImage{ nullptr };
    VkDeviceMemory m_TextureImageMemory{ nullptr };
    VkImageView m_TextureImageView{ nullptr };
    VkSampler m_TextureSampler{ nullptr };

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;

    VkBuffer m_VertexBuffer{ nullptr };
    VkDeviceMemory m_VertexBufferMemory{ nullptr };

    VkBuffer m_IndexBuffer{ nullptr };
    VkDeviceMemory m_IndexBufferMemory{ nullptr };

    std::vector<VkBuffer> m_UniformBuffers;
    std::vector<VkDeviceMemory> m_UniformBuffersMemory;

    VkDescriptorPool m_DescriptorPool{ nullptr };
    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
};
