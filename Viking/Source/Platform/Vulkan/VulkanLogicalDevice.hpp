#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanPhysicalDevice.hpp"

namespace Viking {
	class VulkanLogicalDevice {
	public:
		VulkanLogicalDevice() = default;
		~VulkanLogicalDevice();

		void init(VulkanPhysicalDevice device);

		//Temporary
		static VkDevice getDevice();
		static VkQueue getGraphicsQueue();
		static VkQueue getPresentQueue();

		void onUpdate() const;
	private:
		inline static VkDevice m_Device{ nullptr };

		inline static VkQueue m_GraphicsQueue{ nullptr };
		inline static VkQueue m_PresentQueue{ nullptr };
	};
}
