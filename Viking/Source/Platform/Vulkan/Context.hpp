#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Vulkan {
    const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    class Context {
    public:
        Context();
        ~Context();

        void createSurface(GLFWwindow* window);

        static VkInstance getInstance();
        static VkSurfaceKHR getSurface();
    private:
        static bool checkValidationLayerSupport();
        static std::vector<const char*> getRequiredExtensions();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VKAPI_ATTR static VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        void setupDebugMessenger();

        inline static VkInstance s_Instance{};
        VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
        inline static VkSurfaceKHR s_Surface{};
    };
}
