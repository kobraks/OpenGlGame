#pragma once

#include <memory>

#define SPDLOG_FMT_EXTERNAL

#ifdef GAME_DEBUG
	#define GAME_ENABLE_ASSERTS
	#define GAME_ENABLE_PROFILING

	#define GAME_DEBUGBREAK() __debugbreak()
#else
	#define GAME_DEBUGBREAK()
#endif

#define GAME_EXPAND_MACRO(x) x
#define GAME_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


namespace Game
{
	template <typename Type>
	using Ref = std::shared_ptr<Type>;

	template <typename Type, typename ...Args>
	constexpr Ref<Type> CreateRef(Args && ... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}

	template <typename Type, typename ...Args>
	constexpr Ref<Type> MakeRef(Args &&... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}

	template <typename Type>
	using Pointer = std::shared_ptr<Type>;

	template <typename Type, typename ... Args>
	constexpr Pointer<Type> CreatePointer(Args && ... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}

	template <typename Type, typename ...Args>
	constexpr Pointer<Type> MakePointer(Args &&... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}
	
	template <typename Type>
	using Scope = std::unique_ptr<Type>;

	template <typename Type, typename ...Args>
	constexpr Scope<Type> CreateScope(Args &&... args)
	{
		return std::make_unique<Type>(std::forward<Args>(args)...);
	}

	template <typename Type, typename ...Args>
	constexpr Scope<Type> MakeScope(Args &&... args)
	{
		return std::make_unique<Type>(std::forward<Args>(args)...);
	}
}

#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"