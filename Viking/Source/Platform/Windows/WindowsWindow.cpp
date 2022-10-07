#include "vipch.hpp"
#include "Platform/Windows/WindowsWindow.hpp"

#include "Viking/Events/ApplicationEvent.hpp"
#include "Viking/Events/KeyEvent.hpp"
#include "Viking/Events/MouseEvent.hpp"

namespace Viking {
    static uint8_t s_GLFWWindowCount{ 0 };

    static void GLFWErrorCallback(int error, const char* description) {
        VI_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    WindowsWindow::WindowsWindow(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        VI_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

        if (s_GLFWWindowCount == 0) {
            const auto result = glfwInit();
            VI_CORE_ASSERT(result, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            m_Window = glfwCreateWindow(static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height), m_Data.Title.c_str(), nullptr, nullptr);
            if (!m_Window) {
                glfwTerminate();
                VI_CORE_ASSERT(false, "Could not create GLFW window!");
            }
            ++s_GLFWWindowCount;
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);
        WindowsWindow::setVSync(true);

        m_Context = Context::create();
        m_Context->init(m_Data.Title, m_Window);

        //Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const int width, const int height) {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, const int key, [[maybe_unused]] int scancode, const int action, int mods) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                data.EventCallback(event);
                break;
            }
            default: 
                break;
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    WindowsWindow::~WindowsWindow() {
        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount) {
            glfwTerminate();
        }
    }

    void WindowsWindow::onUpdate() {
        m_Context->onUpdate();

        glfwPollEvents();
    }

    uint32_t WindowsWindow::getWidth() const {
        return m_Data.Width;
    }

    uint32_t WindowsWindow::getHeight() const {
        return m_Data.Height;
    }

    void WindowsWindow::setEventCallback(const EventCallbackFn& callback) {
        m_Data.EventCallback = callback;
    }

    void WindowsWindow::setVSync(const bool enabled) {
        if (enabled) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }

        m_Data.VSync = enabled;
    }

    bool WindowsWindow::isVSync() const {
        return m_Data.VSync;
    }

    void* WindowsWindow::getNativeWindow() const {
        return m_Window;
    }

    Ref<Context> WindowsWindow::getContext() const {
        return m_Context;
    }
}
