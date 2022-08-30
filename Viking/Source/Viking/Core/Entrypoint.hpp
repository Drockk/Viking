#pragma once

#include "Viking/Core/Application.hpp"
#include "Viking/Core/Log.hpp"

extern std::unique_ptr<Viking::Application> createApplication(Viking::ApplicationCommandLineArgs args);

int main(int argc, char** argv) {
    Viking::Log::init();

    try {
        const auto app = createApplication({argc, argv});
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