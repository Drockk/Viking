//
// Created by batzi on 28.10.2023.
//

#include "Window.hpp"

#include <stdexcept>

namespace vi {
    Window::Window(std::string_view t_title, std::pair<int32_t, int32_t> t_resolution) {
        if (not glfwInit()) {
            throw std::runtime_error("Cannot initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        auto [width, height] = t_resolution;
        m_window = glfwCreateWindow(width, height, std::string(t_title).c_str(), nullptr, nullptr);
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

    VkSurfaceKHR Window::create_surface(VkInstance t_instance) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(t_instance, m_window, nullptr, &surface)) {
            throw std::runtime_error("Cannot create GLFW window surface");
        }

        return surface;
    }

    void Window::on_update() {
        glfwPollEvents();
    }

    bool Window::should_close() {
        return glfwWindowShouldClose(m_window);
    }
} // vi