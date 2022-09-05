#pragma once

#include <Viking.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class ExampleLayer:public Viking::Layer {
public:
	ExampleLayer();
	~ExampleLayer() override = default;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(Viking::TimeStep timeStep) override;

private:
	void initWindow();

	bool m_FramebufferResized{ false };

	GLFWwindow* m_Window{ nullptr };
};
