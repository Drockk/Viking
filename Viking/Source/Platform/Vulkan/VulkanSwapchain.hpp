#pragma once

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Viking {
	class VulkanSwapchain {
	public:
		VulkanSwapchain() = default;
		~VulkanSwapchain();

		void init(const Ref<VulkanPhysicalDevice>& physicalDevice, VkSurfaceKHR surface, const Ref<VulkanLogicalDevice>& logicalDevice, GLFWwindow* window);

		VkImageView createColorResource(VkImage image, VkDeviceMemory memory) const;
		VkImageView createDepthResource(VkImage image, VkDeviceMemory memory);

		//Temp
		[[nodiscard]] VkSwapchainKHR getSwapchain() const;
		[[nodiscard]] VkFormat getSwapchainImageFormat() const;
		[[nodiscard]] VkExtent2D getSwapchainExtent() const;
		[[nodiscard]] std::vector<VkImageView> getSwapchainImagesViews() const;
		[[nodiscard]] Ref<VulkanRenderPass>& getRenderPass();

	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) const;

		void createImageViews();

		VkImageView createResource(VkFormat format, VkImage image, VkDeviceMemory memory, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkImageAspectFlags aspectFlags) const;

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
			VkDeviceMemory& imageMemory) const;

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);



		Ref<VulkanRenderPass> m_RenderPass;

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanLogicalDevice> m_LogicalDevice;

		VkSwapchainKHR m_Swapchain{ nullptr };
		std::vector<VkImage> m_SwapchainImages;
		VkFormat m_SwapchainImageFormat{ VK_FORMAT_UNDEFINED };
		VkExtent2D m_SwapchainExtent = { 0, 0 };
		std::vector<VkImageView> m_SwapchainImageViews;
	};
}
