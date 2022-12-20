#pragma once

#include "Platform/Vulkan/Context.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
	class Swapchain {
	public:
		Swapchain();
		~Swapchain();

	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		[[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;

		void createImageViews();

		Viking::Ref<Device> m_Device;

		VkSwapchainKHR m_Swapchain{};
		std::vector<VkImage> m_SwapchainImages;
		VkFormat m_SwapchainImageFormat{};
		VkExtent2D m_SwapchainExtent{};
		std::vector<VkImageView> m_SwapchainImageViews;
	};
}
