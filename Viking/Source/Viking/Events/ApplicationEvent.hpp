#pragma once

#include "Viking/Events/Event.hpp"

namespace Viking {
    class WindowResizeEvent: public Event {
    public:
        WindowResizeEvent(const uint32_t width, const uint32_t height): m_Width(width), m_Height(height) {}

        [[nodiscard]] uint32_t getWidth() const {
            return m_Width;
        }

        [[nodiscard]] uint32_t getHeight() const {
            return  m_Height;
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss{};
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }
    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };
}
