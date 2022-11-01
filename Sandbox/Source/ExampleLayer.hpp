#pragma once

#include <Viking.hpp>

#include <vulkan/vulkan.hpp>

class ExampleLayer: public Viking::Layer {
public:
    ExampleLayer();
    ~ExampleLayer() override = default;

    void onAttach() override;
    void onDetach() override;

    void onUpdate(Viking::TimeStep timeStep) override;

private:
    void initVulkan();

    //Init Vulkan Methods
    static VkFormat findDepthFormat();
    [[nodiscard]] static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void createCommandPool();
    void createColorResources();
    void createDepthResources();
    void createFramebuffers();
    void createTextureImage();

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
    void createVertexBuffer();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    void cleanup() const;
    void cleanupSwapChain() const;

    void drawFrame();
    void recreateSwapChain();

    void updateUniformBuffer(uint32_t currentImage) const;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

    bool m_FramebufferResized{ false };

    Viking::Scope<Viking::Context> m_Instance;
    Viking::Scope<Viking::Mesh> m_Mesh;

    std::vector<VkFramebuffer> m_SwapChainFramebuffers;

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

    uint32_t m_CurrentFrame{ 0 };
};
