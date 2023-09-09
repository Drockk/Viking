#include "vk_engine.hpp"
#include "vk_initializers.hpp"
#include "vk_types.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

void ViEngine::init()
{
    if (!glfwInit()) {
        throw std::runtime_error("Cannot initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(m_windowExtent.width, m_windowExtent.height, "Vi Engine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Cannot create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    m_isInitialized = true;
}

void ViEngine::cleanup()
{
    if (m_isInitialized) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void ViEngine::draw()
{
    //TODO: Nothing yet
}

void ViEngine::run()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        draw();
    }
}
