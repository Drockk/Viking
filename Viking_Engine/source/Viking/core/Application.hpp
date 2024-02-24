//
// Created by Bartosz Zielonka on 10.02.2024.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Viking/core/LayerStack.hpp"
#include "Viking/core/Timestep.hpp"
#include "Viking/core/Window.hpp"
#include "Viking/renderer/Renderer.hpp"

#include <memory>

namespace vi {
class Application {
public:
    explicit Application(const std::string_view& p_name);

    void init();
    void run();
    void shutdown();

    void push_layer(Layer* p_layer);
    void push_overlay(Layer* p_layer);

private:
    std::string m_application_name{};
    std::shared_ptr<Window> m_window;
    bool m_running{ true };
    LayerStack m_layer_stack;
    TimeStep m_last_frame_time;
    Renderer m_renderer;
};
}

std::unique_ptr<vi::Application> create_application();

#endif //APPLICATION_H
