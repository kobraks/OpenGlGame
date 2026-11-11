#pragma once

#ifdef _WIN32
#ifdef _WIN64
#define ENGINE_PLATFORM_WINDOWS
#else
#error "x86 is unsupported"
#endif // __WIN64
#elif defined(__linux__)
#define ENGINE_PLATFORM_LINUX
#else
#error "Unsuported platform"
#endif //_WIN32

#if defined(ENGINE_DEBUG) && defined(ENGINE_RELEASE)
#error "ENGINE_DEBUG and ENGINE_RELEASE cannot be defined at the same time"
#endif

#if defined(ENGINE_DEBUG) && !defined(ENGINE_DISABLE_ASSERTS)
#define ENGINE_ENABLE_ASSERTS

#ifdef ENGINE_PLATFORM_WINDOWS
#if defined(_MSC_VER)
#include <intrin.h>
#define ENGINE_DEBUG_BREAK() __debugbreak()
#endif
#elif defined(ENGINE_PLATFORM_LINUX)
#include <signal.h>
#define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak"
#endif //ENGINE_PLATFORM_WINDOWS
#else
#define ENGINE_DEBUG_BREAK()
#endif //ENGINE_DEBUG