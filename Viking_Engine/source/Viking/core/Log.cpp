//
// Created by Bartosz Zielonka on 11.02.2024.
//

#include "Log.hpp"
#pragma warning(push, 0)
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace vi {
    void Log::init() {
        auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();

        const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("%^[%T] %n: %v%$");
        dist_sink->add_sink(console_sink);

        const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Viking.log", true);
        file_sink->set_pattern("[%T] [%l] %n: %v");
        dist_sink->add_sink(file_sink);

        s_core_logger = std::make_shared<spdlog::logger>("VIKING", dist_sink);
        s_core_logger->set_level(spdlog::level::trace);
        s_core_logger->flush_on(spdlog::level::trace);

        s_client_logger = std::make_shared<spdlog::logger>("APP", dist_sink);
        s_client_logger->set_level(spdlog::level::trace);
        s_client_logger->flush_on(spdlog::level::trace);
    }
}
