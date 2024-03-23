//
// Created by Bartosz Zielonka on 11.02.2024.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <memory>
#include <string>

namespace vi
{
    struct WindowProps {
    std::string Title{};
    std::pair<int32_t, int32_t> Size{};

    explicit WindowProps(std::string p_title = "Viking Engine", const std::pair<int32_t, int32_t> p_size = {800, 600}): Title{
        std::move(p_title)
    }, Size{p_size} {}
};

class Window {
public:
    virtual ~Window() = default;

    virtual void on_update() = 0;
    virtual void on_swap() = 0;

    [[nodiscard]] virtual std::pair<int32_t, int32_t> get_size() const = 0;

    [[nodiscard]] virtual float get_time() const = 0;

    [[nodiscard]] static std::shared_ptr<Window> create(const WindowProps& p_props = WindowProps());
};
}

#endif //WINDOW_HPP
