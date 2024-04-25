#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

#include <filesystem>

#ifdef GAME_ENABLE_ASSERT
#define GAME_INTERNAL_ASSERT_IMPL(type, check, msg, ...) {if (!(check)) { LOG##type##ERROR(msg, __VA_ARGS__); GAME_DEBUGBREAK(); } }

#define GAME_INTERNAL_ASSERT_WITH_MSG(type, check, ...) GAME_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define GAME_INTERNAL_ASSERT_NO_MSG(type, check) GAME_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", GAME_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define GAME_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define GAME_INTERNAL_ASSERT_GET_MACRO(...) GAME_EXPAND_MACRO(GAME_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, GAME_INTERNAL_ASSERT_WITH_MSG, GAME_INTERNAL_ASSERT_NO_MSG))

#define GAME_ASSERT(...) GAME_EXPAND_MACRO(GAME_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define ENGINE_ASSERT(...) GAME_EXPAND_MACRO(GAME_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_ENGINE_, __VA_ARGS__))
#else
#define GAME_ASSERT(...)
#define ENGINE_ASSERT(...)
#endif