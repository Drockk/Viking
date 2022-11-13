#pragma once

#include "Viking/Events/Event.hpp"
#include "Viking/Core/KeyCodes.hpp"

#include <sstream>

namespace Viking {
    class KeyEvent : public Event {
    public:
        [[nodiscard]] KeyCode getKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(const KeyCode keycode): m_KeyCode(keycode) {}

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(const KeyCode keycode, bool isRepeat = false): KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        [[nodiscard]] bool isRepeat() const { return m_IsRepeat; }

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(const KeyCode keycode): KeyEvent(keycode) {}

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(const KeyCode keycode): KeyEvent(keycode) {}

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}