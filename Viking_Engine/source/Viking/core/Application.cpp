//
// Created by Bartosz Zielonka on 10.02.2024.
//

#include "Viking/core/Application.hpp"
#include "Viking/core/Log.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include <algorithm>

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

    m_renderer.init(m_application_name, m_window);
}

void Application::run()
{
    while (m_running)
    {
        EventDispatcher::dispatch();

        const auto now = m_window->get_time();
        TimeStep time_step = now - m_last_frame_time;
        m_last_frame_time = now;

        std::ranges::for_each(m_layer_stack, [&time_step](Layer* p_layer)
        {
            p_layer->on_update(time_step);
        });

        //TODO: update on imgui layer

        m_renderer.begin_frame();
        m_renderer.end_frame();

        m_window->on_update();
    }
}

void Application::shutdown()
{
    m_renderer.shutdown();
    VI_CORE_INFO("{} closed", m_application_name);
}

void Application::push_layer(Layer* p_layer)
{
    m_layer_stack.push_layer(p_layer);
    p_layer->on_attach();
}

void Application::push_overlay(Layer* p_layer)
{
    m_layer_stack.push_overlay(p_layer);
    p_layer->on_attach();
}
}
