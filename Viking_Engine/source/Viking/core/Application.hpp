//
// Created by Bartosz Zielonka on 10.02.2024.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <string_view>

namespace vi {
class Application {
public:
    explicit Application(const std::string_view& p_name);

    void init();
    void run();
    void shutdown();

private:
    std::string m_application_name{};
};
}

std::unique_ptr<vi::Application> create_application();

#endif //APPLICATION_H
