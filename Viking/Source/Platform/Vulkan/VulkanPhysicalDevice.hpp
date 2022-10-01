#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

const std::vector DEVICE_EXTENSION = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	[[nodiscard]] bool isComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

namespace Viking {
	class VulkanPhysicalDevice {
	public:
		VulkanPhysicalDevice() = default;
		~VulkanPhysicalDevice() = default;

		static void init(VkInstance instance, VkSurfaceKHR surface);

		//Temporary
		static VkSampleCountFlagBits getMsaaSamples();
		static QueueFamilyIndices getQueueFamilyIndices();
		static SwapChainSupportDetails getSwapChainSupportDetails();
		static VkPhysicalDevice getPhysicalDevice();
		static VkPhysicalDeviceProperties getPhysicalDeviceProperties();

	private:
		[[nodiscard]] static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
		static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		[[nodiscard]] static VkSampleCountFlagBits getMaxUsableSampleCount();

		inline static VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
		static VkSampleCountFlagBits m_MsaaSamples;
		inline static QueueFamilyIndices m_QueueFamilyIndices{};
		inline static SwapChainSupportDetails m_SwapChainSupportDetails{};
		inline static VkPhysicalDeviceProperties m_PhysicalDeviceProperties{};
	};
}
