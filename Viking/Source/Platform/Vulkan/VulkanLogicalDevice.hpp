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

		void onUpdate() const;
	private:
		inline static VkDevice m_Device{ nullptr };

		VkQueue m_GraphicsQueue{ nullptr };
		VkQueue m_PresentQueue{ nullptr };
	};
}
