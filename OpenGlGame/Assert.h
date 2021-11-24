#pragma once
#include "Log.h"
#include "Core.h"

#include <filesystem>

#ifdef GAME_ENABLE_ASSERTS

#define GAME_INTERNAL_ASSERT_IMPL(check, msg, ...) {if (!(check)) { ::Game::Log::GetAssertionLogger()->error(msg, __VA_ARGS__); GAME_DEBUGBREAK(); } }

#define GAME_INTERNAL_ASSERT_WITH_MSG(check, ...) GAME_INTERNAL_ASSERT_IMPL(check, "Assertion failed: {0}", fmt::format(__VA_ARGS__))
#define GAME_INTERNAL_ASSERT_NO_MSG(check) GAME_INTERNAL_ASSERT_IMPL(check, "Assertion '{0}' failed at {1}:{2}", GAME_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define GAME_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, macro, ...) macro
#define GAME_INTERNAL_ASSERT_GET_MACRO(...) GAME_EXPAND_MACRO(GAME_INTERNAL_ASSERT_GET_MACRO_NAME(GAME_INTERNAL_ASSERT_WITH_MSG, GAME_INTERNAL_ASSERT_NO_MSG))

#define ASSERT(...) GAME_EXPAND_MACRO(GAME_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
#define ASSERT(x, ...) 
#endif