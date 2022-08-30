#include "vipch.hpp"

#include "Viking/Core/Application.hpp"
#include "Viking/Core/Log.hpp"

namespace Viking {
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, ApplicationCommandLineArgs args): m_CommandLineArgs(args) {
        VI_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }

    void Application::close() {
        m_Running = false;
    }

    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(VI_BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(VI_BIND_EVENT_FN(Application::onWindowResize));
    }

    void Application::run() {
        while(m_Running) {
            runTemp();
        }
    }

    bool Application::onWindowClose(WindowCloseEvent&) {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        if (e.getWidth() == 0 || e.getHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        return false;
    }
}
