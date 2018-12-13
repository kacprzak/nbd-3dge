#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#ifdef _MSC_VER
#include <spdlog/sinks/msvc_sink.h>
#else
#include <spdlog/sinks/stdout_sinks.h>
#endif

#define LOG_TRACE(...) spdlog::get("console")->log(spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) spdlog::get("console")->log(spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) spdlog::get("console")->log(spdlog::level::info, __VA_ARGS__)
#define LOG_WARNING(...) spdlog::get("console")->log(spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) spdlog::get("console")->log(spdlog::level::err, __VA_ARGS__)
#define LOG_FATAL(...) spdlog::get("console")->log(spdlog::level::critical, __VA_ARGS__)

void logGlError();

#define LOG_GL_ERROR logGlError()

#endif // LOGGER_H
