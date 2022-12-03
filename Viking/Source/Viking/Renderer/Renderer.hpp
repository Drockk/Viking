#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.hpp>

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
        return (hash<glm::vec3>()(vertex.pos) ^ hash<glm::vec3>()(vertex.color) << 1) >> 1 ^ hash<glm::vec2>()(vertex.texCoord) << 1;
    }
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


namespace Viking {
    class Renderer {
    public:
        static void init();
        static void cleanup();

        static void drawFrame();

    private:
        static void createSwapChain();
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        [[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        static void createImageViews();
        static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        static void createRenderPass();
        [[nodiscard]] static VkFormat findDepthFormat();
        static void createDescriptorSetLayout();
        static void createGraphicsPipeline();
        static std::vector<char> readFile(const std::string& filename);
        [[nodiscard]] static VkShaderModule createShaderModule(const std::vector<char>& code);
        static void createCommandPool();
        static void createColorResources();
        static void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        static void createDepthResources();
        static void createFramebuffers();
        static void createTextureImage();
        static void createTextureImageView();
        static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        [[nodiscard]] static VkCommandBuffer beginSingleTimeCommands();
        static void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        static void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        static void createTextureSampler();
        static void loadModel();
        static void createVertexBuffer();
        static void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        static void createIndexBuffer();
        static void createUniformBuffers();
        static void createDescriptorPool();
        static void createDescriptorSets();
        static void createCommandBuffers();
        static void createSyncObjects();
        static void recreateSwapChain();
        static void cleanupSwapChain();
        static void updateUniformBuffer(uint32_t currentImage);
        static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        inline static VkSwapchainKHR m_SwapChain{};
        inline static std::vector<VkImage> m_SwapChainImages;
        inline static VkFormat m_SwapChainImageFormat{};
        inline static VkExtent2D m_SwapChainExtent{};
        inline static std::vector<VkImageView> swapChainImageViews;
        inline static std::vector<VkFramebuffer> swapChainFramebuffers;
        inline static VkRenderPass m_RenderPass{};
        inline static VkDescriptorSetLayout m_DescriptorSetLayout{};
        inline static VkPipelineLayout m_PipelineLayout{};
        inline static VkPipeline m_GraphicsPipeline{};
        inline static VkCommandPool m_CommandPool{};
        inline static VkImage m_ColorImage{};
        inline static VkDeviceMemory m_ColorImageMemory{};
        inline static VkImageView m_ColorImageView{};
        inline static VkImage m_DepthImage{};
        inline static VkDeviceMemory m_DepthImageMemory{};
        inline static VkImageView m_DepthImageView{};
        inline static uint32_t m_MipLevels{};
        inline static VkImage m_TextureImage{};
        inline static VkDeviceMemory m_TextureImageMemory{};
        inline static VkImageView m_TextureImageView{};
        inline static VkSampler m_TextureSampler{};
        inline static std::vector<Vertex> m_Vertices;
        inline static std::vector<uint32_t> m_Indices;
        inline static VkBuffer m_VertexBuffer{};
        inline static VkDeviceMemory m_VertexBufferMemory{};
        inline static VkBuffer m_IndexBuffer{};
        inline static VkDeviceMemory m_IndexBufferMemory{};
        inline static std::vector<VkBuffer> m_UniformBuffers;
        inline static std::vector<VkDeviceMemory> m_UniformBuffersMemory;
        inline static std::vector<void*> m_UniformBuffersMapped;
        inline static VkDescriptorPool m_DescriptorPool{};
        inline static std::vector<VkDescriptorSet> m_DescriptorSets;
        inline static std::vector<VkCommandBuffer> m_CommandBuffers;
        inline static std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        inline static std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        inline static std::vector<VkFence> m_InFlightFences;
        inline static bool m_FramebufferResized{ false };
        inline static uint32_t m_CurrentFrame{ 0 };
    };
}
