#pragma once

#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/VulkanSwapchain.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"

namespace Viking {
	class VulkanRenderPass {
	public:
		VulkanRenderPass() = default;
		~VulkanRenderPass();

		void init(const Ref<VulkanSwapchain>& swapchain, const Ref<VulkanPhysicalDevice>& physicalDevice, const Ref<VulkanLogicalDevice>
		          & logicalDevice);
	private:
		VkFormat findDepthFormat() const;
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
			VkFormatFeatureFlags features) const;

		VkRenderPass m_RenderPass{ nullptr };
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanLogicalDevice> m_LogicalDevice;
	};
}
