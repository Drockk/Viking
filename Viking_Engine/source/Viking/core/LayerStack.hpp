#ifndef LAYER_STACK_HPP
#define LAYER_STACK_HPP

#include "Viking/core/Layer.hpp"

#include <vector>

namespace vi
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void push_layer(Layer* p_layer);
        void push_overlay(Layer* p_layer);
        void pop_layer(Layer* p_layer);
        void pop_overlay(Layer* p_layer);

        [[nodiscard]] auto begin() { return m_layers.begin(); }
        [[nodiscard]] auto end() { return m_layers.end(); }
        [[nodiscard]] auto rbegin() { return m_layers.rbegin(); }
        [[nodiscard]] auto rend() { return m_layers.rend(); }

        [[nodiscard]] auto begin() const { return m_layers.begin(); }
        [[nodiscard]] auto end() const { return m_layers.end(); }
        [[nodiscard]] auto rbegin() const { return m_layers.rbegin(); }
        [[nodiscard]] auto rend() const { return m_layers.rend(); }

    private:
        std::vector<Layer*> m_layers;
        uint32_t m_layer_insert_index{ 0 };
    };
}

#endif
