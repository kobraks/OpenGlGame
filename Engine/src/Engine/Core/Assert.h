#pragma once

#include "Engine/Core/Config.h"
#include "Engine/Core/Log.h"

#include <cstdio>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace Engine::Detail {
	void WriteAssertionFailure(const Ref<spdlog::logger>& logger, std::string_view condition, std::source_location location, std::string_view additionalMessage = {});

	inline void ReportAssertionFailure(const Ref<spdlog::logger>& logger, std::string_view condition, std::source_location location) {
		WriteAssertionFailure(logger, condition, location);
	}

	template<typename... Args>
	inline void ReportAssertionFailure(const Ref<spdlog::logger>& logger, std::string_view condition, std::source_location location, fmt::format_string<Args...> format, Args&&... args) {
		WriteAssertionFailure(logger, condition, location, fmt::format(format, std::forward<Args>(args)...));
	}
}

#ifdef ENGINE_ENABLE_ASSERTS
#define ENGINE_INTERNAL_ASSERT(condition, logger, ...) \
	do { \
		if (!(condition)) [[unlikely]] { \
		::Engine::Detail::ReportAssertionFailure(\
			(logger), #condition, std::source_location::current()\
			__VA_OPT__(,) __VA_ARGS__ \
			); \
			ENGINE_DEBUG_BREAK(); \
		}\
	} while (false)

#define ENGINE_ASSERT(condition, ...) ENGINE_INTERNAL_ASSERT(condition, ::Engine::Log::GetEngineLogger() __VA_OPT__(,) __VA_ARGS__)

#define GAME_ASSERT(condition, ...) ENGINE_INTERNAL_ASSERT(condition, ::Engine::Log::GetApplicationLogger() __VA_OPT__(,) __VA_ARGS__)

#else
#define ENGINE_ASSERT(condition, ...)
#define GAME_ASSERT(condition, ...)
#endif
