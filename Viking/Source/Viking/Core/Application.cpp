#include "vipch.hpp"

#include "Viking/Core/Application.hpp"
#include "Viking/Renderer/Renderer.hpp"


namespace Viking {
    Application* Application::s_Application{ nullptr };

    void Application::init() {
        if(s_Application != nullptr) {
            throw std::runtime_error("Application instance already exists!");
        }

        s_Application = this;

        m_Window = Window::create(WindowProperties());
        m_Window->setEventCallback(VI_BIND_EVENT_FN(Application::onEvent));

        Renderer::init();
    }

    void Application::run() {
        while (m_Running) {
            const auto time = m_Window->getTime();
            m_TimeStep = time - m_LastFrameTime;
            m_LastFrameTime = m_TimeStep;

            m_Window->onUpdate();

            Renderer::drawFrame();
        }
    }

    Ref<Window> Application::getWindow() {
        return m_Window;
    }

    void Application::shutdown() {
        Renderer::cleanup();
    }

    Application* Application::get() {
        return s_Application;
    }

    void Application::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<WindowCloseEvent>(VI_BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(VI_BIND_EVENT_FN(Application::onWindowResize));
    }

    bool Application::onWindowClose([[maybe_unused]] WindowCloseEvent& event)
    {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResize(const WindowResizeEvent& event)
    {
        if (event.getWidth() == 0 || event.getHeight() == 0)
        {
            return false;
        }

        //m_FramebufferResized = true;

        return false;
    }
}
