#pragma once

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

namespace Vulkan {
	class PhysicalDevice {
	public:
		PhysicalDevice();
		~PhysicalDevice() = default;

		[[nodiscard]] VkPhysicalDevice get() const;
		[[nodiscard]] VkSampleCountFlagBits getMsaaSamples() const;
		[[nodiscard]] QueueFamilyIndices getQueueFamilyIndices() const;
		[[nodiscard]] SwapChainSupportDetails getSwapChainSupportDetails() const;
		[[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		[[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	private:
		static bool isDeviceSuitable(VkPhysicalDevice device);
		static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		[[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;

		VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
		VkSampleCountFlagBits m_MsaaSamples{ VK_SAMPLE_COUNT_1_BIT };
		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SwapChainSupportDetails;
	};
}
