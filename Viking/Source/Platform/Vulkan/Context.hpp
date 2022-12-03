#pragma once

#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Vulkan {
    class Context {
    public:
        Context();
        ~Context();

        static Context* get();

        void createSurface(GLFWwindow* window);
        void createPhysicalDevice();
        void createDevice();

        [[nodiscard]] VkInstance getInstance() const;
        [[nodiscard]] VkSurfaceKHR getSurface() const;
        Viking::Ref<PhysicalDevice> getPhysicalDevice();
        Viking::Ref<Device>& getDevice();
    private:
        static bool checkValidationLayerSupport();
        static std::vector<const char*> getRequiredExtensions();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VKAPI_ATTR static VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        void setupDebugMessenger();

        //Devices
        Viking::Ref<PhysicalDevice> m_PhysicalDevice;
        Viking::Ref<Device> m_Device;

        //Instances
        VkInstance m_Instance{};
        VkDebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
        VkSurfaceKHR m_Surface{};

        inline static Context* s_Context{ nullptr };
    };
}
