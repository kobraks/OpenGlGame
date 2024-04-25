#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define GAME_WINDOWS_PLATFORM
#else
#error "x86 is unsupported"
#endif // __WIN64
#elif defined(__LINUX__)
#define GAME_LINUX_PLATFORM
#else
#error "Unsuported"
#endif //_WIN32

#ifdef GAME_DEBUG
#define GAME_ENABLE_ASSERT
#define GAME_ENABLE_PROFILING

#ifdef GAME_WINDOWS_PLATFORM
#define GAME_DEBUGBREAK() __debugbreak()
#elif defined(GAME_LINUX_PLATFORM)
#include <signal.h>
#define GAME_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak"
#endif //GAME_WINDOWS_PLATFORM
#else
#define GAME_DEBUGBREAK()
#endif //GAME_DEBUG