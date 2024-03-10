#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Viking/core/Window.hpp"

#include <memory>
#include <string_view>

namespace vi
{
    class Renderer
    {
    public:
        void init(std::string_view p_app_name, const std::shared_ptr<Window>& p_window);
        void shutdown();

        void begin_frame();
        void end_frame();
    };
}

#endif
