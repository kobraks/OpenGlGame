#pragma once

#ifdef GAME_DEBUG
#define GAME_ENABLE_ASSERT
#define GAME_ENABLE_PROFILING

#define GAME_DEBUGBREAK() __debugbreak()
#else
#define GAME_DEBUGBREAK()
#endif