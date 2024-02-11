//
// Created by Bartosz Zielonka on 10.02.2024.
//

#include "Application.hpp"

#include "Log.hpp"

namespace vi {
Application::Application(const std::string_view &p_name): m_application_name{p_name} {
}

void Application::init() {
    VI_CORE_INFO("Viking Engine initialized");
}

void Application::run() {

}

void Application::shutdown() {

}
}
