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

	const char* ToString(UniformKind kind);
	const char* ToString(ScalarKind kind);
	const char* ToString(TextureDim texture);
	std::string ToString(UniformTypeDesc desc);
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

template <>
struct fmt::formatter<Engine::UniformKind> : fmt::formatter<const char*> {
	auto format(Engine::UniformKind uniform, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(uniform), ctx);
	}
};

template <>
struct fmt::formatter<Engine::ScalarKind> : fmt::formatter<const char*> {
	auto format(Engine::ScalarKind scalar, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(scalar), ctx);
	}
};

template <>
struct fmt::formatter<Engine::TextureDim> : fmt::formatter<const char*> {
	auto format(Engine::TextureDim texture, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(texture), ctx);
	}
};

template <>
struct fmt::formatter<Engine::UniformTypeDesc>{
	char Presentation = 'g';

	constexpr auto parse(fmt::format_parse_context& ctx) {
		auto it = ctx.begin();
		auto end = ctx.end();

		if (it != end && *it != '}') {
			Presentation = *it++;
			if (it != end && *it != '}') {
				throw fmt::format_error("Invalid format specifier for UniformTypeDesc");
			}
		}

		return it;
	}

	auto format(Engine::UniformTypeDesc desc, format_context& ctx) const {
		if (Presentation == 'v') {
			return fmt::format_to(ctx.out(), "UniformTypeDesc {{ Kind: {}, Scalar: {}, Rows: {}, Cols: {}, Dim: {}, Shadow: {} }}", desc.Kind, desc.Scalar, desc.Rows, desc.Cols, desc.Dim, desc.Shadow);
		}

		return fmt::format_to(ctx.out(), "{}", Engine::Utils::ToString(desc));
	}
};