#include "ExampleLayer.hpp"

constexpr uint32_t WIDTH{ 800 };
constexpr uint32_t HEIGHT{ 600 };

ExampleLayer::ExampleLayer(): Layer("ExampleLayer") {
}

void ExampleLayer::onAttach() {
	initWindow();
	initVulkan();
}

void ExampleLayer::onDetach() {
	cleanup();
}

void ExampleLayer::onUpdate(Viking::TimeStep timeStep) {
	mainLoop();
}

void ExampleLayer::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Sandbox", nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {
		m_FramebufferResized 
	});
}
