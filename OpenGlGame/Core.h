#pragma once

#ifdef NDEBUG
#else
#define GAME_DEBUG 1
#define GAME_ENABLE_ASSERTS 1
#define ENABLE_PROFILING 1
#endif

#ifdef GAME_DEBUG
#define GAME_DEBUGBREAK() __debugbreak()
#endif

#define GAME_EXPAND_MACRO(x) x
#define GAME_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Types.h"