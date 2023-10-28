//
// Created by batzi on 28.10.2023.
//

#ifndef VIKING_WINDOW_HPP
#define VIKING_WINDOW_HPP

#include <string>
#include <utility>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace vi {
    class Window {
    public:
        Window() = delete;
        Window(std::string_view t_title, std::pair<int32_t, int32_t> t_resolution);
        ~Window();

        VkSurfaceKHR create_surface(VkInstance t_instance);

        void on_update();
        bool should_close();

    private:
        GLFWwindow* m_window{nullptr};
    };
} // vi

#endif //VIKING_WINDOW_HPP
