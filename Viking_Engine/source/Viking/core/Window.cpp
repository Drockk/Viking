//
// Created by Bartosz Zielonka on 11.02.2024.
//

#include "Viking/core/Window.hpp"
#include "Platform/Windows/Window.hpp"

namespace vi {
    std::shared_ptr<Window> Window::create(const WindowProps &p_props) {
        return std::make_shared<Windows::Window>(p_props);
    }

}
