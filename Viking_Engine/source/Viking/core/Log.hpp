//
// Created by Bartosz Zielonka on 11.02.2024.
//

#ifndef LOG_HPP
#define LOG_HPP

#include <memory>
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace vi {
    class Log {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger>& get_core_logger() {
            return s_core_logger;
        }
        static std::shared_ptr<spdlog::logger>& get_client_logger() {
            return s_client_logger;
        }
    private:
        inline static std::shared_ptr<spdlog::logger> s_core_logger;
        inline static std::shared_ptr<spdlog::logger> s_client_logger;
    };
}

// Core log macros
#define VI_CORE_TRACE(...)    ::vi::Log::get_core_logger()->trace(__VA_ARGS__)
#define VI_CORE_INFO(...)     ::vi::Log::get_core_logger()->info(__VA_ARGS__)
#define VI_CORE_WARN(...)     ::vi::Log::get_core_logger()->warn(__VA_ARGS__)
#define VI_CORE_ERROR(...)    ::vi::Log::get_core_logger()->error(__VA_ARGS__)
#define VI_CORE_CRITICAL(...) ::vi::Log::get_core_logger()->critical(__VA_ARGS__)

// Client log macros
#define VI_TRACE(...)         ::vi::Log::get_client_logger()->trace(__VA_ARGS__)
#define VI_INFO(...)          ::vi::Log::get_client_logger()->info(__VA_ARGS__)
#define VI_WARN(...)          ::vi::Log::get_client_logger()->warn(__VA_ARGS__)
#define VI_ERROR(...)         ::vi::Log::get_client_logger()->error(__VA_ARGS__)
#define VI_CRITICAL(...)      ::vi::Log::get_client_logger()->critical(__VA_ARGS__)

#endif //LOG_HPP
