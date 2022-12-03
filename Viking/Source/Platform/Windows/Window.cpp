#include "vipch.hpp"
#include "Platform/Windows/Window.hpp"

#include "Viking/Events/ApplicationEvent.hpp"
#include "Viking/Events/KeyEvent.hpp"
#include "Viking/Events/MouseEvent.hpp"

namespace Windows {
    static uint8_t s_GLFWWindowCount{ 0 };

    static void GLFWErrorCallback(int error, const char* description) {
        std::cerr << "GLFW Error (" << error << ") " << description << "\n";
    }

    Window::Window(const Viking::WindowProperties& properties) {
        init(properties);
    }

    Window::~Window() {
        shutdown();
    }

    void Window::onUpdate() {
        glfwPollEvents();
    }

    uint32_t Window::getWidth() {
        return m_Data.Width;
    }

    uint32_t Window::getHeight() {
        return m_Data.Height;
    }

    float Window::getTime() const {
        return static_cast<float>(glfwGetTime());
    }

    void Window::setEventCallback(const EventCallbackFunction& callback) {
        m_Data.EventCallback = callback;
    }

    void Window::setVSync(const bool enabled) {
        if (enabled) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }

        m_Data.VSync = enabled;
    }

    bool Window::isVSync() const {
        return m_Data.VSync;
    }

    void* Window::getNativeWindow() const {
        return m_Window;
    }

    void Window::init(const Viking::WindowProperties& properties) {
        m_Data.Title = properties.Title;
        m_Data.Width = properties.Width;
        m_Data.Height = properties.Height;

        std::cout << "Creating Window " << properties.Title << "(" << properties.Width << " " << properties.Height << ")\n";

        if (s_GLFWWindowCount == 0) {
            if(!glfwInit()) {
                throw std::runtime_error("Could not initialize GLFW");
            }

            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(static_cast<int>(properties.Width), static_cast<int>(properties.Height), properties.Title.c_str(), nullptr, nullptr);
        if (!m_Window) {
            glfwTerminate();
            throw std::runtime_error("Could not create window");
        }

        ++s_GLFWWindowCount;

        glfwSetWindowUserPointer(m_Window, &m_Data);
        setVSync(true);

        //Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.Width = width;
            data.Height = height;

            Viking::WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            Viking::WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, int mods) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS:
            {
                Viking::KeyPressedEvent event((Viking::KeyCode)key, false);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                Viking::KeyReleasedEvent event(static_cast<Viking::KeyCode>(key));
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                Viking::KeyPressedEvent event((Viking::KeyCode)key, true);
                data.EventCallback(event);
                break;
            }
            default: ;
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            Viking::KeyTypedEvent event(static_cast<Viking::KeyCode>(keycode));
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS:
            {  
                Viking::MouseButtonPressedEvent event(static_cast<Viking::MouseCode>(button));
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                Viking::MouseButtonReleasedEvent event(static_cast<Viking::MouseCode>(button));
                data.EventCallback(event);
                break;
            }
            default: ;
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            Viking::MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            Viking::MouseMovedEvent event(xPos, yPos);
            data.EventCallback(event);
        });

        m_Context = Viking::createRef<Vulkan::Context>();
        m_Context->createSurface(m_Window);
        m_Context->createPhysicalDevice();
        m_Context->createDevice();
    }

    void Window::shutdown() const {
        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0) {
            glfwTerminate();
        }
    }
}
