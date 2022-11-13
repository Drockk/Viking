#pragma once
#include "Viking/Core/Base.hpp"
#include "Viking/Events/Event.hpp"

namespace Viking {
    struct WindowProperties {
        std::string Title{};
        uint32_t Width{};
        uint32_t Height{};
        WindowProperties(std::string title = "Viking Engine", const uint32_t width = 1600, const uint32_t height = 900): Title(std::move(title)), Width(width), Height(height) {}
    };

    class Window {
    public:
        using EventCallbackFunction = std::function<void(Event&)>;
        Window() = default;
        virtual ~Window() = default;

        virtual void onUpdate() = 0;
        virtual uint32_t getWidth() = 0;
        virtual uint32_t getHeight() = 0;

        virtual void setEventCallback(const EventCallbackFunction& callback) = 0;
        virtual void setVSync(bool enabled) = 0;
        [[nodiscard]] virtual bool isVSync() const = 0;

        [[nodiscard]] virtual void* getNativeWindow() const = 0;

        static std::shared_ptr<Window> create(const WindowProperties& properties);
    };
}
