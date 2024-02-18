//
// Created by Bartosz Zielonka on 10.02.2024.
//

#include "Application.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include "Log.hpp"

namespace vi {
Application::Application(const std::string_view &p_name): m_application_name{p_name}
{
}

void Application::init()
{
    m_window = Window::create(WindowProps{m_application_name, {800, 600}});
    VI_CORE_INFO("{} initialized", m_application_name);

    EventDispatcher::add_listener(EventType::WindowClose, [this](const EventPointer&)
    {
        m_running = false;
    });
}

void Application::run()
{
    while (m_running)
    {
        EventDispatcher::dispatch();
        m_window->on_update();
        //m_window->on_swap(); // TODO: Commented out till Vulkan renderer will be implemented
    }
}

void Application::shutdown()
{
    VI_CORE_INFO("{} closed", m_application_name);
}
}
