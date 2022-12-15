#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.hpp>

#include "Viking/Renderer/Mesh.hpp"

#include "Platform/Vulkan/Buffer.hpp"
#include  "Platform/Vulkan/Image.hpp"

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
        inline static Ref<Mesh> m_Mesh{};
        inline static std::vector<Ref<Vulkan::Buffer>> m_UniformBuffers;
        inline static Ref<Vulkan::Image> m_ColorImage;
        inline static Ref<Vulkan::Image> m_DepthImage;
        inline static Ref<Vulkan::Image> m_TextureImage;

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
        static void createColorResources();
        static void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        static void createDepthResources();
        static void createFramebuffers();
        static void createTextureImage();
        static void createTextureImageView();
        static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        static void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        static void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        static void createTextureSampler();
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
        inline static uint32_t m_MipLevels{};
        //inline static VkImage m_TextureImage{};
        //inline static VkDeviceMemory m_TextureImageMemory{};
        //inline static VkImageView m_TextureImageView{};
        inline static VkSampler m_TextureSampler{};
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
