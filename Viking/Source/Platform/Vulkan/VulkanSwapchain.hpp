#pragma once

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Viking {
	class VulkanSwapchain {
	public:
		VulkanSwapchain() = default;
		~VulkanSwapchain();

		void init(const Ref<VulkanPhysicalDevice>& physicalDevice, VkSurfaceKHR surface, const Ref<VulkanLogicalDevice>& logicalDevice, GLFWwindow* window);

		//Temp
		[[nodiscard]] VkSwapchainKHR getSwapchain() const;
		[[nodiscard]] VkFormat getSwapchainImageFormat() const;
		[[nodiscard]] VkExtent2D getSwapchainExtent() const;
		[[nodiscard]] std::vector<VkImageView> getSwapchainImagesViews() const;

	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const;

		void createImageViews();

		Ref<VulkanLogicalDevice> m_LogicalDevice;

		VkSwapchainKHR m_Swapchain{ nullptr };
		std::vector<VkImage> m_SwapchainImages;
		VkFormat m_SwapchainImageFormat{ VK_FORMAT_UNDEFINED };
		VkExtent2D m_SwapchainExtent = { 0, 0 };
		std::vector<VkImageView> m_SwapchainImageViews;
	};
}
