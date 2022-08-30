#pragma once
#include "Viking/Core/Base.hpp"

#include "Viking/Events/Event.hpp"
#include "Viking/Events/ApplicationEvent.hpp"

int main(int argc, char** argv);

namespace Viking {
    struct ApplicationCommandLineArgs {
        int Count{ 0 };
        char** Args{ nullptr };

        const char* operator[](const int index) const {
            VI_CORE_ASSERT(index < Count);
            return Args[index];
        }
    };

    class Application {
    public:
        Application(const std::string& name = "Viking App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
        virtual ~Application() = default;

        void onEvent(Event& e);

        void close();

        static Application& get() {
            return *s_Instance;
        }

        [[nodiscard]] ApplicationCommandLineArgs getCommandLine() const {
            return m_CommandLineArgs;
        }

        //Temp
        virtual void init() = 0;
        virtual void runTemp() = 0;
        virtual void shutdown() = 0;

    private:
        void run();
        bool onWindowClose(WindowCloseEvent&);
        bool onWindowResize(WindowResizeEvent& e);

        ApplicationCommandLineArgs m_CommandLineArgs;
        bool m_Running{ true };
        bool m_Minimized{ false };

        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };

    std::unique_ptr<Application> createApplication(ApplicationCommandLineArgs args);
}
