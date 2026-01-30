#pragma once
#include "Engine/Renderer/GraphicEnums.h"
#include "Engine/Renderer/BufferEnums.h"

namespace Engine::Utils {
	std::string_view ToString(WrapMode mode);
	std::string_view ToString(FilterMode mode);
	std::string_view ToString(BlitFilter filter);
	std::string_view ToString(BlitMask mask);
	std::string_view ToString(DataFormat format);
	std::string_view ToString(ImageFormat format);
	std::string_view ToString(DataType type);
	std::string_view ToString(TextureUsage usage);

	std::string_view ToString(BufferUsage usage);
	std::string ToString(BufferStorageFlags flags);
	std::string_view ToString(BufferAccess access);
	std::string_view ToString(BufferStorageMode mode);
	std::string_view ToString(BufferTarget target);

	std::string_view ToString(CompareFunction function);

	std::string_view ToString(UniformKind kind);
	std::string_view ToString(ScalarKind kind);
	std::string_view ToString(TextureDim texture);
	std::string ToString(UniformTypeDesc desc);
}

template <>
struct fmt::formatter<Engine::WrapMode> : fmt::formatter<std::string_view> {
	auto format(Engine::WrapMode v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::FilterMode> : fmt::formatter<std::string_view> {
	auto format(Engine::FilterMode v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitFilter> : fmt::formatter<std::string_view> {
	auto format(Engine::BlitFilter v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BlitMask> : fmt::formatter<std::string_view> {
	auto format(Engine::BlitMask v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataFormat> : fmt::formatter<std::string_view> {
	auto format(Engine::DataFormat v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::ImageFormat> : fmt::formatter<std::string_view> {
	auto format(Engine::ImageFormat v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::DataType> : fmt::formatter<std::string_view> {
	auto format(Engine::DataType v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::TextureUsage> : fmt::formatter<std::string_view> {
	auto format(Engine::TextureUsage v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template<>
struct fmt::formatter<Engine::BufferUsage> : fmt::formatter<std::string_view> {
	auto format(Engine::BufferUsage v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferStorageFlags> : fmt::formatter<std::string> {
	auto format(Engine::BufferStorageFlags v, format_context& ctx) const {
		return fmt::formatter<std::string>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferStorageMode> : fmt::formatter<std::string_view> {
	auto format(Engine::BufferStorageMode v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferAccess> : fmt::formatter<std::string_view> {
	auto format(Engine::BufferAccess v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::BufferTarget> : fmt::formatter<std::string_view> {
	auto format(Engine::BufferTarget v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::CompareFunction> : fmt::formatter<std::string_view> {
	auto format(Engine::CompareFunction v, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(v), ctx);
	}
};

template <>
struct fmt::formatter<Engine::UniformKind> : fmt::formatter<std::string_view> {
	auto format(Engine::UniformKind uniform, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(uniform), ctx);
	}
};

template <>
struct fmt::formatter<Engine::ScalarKind> : fmt::formatter<std::string_view> {
	auto format(Engine::ScalarKind scalar, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(scalar), ctx);
	}
};

template <>
struct fmt::formatter<Engine::TextureDim> : fmt::formatter<std::string_view> {
	auto format(Engine::TextureDim texture, format_context& ctx) const {
		return fmt::formatter<std::string_view>::format(Engine::Utils::ToString(texture), ctx);
	}
};

// g - general
// v - verbose
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