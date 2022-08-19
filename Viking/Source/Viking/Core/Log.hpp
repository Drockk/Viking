#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


namespace Viking {
    class Log {
    public:
        Log() = default;
        ~Log() = default;

        static void init();

        static std::shared_ptr<spdlog::logger>& getCoreLogger();
        static std::shared_ptr<spdlog::logger>& getClientLogger();
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

// Core log macros
#define VI_CORE_TRACE(...)    ::Viking::Log::getCoreLogger()->trace(__VA_ARGS__)
#define VI_CORE_DEBUG(...)    ::Viking::Log::getCoreLogger()->debug(__VA_ARGS__)
#define VI_CORE_INFO(...)     ::Viking::Log::getCoreLogger()->info(__VA_ARGS__)
#define VI_CORE_WARN(...)     ::Viking::Log::getCoreLogger()->warn(__VA_ARGS__)
#define VI_CORE_ERROR(...)    ::Viking::Log::getCoreLogger()->error(__VA_ARGS__)
#define VI_CORE_CRITICAL(...) ::Viking::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define VI_TRACE(...)         ::Viking::Log::getClientLogger()->trace(__VA_ARGS__)
#define VI_DEBUG(...)         ::Viking::Log::getClientLogger()->debug(__VA_ARGS__)
#define VI_INFO(...)          ::Viking::Log::getClientLogger()->info(__VA_ARGS__)
#define VI_WARN(...)          ::Viking::Log::getClientLogger()->warn(__VA_ARGS__)
#define VI_ERROR(...)         ::Viking::Log::getClientLogger()->error(__VA_ARGS__)
#define VI_CRITICAL(...)      ::Viking::Log::getClientLogger()->critical(__VA_ARGS__)
