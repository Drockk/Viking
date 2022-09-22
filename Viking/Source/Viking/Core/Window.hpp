#pragma once

#include <sstream>

#include "Viking/Core/Base.hpp"
#include "Viking/Events/Event.hpp"

namespace Viking {
    struct WindowProps {
        std::string Title{};
        uint32_t Width{};
        uint32_t Height{};

        WindowProps(std::string title = "Viking Engine", uint32_t width = 1600, uint32_t height = 900):Title(std::move(
            title)), Width(width), Height(height) {}
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        [[nodiscard]] virtual uint32_t getWidth() const = 0;
        [[nodiscard]] virtual uint32_t getHeight() const = 0;

        //Window attributes
        virtual void setEventCallback(const EventCallbackFn& callback) = 0;

        virtual void setVSync(bool enabled) = 0;
        [[nodiscard]] virtual bool isVSync() const = 0;

        [[nodiscard]] virtual void* getNativeWindow() const = 0;

        static Scope<Window> create(const WindowProps& props = WindowProps());
    };
}
