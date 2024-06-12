#pragma once
#include <spdlog/spdlog.h>

// Переопределение методов библиотеки SPDLOG в препроцессорные макросы
namespace r_engine
{
    #define LOG_INFO(...) spdlog::info(__VA_ARGS__)
    #define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
    #define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
    #define LOG_CRIT(...) spdlog::critical(__VA_ARGS__)
}