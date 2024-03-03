#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#include "Viking/core/Window.hpp"

#include <memory>
#include <string_view>

namespace vi
{
    class Context
    {
    public:
        virtual ~Context() = default;

        virtual void init(std::string_view p_app_name, const std::shared_ptr<Window>& p_window) = 0;
        virtual void cleanup() = 0;

        static std::shared_ptr<Context> create();
    };
}

#endif
