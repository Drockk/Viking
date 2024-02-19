//
// Created by Bartosz Zielonka on 10.02.2024.
//
#include "SandboxLayer.hpp"
#include <Viking.hpp>

namespace sandbox {
    class SandboxApplication final: public vi::Application {
    public:
        SandboxApplication(): Application("Sandbox") {
            push_layer(new SandboxLayer);
        }
    };
}

std::unique_ptr<vi::Application> create_application() {
    return std::make_unique<sandbox::SandboxApplication>();
}
