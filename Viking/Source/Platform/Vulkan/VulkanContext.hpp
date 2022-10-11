#pragma once

#include "Viking/Renderer/Context.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

const std::vector VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

namespace Viking {
	class VulkanContext: public Context {
	public:
		VulkanContext() = default;
		~VulkanContext() override;

		void init(const std::string& name, GLFWwindow* window) override;
		void onUpdate() const override;

		void* getInstance() override;
		void* getSurface() override;

		static Ref<VulkanSwapchain> getSwapchain();
	private:
		static bool checkValidationLayerSupport();
		static std::vector<const char*> getRequiredExtensions();
		static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void setupDebugMessenger();
		static VkResult createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void destroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
		                                          const VkAllocationCallbacks* pAllocator);
		void createSurface(GLFWwindow* window);

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanLogicalDevice> m_LogicalDevice;
		static inline Ref<VulkanSwapchain> m_Swapchain;

		VkInstance m_Instance{ nullptr };
		VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
		VkSurfaceKHR m_Surface{ nullptr };
	};
}
