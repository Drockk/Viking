#pragma once

#include "VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Viking {
	class VulkanLogicalDevice {
	public:
		VulkanLogicalDevice() = default;
		~VulkanLogicalDevice();

		void init(Ref<VulkanPhysicalDevice> device);

		static void onUpdate();

		static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		//Temporary
		static VkDevice getDevice();
		static VkQueue getGraphicsQueue();
		static VkQueue getPresentQueue();

	private:
		inline static VkDevice m_Device{ nullptr };

		inline static VkQueue m_GraphicsQueue{ nullptr };
		inline static VkQueue m_PresentQueue{ nullptr };
	};
}
