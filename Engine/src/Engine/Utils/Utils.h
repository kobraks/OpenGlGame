#pragma once
#include "Engine/Core/Base.h"

#include <string>

namespace Game
{
	template <typename... Types>
	struct TypeListLength
	{
		constexpr static size_t Length = sizeof ...(Types);
	};

	template <typename... Types>
	constexpr size_t TypeListLengthV =TypeListLength<Types...>::Length; 
	
	std::string& TrimLeft(std::string &string);
	std::string& TrimRight(std::string &string);
	std::string& Trim(std::string &string);

	std::string TrimLeftCopy(std::string string);
	std::string TrimRightCopy(std::string string);
	std::string TrimCopy(std::string string);

	std::string& ToLower(std::string &string);
	std::string& ToUpper(std::string &string);

	std::string ToUpperCopy(std::string string);
	std::string ToLowerCopy(std::string string);
}
