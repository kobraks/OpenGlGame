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

	namespace Utils {
		const char* ToString(BufferStorageFlags flags);
		const char* ToString(BufferAccess access);
		const char* ToString(BufferStorageMode mode);
		const char* ToString(BufferTarget target);

		uint32_t ToGL(BufferAccess access);
		uint32_t ToGL(BufferStorageFlags flags);
		uint32_t ToGL(BufferTarget target);

		constexpr BufferAccess DetermineAccessFromFlags(BufferStorageFlags flags) {
			if (HasFlag(flags, BufferStorageFlags::MapRead) && HasFlag(flags, BufferStorageFlags::MapWrite))
				return BufferAccess::ReadWrite;
			if (HasFlag(flags, BufferStorageFlags::MapRead))
				return BufferAccess::ReadOnly;
			if (HasFlag(flags, BufferStorageFlags::MapWrite))
				return BufferAccess::WriteOnly;

			ENGINE_ASSERT(false, "Invalid mapping flags for determining BufferAccess.");
			throw std::runtime_error("BufferStorageFlags must include MapRead or MapWrite to determine access.");
		}

		inline void AssertAccess(BufferAccess access, bool requiresRead, bool requiresWrite) {
			if (requiresRead && access == BufferAccess::WriteOnly) {
				ENGINE_ASSERT(false, "Attempted to read form buffer mapped as WriteOnly");
				throw std::runtime_error("Attempted to read from buffer mapped as WriteOnly");
			}

			if (requiresWrite && access == BufferAccess::ReadOnly) {
				ENGINE_ASSERT(false, "Attempted to write into buffer mapped as ReadOnly");
				throw std::runtime_error("Attempted to write into buffer mapped as ReadOnly");
			}
		}
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

template <>
struct fmt::formatter<Engine::BufferStorageFlags> : fmt::formatter<const char*> {
	auto format(Engine::BufferStorageFlags v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferStorageMode> : fmt::formatter<const char*> {
	auto format(Engine::BufferStorageMode v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferAccess> : fmt::formatter<const char*> {
	auto format(Engine::BufferAccess v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferTarget> : fmt::formatter<const char*> {
	auto format(Engine::BufferTarget v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};