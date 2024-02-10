#include "Window.hpp"

#include <stdexcept>

namespace vi {
    Window::Window(const std::string_view p_title, std::pair<int32_t, int32_t> p_resolution) {
        if (not glfwInit()) {
            throw std::runtime_error("Cannot initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        auto [width, height] = p_resolution;
        m_window = glfwCreateWindow(width, height, std::string(p_title).c_str(), nullptr, nullptr);
        if (not m_window) {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }

        glfwMakeContextCurrent(m_window);
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    VkSurfaceKHR Window::create_surface(const VkInstance p_instance) const {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(p_instance, m_window, nullptr, &surface)) {
            throw std::runtime_error("Cannot create GLFW window surface");
        }

        return surface;
    }

    void Window::on_update() {
        glfwPollEvents();
    }

    bool Window::should_close() const {
        return glfwWindowShouldClose(m_window);
    }
} // vi
