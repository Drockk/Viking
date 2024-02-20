//
// Created by Bartosz Zielonka on 10.02.2024.
//

#ifndef ENTRYPOINT_HPP
#define ENTRYPOINT_HPP

#include "Application.hpp"
#include "Log.hpp"

int main() {
    vi::Log::init();

    try {
        const auto app = create_application();
        app->init();
        app->run();
        app->shutdown();
    }
    catch (const std::exception& p_exception) {
        VI_CRITICAL("{}", p_exception.what());
    }

    return 0;
}

#endif //ENTRYPOINT_HPP
