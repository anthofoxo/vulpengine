#pragma once

/*!
Simple log macros, will default expand into standard compliant logs

If spdlog is detected then will prefer to expand using spdlog
*/

#include "vulpengine/vp_features.hpp"

#ifdef VP_HAS_SPDLOG
#	include <spdlog/spdlog.h>
#	define VP_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#	define VP_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#	define VP_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#	define VP_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#	define VP_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#	define VP_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)
#else
#	include <iostream>
#	include <format>
#	define VP_LOG_TRACE(...) std::cout << std::format(__VA_ARGS__) << '\n'
#	define VP_LOG_DEBUG(...) std::cout << std::format(__VA_ARGS__) << '\n'
#	define VP_LOG_INFO(...) std::cout << std::format(__VA_ARGS__) << '\n'
#	define VP_LOG_WARN(...) std::cout << std::format(__VA_ARGS__) << '\n'
#	define VP_LOG_ERROR(...) std::cerr << std::format(__VA_ARGS__) << '\n'
#	define VP_LOG_CRITICAL(...) std::cerr << std::format(__VA_ARGS__) << '\n'
#endif