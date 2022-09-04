#pragma once

#include <memory>

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

/*
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Color.h"

#define IM_VEC2_CLASS_EXTRA                                             \
    template <typename T>                                               \
    ImVec2(const Game::Vector2<T>& v) : x(v.X), y(v.Y) {}                                                                   \
                                                                        \
    template <typename T>                                               \
    operator Game::Vector2<T>() const {                                   \
        return Game::Vector2<T>(x, y);                                    \
    }  \
    \
    template <typename T> \
	ImVec2(const glm::vec<2, T> v) : x(v.x), y(v.y) {}\
	\
	template <typename T>\
	operator glm::vec<2, T>() const {\
		return glm::vec<2, T>(x, y);\
	}

#define IM_VEC4_CLASS_EXTRA                                             \
    ImVec4(const Game::Color & c)                                         \
        : x(c.R / 255.f), y(c.G / 255.f), z(c.B / 255.f), w(c.A / 255.f)\
    {}                                                                  \
    operator Game::Color() const {                                      \
        return Game::Color(x, y, z, w);                                 \
    }
    */

