#pragma once

#include "Viking/Renderer/Pipeline.hpp"

#include "Platform/Vulkan/VulkanSwapchain.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Viking {
    class VulkanPipeline: public Pipeline {
    public:
        VulkanPipeline(const Ref<VulkanSwapchain>& swapchain, const Ref<VulkanLogicalDevice>& logicalDevice, const Ref<VulkanPhysicalDevice>& physicalDevice, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
        ~VulkanPipeline() override;

    private:
        void createDescriptorSetLayout();
        static std::vector<char> readFile(const std::string& filename);
        [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& code) const;

        Ref<VulkanLogicalDevice> m_LogicalDevice{ nullptr };

        VkDescriptorSetLayout m_DescriptorSetLayout{ nullptr };
        VkPipelineLayout m_PipelineLayout{ nullptr };
        VkPipeline m_GraphicsPipeline{ nullptr };
    };
}
