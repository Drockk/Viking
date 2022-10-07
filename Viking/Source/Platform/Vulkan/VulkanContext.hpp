#pragma once

#include "Viking/Renderer/Context.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanLogicalDevice.hpp"

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

		void init(const std::string& name, GLFWwindow*) override;
		void onUpdate() const;

		void* getInstance() override;
		void* getSurface() override;
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

		VulkanPhysicalDevice m_PhysicalContext;
		Ref<VulkanLogicalDevice> m_LogicalDevice;

		VkInstance m_Instance{ nullptr };
		VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
		VkSurfaceKHR m_Surface{ nullptr };
	};
}
