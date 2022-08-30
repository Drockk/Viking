#pragma once

#include "Viking/Events/Event.hpp"
#include "Viking/Core/MouseCodes.hpp"

namespace Viking {
    class MouseMovedEvent: public Event {
    public:
        explicit MouseMovedEvent(const float x, const float y): m_MouseX(x), m_MouseY(y) {
        }

        [[nodiscard]] float getX() const {
            return m_MouseX;
        }

        [[nodiscard]] float getY() const {
            return m_MouseY;
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent: public Event {
    public:
        explicit MouseScrolledEvent(const float xOffset, const float yOffset): m_XOffset(xOffset), m_YOffset(yOffset) {
        }

        [[nodiscard]] float getXOffset() const {
            return m_XOffset;
        }

        [[nodiscard]] float getYOffset() const {
            return m_YOffset;
        }

        [[nodiscard]] std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent: public Event {
    public:
        [[nodiscard]] MouseCode getMouseButton() const {
            return m_Button;
        }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    protected:
        explicit MouseButtonEvent(const MouseCode button): m_Button(button) {
        }

        MouseCode m_Button;
    };

    class MouseButtonPressedEvent: public MouseButtonEvent {
    public:
        explicit MouseButtonPressedEvent(const MouseCode button): MouseButtonEvent(button) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent: public MouseButtonEvent {
    public:
        explicit MouseButtonReleasedEvent(const MouseCode button): MouseButtonEvent(button) {
        }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}
