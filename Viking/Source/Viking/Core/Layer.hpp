#pragma once

#include "Viking/Core/Base.hpp"
#include "Viking/Core/Timestep.hpp"
#include "Viking/Events/Event.hpp"

namespace Viking {
    class Layer {
    public:
        Layer(std::string name = "Layer");
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(Timestep timeStep) {}
        virtual void onImGuiRender() {}
        virtual void onEvent(Event& event) {}

        [[nodiscard]] const std::string& getName() const {
            return m_DebugName;
        }
    protected:
        std::string m_DebugName;
    };
}
