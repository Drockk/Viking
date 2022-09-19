#include "vipch.hpp"
#include "Platform/Windows/WindowsWindow.hpp"


Viking::WindowsWindow::WindowsWindow(const WindowProps& props) {
	m_Props = props;
	if (!glfwInit()) {
		throw std::runtime_error("Cannot initialize GLFW");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(static_cast<int>(m_Props.Width), static_cast<int>(m_Props.Height), m_Props.Title.c_str(), nullptr, nullptr);
	if (!m_Window) {
		glfwTerminate();
		throw std::runtime_error("Cannot create GLFW Window");
	}

	glfwSetWindowUserPointer(m_Window, this);
}

Viking::WindowsWindow::~WindowsWindow() {
	glfwTerminate();
}

void Viking::WindowsWindow::onUpdate() {
}

uint32_t Viking::WindowsWindow::getWidth() const {
	return m_Props.Width;
}

uint32_t Viking::WindowsWindow::getHeight() const {
	return m_Props.Height;
}

void Viking::WindowsWindow::setEventCallback(const EventCallbackFn& callback) {
}

void Viking::WindowsWindow::setVSync(bool enabled) {
}

bool Viking::WindowsWindow::isVSync() const {
	return false;
}

void* Viking::WindowsWindow::getNativeWindow() const {
	return nullptr;
}
