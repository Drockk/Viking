#pragma once

#include "Viking/Renderer/Context.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


namespace Viking {
	class VulkanContext: public Context {
	public:
		VulkanContext() = default;
		~VulkanContext() override;

		void init(const std::string& name, GLFWwindow*) override;
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

		VkInstance m_Instance{ nullptr };
		VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
		VkSurfaceKHR m_Surface{ nullptr };
	};
}
