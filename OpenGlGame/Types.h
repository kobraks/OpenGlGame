#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace Game
{
	template <typename Type>
	using Pointer = std::shared_ptr<Type>;

	template <typename Type, typename ... Args>
	constexpr Pointer<Type> CreatePointer(Args && ... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}

	template <typename Type, typename ...Args>
	constexpr Pointer<Type> MakePointer(Args&&... args)
	{
		return std::make_shared<Type>(std::forward<Args>(args)...);
	}
}