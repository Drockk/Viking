#include "vipch.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"

#include <set>

namespace Viking {
	VulkanLogicalDevice::~VulkanLogicalDevice() {
		if (m_Device != nullptr) {
			vkDestroyDevice(m_Device, nullptr);
		}
	}

	void VulkanLogicalDevice::init(Ref<VulkanPhysicalDevice> device) {
		auto [graphicsFamily, presentFamily] = device->getQueueFamilyIndices();

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set uniqueQueueFamilies = { graphicsFamily.value(), presentFamily.value() };

		auto queuePriority{ 1.0f };
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSION.size());
		createInfo.ppEnabledExtensionNames = DEVICE_EXTENSION.data();

		if constexpr (ENABLE_VALIDATION_LAYERS) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(device->getPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(m_Device, graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, presentFamily.value(), 0, &m_PresentQueue);
	}

	VkDevice VulkanLogicalDevice::getDevice() {
		return m_Device;
	}

	VkQueue VulkanLogicalDevice::getGraphicsQueue() {
		return m_GraphicsQueue;
	}

	VkQueue VulkanLogicalDevice::getPresentQueue() {
		return m_PresentQueue;
	}

	void VulkanLogicalDevice::onUpdate() {
		vkDeviceWaitIdle(m_Device);
	}

	VkImageView VulkanLogicalDevice::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
		uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		[[maybe_unused]] const auto result = vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView);
		VI_CORE_ASSERT(result != VK_SUCCESS, "Failed to create texture image view!");

		return imageView;
	}
}
