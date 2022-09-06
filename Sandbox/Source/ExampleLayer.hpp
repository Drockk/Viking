#pragma once

#include <Viking.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	bool m_FramebufferResized{ false };

	GLFWwindow* m_Window{ nullptr };

	VkDevice m_Device{ nullptr };
	VkInstance m_Instance{ nullptr };
};
