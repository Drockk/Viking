#include "vipch.hpp"

#include "Viking/Core/Application.hpp"
#include "Viking/Core/Log.hpp"

#include <GLFW/glfw3.h>

namespace Viking {
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, const ApplicationCommandLineArgs args): m_CommandLineArgs(args) {
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

        for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (e.Handled)
                break;

            (*it)->onEvent(e);
        }
    }

    void Application::pushLayer(Layer* layer) {
        m_LayerStack.pushLayer(layer);
        layer->onAttach();
    }

    void Application::pushOverlay(Layer* layer) {
        m_LayerStack.pushOverlay(layer);
        layer->onAttach();
    }


    void Application::run() {
        while(m_Running) {
            const auto time = static_cast<float>(glfwGetTime());
            TimeStep timeStep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if(!m_Minimized) {
                {
                    for (auto* layer : m_LayerStack) {
                        layer->onUpdate(timeStep);
                    }
                }
            }
            runTemp();
        }
    }

    bool Application::onWindowClose([[maybe_unused]] WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResize(const WindowResizeEvent& e) {
        if (e.getWidth() == 0 || e.getHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        return false;
    }
}
