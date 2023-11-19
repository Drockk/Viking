#pragma once

#include <utility>

#include <vulkan/vulkan.hpp>
#include "GLFW/glfw3.h"

namespace vi {
    class Window {
    public:
        Window() = delete;
        Window(std::string_view p_title, std::pair<int32_t, int32_t> p_resolution);
        ~Window();

        VkSurfaceKHR create_surface(VkInstance p_instance) const;

        static void on_update();
        [[nodiscard]] bool should_close() const;

    private:
        GLFWwindow* m_window{nullptr};
    };
} // vi
