//
// Created by Bartosz Zielonka on 18.02.2024.
//

#include "Platform/Windows/Window.hpp"

#include "Viking/core/Log.hpp"
#include "Viking/event/ApplicationEvent.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include <stdexcept>

namespace windows
{
    Window::Window(vi::WindowProps p_props) : m_window_props{ std::move(p_props) }
    {
        init();
        create_window();
    }

    Window::~Window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void Window::on_update()
    {
        glfwPollEvents();
    }

    void Window::on_swap()
    {
        if (!m_window)
        {
            throw std::runtime_error("Window pointer is invalid");
        }

        glfwSwapBuffers(m_window);
    }

    [[nodiscard]] std::pair<int32_t, int32_t> Window::get_size() const
    {
        return m_window_props.Size;
    }

    float Window::get_time() const
    {
        return static_cast<float>(glfwGetTime());
    }

    VkSurfaceKHR Window::create_surface(const VkInstance p_instance) const
    {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(p_instance, m_window, nullptr, &surface)) {
            throw std::runtime_error("Cannot create GLFW window surface");
        }

        return surface;
    }

    void Window::init()
    {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize GLFW");
        }

        glfwSetErrorCallback([](int p_error, const char* p_description)
        {
                VI_CORE_ERROR("[GLFW] {}:{}", p_error, p_description);
        });

        VI_CORE_TRACE(glfwGetVersionString());
    }

    void Window::create_window()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        const auto& [width, height] = m_window_props.Size;
        m_window = glfwCreateWindow(width, height, m_window_props.Title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow*)
        {
            VI_CORE_TRACE("Received window should close");
            vi::EventDispatcher::send_event(std::make_shared<vi::WindowCloseEvent>());
        });
    }
}
