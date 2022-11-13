#include "vipch.hpp"
#include "Viking/Core/Window.hpp"

#ifdef VI_PLATFORM_WINDOWS
#include "Platform/Windows/Window.hpp"
#endif

namespace Viking {

    std::shared_ptr<Window> Window::create(const WindowProperties& properties) {
#ifdef VI_PLATFORM_WINDOWS
        return std::make_shared<Windows::Window>(properties);
#else
        static_assert("Unknown platform!");
#endif
    }
}