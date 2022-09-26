#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

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

		void init(VkInstance instance, VkSurfaceKHR surface);

	private:
		[[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
		static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		[[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;

		VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
		VkSampleCountFlagBits m_MsaaSamples{ VK_SAMPLE_COUNT_1_BIT };
	};
}
