//
// Created by Bartosz Zielonka on 18.02.2024.
//

#ifndef WINDOWS_WINDOW_HPP
#define WINDOWS_WINDOW_HPP

#include "Viking/core/Window.hpp"

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace windows {
class Window: public vi::Window {
public:
    Window(vi::WindowProps p_props);
    ~Window() override;

    void on_update() override;
    void on_swap() override;
    [[nodiscard]] std::pair<int32_t, int32_t> get_size() const override;
    [[nodiscard]] float get_time() const override;

    [[nodiscard]] VkSurfaceKHR create_surface(VkInstance p_instance) const;

private:
    static void init();
    void create_window();

    vi::WindowProps m_window_props{};
    GLFWwindow* m_window{ nullptr };
    bool vsync{ false };
};
}

#endif //WINDOWS_WINDOW_HPP
