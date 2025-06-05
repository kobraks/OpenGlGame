#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	struct NonCopyable {
		NonCopyable() = default;
		~NonCopyable() = default;

		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
	};

	struct NonMoveable {
		NonMoveable() = default;
		~NonMoveable() = default;

		NonMoveable(NonMoveable&&) noexcept = delete;
		NonMoveable& operator=(NonMoveable&&) = delete;
	};

	struct NonCopyableNonMoveable : NonCopyable, NonMoveable {};
}