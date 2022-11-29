#pragma once

#include <memory>

#include "Viking/Core/Window.hpp"
#include "Viking/Events/ApplicationEvent.hpp"


namespace Viking {
    class Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void init();
        void run();

        Ref<Window> getWindow();

        static void shutdown();

        static Application* get();
    private:
        void onEvent(Event& event);
        bool onWindowClose(WindowCloseEvent& event);
        bool onWindowResize(const WindowResizeEvent& event);

        bool m_Running{ true };

        Ref<Window> m_Window;
        float m_TimeStep{0};
        float m_LastFrameTime{0};

        static Application* s_Application;
    };

    std::unique_ptr<Application> createApplication();
}
