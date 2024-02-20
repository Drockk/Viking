//
// Created by Bartosz Zielonka on 18.02.2024.
//

#include "Platform/Windows/Window.hpp"

#include "Viking/core/Log.hpp"
#include "Viking/event/ApplicationEvent.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include <stdexcept>

namespace Windows
{
    Window::Window(vi::WindowProps p_props) : m_window_props{ std::move(p_props) }
    {
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

    void Window::set_vsync(const bool p_enabled)
    {
        vsync = p_enabled;
        glfwSwapInterval(p_enabled);
    }

    bool Window::is_vsync() const
    {
        return vsync;
    }

    float Window::get_time() const
    {
        return static_cast<float>(glfwGetTime());
    }

    void Window::create_window()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize GLFW");
        }

        glfwSetErrorCallback([](int p_error, const char* p_description)
        {
            VI_CORE_ERROR("[GLFW] {}:{}", p_error, p_description);
        });

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        const auto [width, height] = m_window_props.Size;
        m_window = glfwCreateWindow(width, height, m_window_props.Title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Cannot create GLFW window");
        }

        set_vsync(true);

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow*)
        {
            VI_CORE_TRACE("Received window should close");
            vi::EventDispatcher::send_event(std::make_shared<vi::WindowCloseEvent>());
        });
    }
}
