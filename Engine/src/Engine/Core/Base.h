#pragma once

#include "Engine/Core/Config.h"

#include <memory>

#define GAME_EXPAND_MACRO(x) x
#define GAME_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto &&... args) ->decltype(auto) { return this->fn(std::forward<decltype>(args)>(args)...); }

namespace Engine {
	template <typename T>
	using Pointer = std::shared_ptr<T>;

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Pointer<T> MakePointer(Args &&...  args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ... Args>
	constexpr Scope<T> MakeScope(Args &&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}

#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"