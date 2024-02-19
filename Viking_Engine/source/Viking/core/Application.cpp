//
// Created by Bartosz Zielonka on 10.02.2024.
//

#include "Application.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include "Log.hpp"

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
}

void Application::run()
{
    while (m_running)
    {
        EventDispatcher::dispatch();

        //TODO: Get Time step

        std::ranges::for_each(m_layer_stack, [](Layer* p_layer)
        {
            p_layer->on_update(0.0f);
        });

        //TODO: update on imgui layer

        m_window->on_update();
        //m_window->on_swap(); // TODO: Commented out till Vulkan renderer will be implemented
    }
}

void Application::shutdown()
{
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
