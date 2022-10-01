#include "vipch.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"

#include <set>

namespace Viking {
    VkSampleCountFlagBits VulkanPhysicalDevice::m_MsaaSamples{ VK_SAMPLE_COUNT_1_BIT };

	void VulkanPhysicalDevice::init(VkInstance instance, VkSurfaceKHR surface) {
		uint32_t deviceCount{ 0 };
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		VI_CORE_ASSERT(deviceCount == 0, "Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device, surface)) {
                m_PhysicalDevice = device;
                m_MsaaSamples = getMaxUsableSampleCount();
                m_QueueFamilyIndices = findQueueFamilies(device, surface);
                break;
            }
        }


        VI_CORE_ASSERT(m_PhysicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU!");

        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
	}

    VkSampleCountFlagBits VulkanPhysicalDevice::getMsaaSamples() {
        return m_MsaaSamples;
    }

    QueueFamilyIndices VulkanPhysicalDevice::getQueueFamilyIndices() {
        return m_QueueFamilyIndices;
    }

    SwapChainSupportDetails VulkanPhysicalDevice::getSwapChainSupportDetails() {
        return m_SwapChainSupportDetails;
    }

    VkPhysicalDevice VulkanPhysicalDevice::getPhysicalDevice() {
        return m_PhysicalDevice;
    }

    VkPhysicalDeviceProperties VulkanPhysicalDevice::getPhysicalDeviceProperties() {
        return m_PhysicalDeviceProperties;
    }

    bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
        const auto indices = findQueueFamilies(device, surface);
        const auto extensionSupported = checkDeviceExtensionSupport(device);

        auto swapChainAdequate = false;
        if (extensionSupported) {
            m_SwapChainSupportDetails = querySwapChainSupport(device, surface);
            swapChainAdequate = !m_SwapChainSupportDetails.formats.empty() && !m_SwapChainSupportDetails.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

    QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        auto i{ 0 };
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    bool VulkanPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount{ 0 };
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtension(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtension.data());

        std::set<std::string> requiredExtension(DEVICE_EXTENSION.begin(), DEVICE_EXTENSION.end());

        for (const auto& extension : availableExtension) {
            requiredExtension.erase(extension.extensionName);
        }

        return requiredExtension.empty();
    }

    SwapChainSupportDetails VulkanPhysicalDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount{ 0 };
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount{ 0 };
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSampleCountFlagBits VulkanPhysicalDevice::getMaxUsableSampleCount() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

        const auto counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        if (counts & VK_SAMPLE_COUNT_64_BIT) {
            return VK_SAMPLE_COUNT_64_BIT;
        }

        if (counts & VK_SAMPLE_COUNT_32_BIT) {
            return VK_SAMPLE_COUNT_32_BIT;
        }

        if (counts & VK_SAMPLE_COUNT_16_BIT) {
            return VK_SAMPLE_COUNT_16_BIT;
        }

        if (counts & VK_SAMPLE_COUNT_8_BIT) {
            return VK_SAMPLE_COUNT_8_BIT;
        }

        if (counts & VK_SAMPLE_COUNT_4_BIT) {
            return VK_SAMPLE_COUNT_4_BIT;
        }

        if (counts & VK_SAMPLE_COUNT_2_BIT) {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
	}
}
