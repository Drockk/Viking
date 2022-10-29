#pragma once

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Viking {
	class VulkanRenderPass {
	public:
		VulkanRenderPass() = default;
		~VulkanRenderPass();

		void init(VkFormat swapchainImageFormat, const Ref<VulkanPhysicalDevice>& physicalDevice, const Ref<VulkanLogicalDevice>& logicalDevice);
		[[nodiscard]] VkRenderPass getRenderPass() const;
	private:
		[[nodiscard]] VkFormat findDepthFormat() const;
		[[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

		VkRenderPass m_RenderPass{ nullptr };
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanLogicalDevice> m_LogicalDevice;
	};
}
