#pragma once
#include "Log.h"

#ifdef NDEBUG
#else
#define GAME_ENABLE_ASSERTS 1
#define ENABLE_PROFILING 1
#endif

#ifdef GAME_ENABLE_ASSERTS
#define ASSERT(x, ...) { if(!(x)) { ::Game::Log::GetAssertionLogger()->error("Assertion Failed: {0}", fmt::format(__VA_ARGS__)); __debugbreak(); }}
#else
#define ASSERT(x, ...) 
#endif