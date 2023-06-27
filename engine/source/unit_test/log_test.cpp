#include <iostream>

#include "runtime/core/log/log_system.h"

using namespace ArchViz;

int main()
{
    LogSystem::getInstance().log(LogSystem::LogLevel::debug, "[" + std::string(__FUNCTION__) + "] " + "Hello World");
    LogSystem::getInstance().log(LogSystem::LogLevel::info, "[" + std::string(__FUNCTION__) + "] " + "Hello World");
    LogSystem::getInstance().log(LogSystem::LogLevel::warn, "[" + std::string(__FUNCTION__) + "] " + "Hello World");
    LogSystem::getInstance().log(LogSystem::LogLevel::error, "[" + std::string(__FUNCTION__) + "] " + "Hello World");
    LogSystem::getInstance().log(LogSystem::LogLevel::fatal, "[" + std::string(__FUNCTION__) + "] " + "Hello World");

    return 0;
}
