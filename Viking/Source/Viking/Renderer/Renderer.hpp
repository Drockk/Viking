#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.hpp>

#include "Viking/Renderer/Mesh.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"
#include "Platform/Vulkan/Image.hpp"

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

        static void createRenderPass();
        [[nodiscard]] static VkFormat findDepthFormat();
        static void createDescriptorSetLayout();
        static void createGraphicsPipeline();
        static std::vector<char> readFile(const std::string& filename);
        [[nodiscard]] static VkShaderModule createShaderModule(const std::vector<char>& code);
        static void createColorResources();
        static void createDepthResources();
        static void createFramebuffers();
        static void createUniformBuffers();
        static void createDescriptorPool();
        static void createDescriptorSets();
        static void createCommandBuffers();
        static void createSyncObjects();
        static void recreateSwapChain();
        static void cleanupSwapChain();
        static void updateUniformBuffer(uint32_t currentImage);
        static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        inline static std::vector<VkFramebuffer> swapChainFramebuffers;
        inline static VkRenderPass m_RenderPass{};
        inline static VkDescriptorSetLayout m_DescriptorSetLayout{};
        inline static VkPipelineLayout m_PipelineLayout{};
        inline static VkPipeline m_GraphicsPipeline{};
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
