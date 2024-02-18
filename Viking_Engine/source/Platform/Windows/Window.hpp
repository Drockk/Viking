//
// Created by Bartosz Zielonka on 18.02.2024.
//

#ifndef WINDOWS_WINDOW_HPP
#define WINDOWS_WINDOW_HPP

#include "Viking/core/Window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Windows {
class Window: public vi::Window {
public:
    Window(vi::WindowProps p_props);
    ~Window() override;

    void on_update() override;
    void on_swap() override;
    [[nodiscard]] std::pair<int32_t, int32_t> get_size() const override;
    void set_vsync(bool p_enabled) override;
    [[nodiscard]] bool is_vsync() const override;

private:
    void create_window();

    vi::WindowProps m_window_props{};
    GLFWwindow* m_window{ nullptr };
    bool vsync{ false };
};
}

#endif //WINDOWS_WINDOW_HPP
