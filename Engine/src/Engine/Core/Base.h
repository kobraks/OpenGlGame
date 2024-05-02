#pragma once

#include "Engine/Core/Config.h"

#include <memory>

#ifdef GAME_DEFINE_DISCRETE_GPU_PREFERENCE
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

#define GAME_EXPAND_MACRO(x) x
#define GAME_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto &&... args) ->decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Ref<T> MakeRef(Args &&...  args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ... Args>
	constexpr Scope<T> MakeScope(Args &&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}

#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"