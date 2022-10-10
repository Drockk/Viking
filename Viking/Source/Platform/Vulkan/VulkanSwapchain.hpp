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
		static VkSwapchainKHR getSwapchain();
		static VkFormat getSwapchainImageFormat();
		static VkExtent2D getSwapchainExtent();

		static std::vector<VkImageView> m_SwapchainImageViews;
	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const;

		void createImageViews();

		Ref<VulkanLogicalDevice> m_LogicalDevice;

		static inline VkSwapchainKHR m_Swapchain{ nullptr };
		std::vector<VkImage> m_SwapchainImages;
		static inline VkFormat m_SwapchainImageFormat{ VK_FORMAT_UNDEFINED };
		static inline VkExtent2D m_SwapchainExtent = { 0, 0 };
	};
}
