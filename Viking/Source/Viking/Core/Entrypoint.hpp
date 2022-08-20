#pragma once

#include "Viking/Core/Application.hpp"
#include "Viking/Core/Log.hpp"

extern std::unique_ptr<Viking::Application> createApplication();

int main() {
    Viking::Log::init();

    try {
        const auto app = createApplication();
        app->init();
        app->run();
        app->shutdown();
    }
    catch (const std::exception& e) {
        VI_CORE_CRITICAL("{0}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}