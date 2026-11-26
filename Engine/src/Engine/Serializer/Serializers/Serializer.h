#pragma once
#include "Engine/Core/Base.h"

#include <type_traits>

namespace Engine {
	class StreamReader;
	class StreamWriter;

	template <class T>
	struct Serializer { using Type = void; };
}