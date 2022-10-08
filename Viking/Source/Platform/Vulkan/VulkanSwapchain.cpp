#include "vipch.hpp"

#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace Viking {

	VulkanSwapchain::~VulkanSwapchain() {
		if(m_Swapchain != nullptr) {
			vkDestroySwapchainKHR(m_LogicalDevice->getDevice(), m_Swapchain, nullptr);
		}
	}

	void VulkanSwapchain::init(const Ref<VulkanPhysicalDevice>& physicalDevice, VkSurfaceKHR surface, const Ref<VulkanLogicalDevice>& logicalDevice, GLFWwindow* window) {
		const auto [capabilities, formats, presentModes] = physicalDevice->getSwapChainSupportDetails();
		const auto surfaceFormat = chooseSwapSurfaceFormat(formats);
		const auto presentMode = chooseSwapPresentMode(presentModes);
		const auto extent = chooseSwapExtent(capabilities, window);

		auto imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const auto indices = physicalDevice->getQueueFamilyIndices();
		const uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		[[maybe_unused]] const auto result = vkCreateSwapchainKHR(logicalDevice->getDevice(), &createInfo, nullptr, &m_Swapchain);
		VI_CORE_ASSERT(result != VK_SUCCESS, "Failed to create swap chain!");

		vkGetSwapchainImagesKHR(logicalDevice->getDevice(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice->getDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());

		m_SwapchainImageFormat = surfaceFormat.format;
		m_SwapchainExtent = extent;

		m_LogicalDevice = logicalDevice;
	}

	VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	void VulkanSwapchain::createImageViews() {
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (uint32_t i{ 0 }; i < m_SwapchainImages.size(); i++) {
			m_SwapchainImageViews[i] = m_LogicalDevice->createImageView(m_SwapchainImages[i], m_SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}
}
