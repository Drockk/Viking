#pragma once

#include <Viking.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>

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

const std::vector DEVICE_EXTENSION = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class ExampleLayer: public Viking::Layer {
public:
	ExampleLayer();
	~ExampleLayer() override = default;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(Viking::TimeStep timeStep) override;

private:
	static bool checkValidationLayerSupport();
	static std::vector<const char*> getRequiredExtensions();
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void initWindow();
	void initVulkan();

	//Init Vulkan Methods
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device) const;
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
	static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
	[[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;
	void createLogicalDevice();
	void createSwapChain();
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	[[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	void createImageViews();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;
	void createRenderPass();
	VkFormat findDepthFormat() const;
	[[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	bool m_FramebufferResized{ false };

	GLFWwindow* m_Window{ nullptr };

	VkDevice m_Device{ nullptr };
	VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
	VkInstance m_Instance{ nullptr };
	VkSampleCountFlagBits m_MsaaSamples{ VK_SAMPLE_COUNT_1_BIT };
	VkPhysicalDevice m_PhysicalDevice;
	VkSurfaceKHR m_Surface{ nullptr };

	VkQueue m_GraphicsQueue{ nullptr };
	VkQueue m_PresentQueue{ nullptr };

	VkSwapchainKHR m_SwapChain{ nullptr };
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat{ VK_FORMAT_UNDEFINED };
	VkExtent2D m_SwapChainExtent = { 0, 0 };
	std::vector<VkImageView> m_SwapChainImageViews;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	VkRenderPass m_RenderPass{ nullptr };
};
