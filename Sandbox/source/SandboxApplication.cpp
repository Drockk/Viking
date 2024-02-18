//
// Created by Bartosz Zielonka on 10.02.2024.
//

#include <Viking.hpp>

namespace snadbox {
    class SandboxApplication final: public vi::Application {
    public:
        SandboxApplication(): Application("Sandbox") {

        }
    };
}

std::unique_ptr<vi::Application> create_application() {
    return std::make_unique<snadbox::SandboxApplication>();
}
