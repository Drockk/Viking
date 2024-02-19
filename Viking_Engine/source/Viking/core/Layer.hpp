#ifndef LAYER_HPP
#define LAYER_HPP
#include "Viking/core/TimeStep.hpp"
#include "Viking/event/DispatcherEvent.hpp"

#include <string_view>

namespace vi
{
    class Layer
    {
    public:
        Layer(const std::string_view& p_name = "Layer"): m_debug_name{ p_name } {}
        virtual ~Layer() = default;

        virtual void on_attach() {}
        virtual void on_detach() {}
        virtual void on_update(const TimeStep& p_time_step) {}
        virtual void on_imgui_render() {}
        virtual void on_event(EventPointer& p_event) {}

        [[nodiscard]] const std::string& get_name() const { return m_debug_name; }

    private:
        std::string m_debug_name{};
    };
}

#endif // !LAYER_HPP
