#pragma once
#include "runtime/core/log/log_system.h"

#include <cassert>
#include <chrono>
#include <thread>


#ifdef UNIT_TEST
#define LOG_HELPER(LOG_LEVEL, ...)
#else
#define LOG_HELPER(LOG_LEVEL, ...) LogSystem::getInstance().log(LOG_LEVEL, "[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__);
#endif

#define LOG_DEBUG(...) LOG_HELPER(LogSystem::LogLevel::debug, __VA_ARGS__);

#define LOG_INFO(...) LOG_HELPER(LogSystem::LogLevel::info, __VA_ARGS__);

#define LOG_WARN(...) LOG_HELPER(LogSystem::LogLevel::warn, __VA_ARGS__);

#define LOG_ERROR(...) LOG_HELPER(LogSystem::LogLevel::error, __VA_ARGS__);

#define LOG_FATAL(...) LOG_HELPER(LogSystem::LogLevel::fatal, __VA_ARGS__);

#define ArchVizSleep(_ms) std::this_thread::sleep_for(std::chrono::milliseconds(_ms));

#define ArchVizNameOf(name) #name

#ifdef NDEBUG
#define ASSERT(statement)
#else
#define ASSERT(statement) assert(statement)
#endif

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define ARCHVIZ_XSTR(s) ARCHVIZ_STR(s)
#define ARCHVIZ_STR(s) #s
