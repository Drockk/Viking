#pragma once
#include "vk_types.hpp"

struct GLFWwindow;

class ViEngine
{
public:
    bool m_isInitialized{ false };
    int m_frameNumber{ 0 };

    VkExtent2D m_windowExtent{ 1600, 900 };
    GLFWwindow* m_window{ nullptr };

    void init();
    void cleanup();
    void draw();
    void run();
};
