#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Utils/CoreUtility.h"

namespace Engine {
	enum class BufferStorageFlags : uint32_t {
		None = 0,
		Dynamic = BIT(0),
		MapRead = BIT(1),
		MapWrite = BIT(2),
		MapPersistent = BIT(3),
		MapCoherent = BIT(4),
		ClientStorage = BIT(5)
	};

	enum class BufferAccess : uint32_t {
		ReadOnly = BIT(0),
		WriteOnly = BIT(1),
		ReadWrite = BIT(1) | BIT(0)
	};

	enum class BufferStorageMode {
		Mutable,
		Immutable,
	};

	enum class BufferTarget {
		None = 0,
		Array,
		ElementArray,
		Uniform,
		ShaderStorage,
		CopyRead,
		CopyWrite,
		PixelPack,
		PixelUnpack,
		DrawIndirect,
		DispatchIndirect,
		TransformFeedback,
		AtomicCounter
	};

	enum class BufferUsage {
		StaticDraw,
		StaticRead,
		StaticCopy,

		DynamicDraw,
		DynamicRead,
		DynamicCopy,

		StreamDraw,
		StreamRead,
		StreamCopy
	};

	inline constexpr BufferAccess operator^(BufferAccess a, BufferAccess b) {
		return static_cast<BufferAccess>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
	}

	inline constexpr BufferAccess operator^=(BufferAccess& a, BufferAccess b) {
		return a = (a ^ b);
	}

	inline constexpr BufferAccess operator|(BufferAccess a, BufferAccess b) {
		return static_cast<BufferAccess>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline constexpr BufferAccess operator|=(BufferAccess& a, BufferAccess b) {
		return a = (a | b);
	}

	inline constexpr BufferAccess operator & (BufferAccess a, BufferAccess b) {
		return static_cast<BufferAccess>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	inline constexpr BufferAccess& operator &= (BufferAccess& a, BufferAccess b) {
		return a = a & b;
	}

	inline constexpr BufferStorageFlags operator^(BufferStorageFlags a, BufferStorageFlags b) {
		return static_cast<BufferStorageFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
	}

	inline constexpr BufferStorageFlags operator^=(BufferStorageFlags& a, BufferStorageFlags b) {
		return a = (a ^ b);
	}

	inline constexpr BufferStorageFlags operator|(BufferStorageFlags a, BufferStorageFlags b) {
		return static_cast<BufferStorageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline constexpr BufferStorageFlags operator|=(BufferStorageFlags& a, BufferStorageFlags b) {
		return a = (a | b);
	}

	inline constexpr BufferStorageFlags operator & (BufferStorageFlags a, BufferStorageFlags b) {
		return static_cast<BufferStorageFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	inline constexpr BufferStorageFlags& operator &= (BufferStorageFlags& a, BufferStorageFlags b) {
		return a = a & b;
	}
}

