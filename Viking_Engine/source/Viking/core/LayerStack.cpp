#include "LayerStack.hpp"

#include <algorithm>

namespace vi
{
    LayerStack::~LayerStack()
    {
        std::ranges::for_each(m_layers, [](Layer* p_layer)
        {
            p_layer->on_detach();
        });
    }

    void LayerStack::push_layer(Layer* p_layer)
    {
        m_layers.emplace(m_layers.begin() + m_layer_insert_index, p_layer);
        ++m_layer_insert_index;
    }

    void LayerStack::push_overlay(Layer* p_layer)
    {
        m_layers.emplace_back(p_layer);
    }

    void LayerStack::pop_layer(Layer* p_layer)
    {
        if (const auto it = std::find(m_layers.begin(), m_layers.begin() + m_layer_insert_index, p_layer); it != m_layers.begin() + m_layer_insert_index)
        {
            p_layer->on_detach();
            m_layers.erase(it);
            --m_layer_insert_index;
        }
    }

    void LayerStack::pop_overlay(Layer* p_layer)
    {
        if (const auto it = std::find(m_layers.begin(), m_layers.begin() + m_layer_insert_index, p_layer); it != m_layers.end())
        {
            p_layer->on_detach();
            m_layers.erase(it);
        }
    }
}
