#pragma once
#include "Engine/Renderer/GraphicEnums.h"
#include "Engine/Renderer/BufferEnums.h"

namespace Engine::Utils {
	const char* ToString(WrapMode mode);
	const char* ToString(FilterMode mode);
	const char* ToString(BlitFilter filter);
	const char* ToString(BlitMask mask);
	const char* ToString(DataFormat format);
	const char* ToString(ImageFormat format);
	const char* ToString(DataType type);
	const char* ToString(TextureUsage usage);

	const char* ToString(BufferUsage usage);
	const char* ToString(BufferStorageFlags flags);
	const char* ToString(BufferAccess access);
	const char* ToString(BufferStorageMode mode);
	const char* ToString(BufferTarget target);
}

template <>
struct fmt::formatter<Engine::WrapMode> : fmt::formatter<const char*> {
	auto format(Engine::WrapMode v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::FilterMode> : fmt::formatter<const char*> {
	auto format(Engine::FilterMode v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitFilter> : fmt::formatter<const char*> {
	auto format(Engine::BlitFilter v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitMask> : fmt::formatter<const char*> {
	auto format(Engine::BlitMask v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataFormat> : fmt::formatter<const char*> {
	auto format(Engine::DataFormat v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::ImageFormat> : fmt::formatter<const char*> {
	auto format(Engine::ImageFormat v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataType> : fmt::formatter<const char*> {
	auto format(Engine::DataType v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::TextureUsage> : fmt::formatter<const char*> {
	auto format(Engine::TextureUsage v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

template<>
struct fmt::formatter<Engine::BufferUsage> : fmt::formatter<const char*> {
	auto format(Engine::BufferUsage v, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(v), ctx);
	}
};

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