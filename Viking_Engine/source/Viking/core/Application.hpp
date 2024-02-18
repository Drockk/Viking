//
// Created by Bartosz Zielonka on 10.02.2024.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.hpp"

#include <memory>

namespace vi {
class Application {
public:
    explicit Application(const std::string_view& p_name);

    void init();
    void run();
    void shutdown();

private:
    std::string m_application_name{};
    std::shared_ptr<Window> m_window;
    bool m_running{ true };
};
}

std::unique_ptr<vi::Application> create_application();

#endif //APPLICATION_H
