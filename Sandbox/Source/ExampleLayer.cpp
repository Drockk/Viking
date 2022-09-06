#include "ExampleLayer.hpp"

constexpr uint32_t WIDTH{ 800 };
constexpr uint32_t HEIGHT{ 600 };

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION_LAYERS{ false };
#else
constexpr bool ENABLE_VALIDATION_LAYERS{ true };
#endif

const std::vector VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

ExampleLayer::ExampleLayer(): Layer("ExampleLayer") {
}

void ExampleLayer::onAttach() {
	initWindow();
	initVulkan();
}

void ExampleLayer::onDetach() {
	//cleanup();
}

void ExampleLayer::onUpdate(Viking::TimeStep timeStep) {
    glfwPollEvents();

    //drawFrame();

    vkDeviceWaitIdle(m_Device);
}

bool ExampleLayer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VALIDATION_LAYERS) {
        auto layerFound{ false };

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> ExampleLayer::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (ENABLE_VALIDATION_LAYERS) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void ExampleLayer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkBool32 ExampleLayer::debugCallback([[maybe_unused]]  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData) {
    VI_CORE_DEBUG("validation layer: {0}", pCallbackData->pMessage);

    return VK_FALSE;
}

void ExampleLayer::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Sandbox", nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	//FIXME: Later
	//glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {
	//	m_FramebufferResized 
	//});

}

void ExampleLayer::initVulkan() {
    createInstance();
    //setupDebugMessenger();
    //createSurface();
    //pickPhysicalDevice();
    //createLogicalDevice();
    //createSwapChain();
    //createImageViews();
    //createRenderPass();
    //createDescriptorSetLayout();
    //createGraphicsPipeline();
    //createCommandPool();
    //createColorResources();
    //createDepthResources();
    //createFramebuffers();
    //createTextureImage();
    //createTextureImageView();
    //createTextureSampler();
    //loadModel();
    //createVertexBuffer();
    //createIndexBuffer();
    //createUniformBuffers();
    //createDescriptorPool();
    //createDescriptorSets();
    //createCommandBuffers();
    //createSyncObjects();
}

void ExampleLayer::createInstance() {
    if (ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Viking App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    const auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if constexpr (ENABLE_VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
