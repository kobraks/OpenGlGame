#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "Vector2.h"

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
