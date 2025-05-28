#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Renderer/GraphicEnums.h"

namespace Engine {
	struct BufferUsageIntent {
		bool cpuWrite = true;
		bool cpuReads = false;
		bool gpuWrite = false;
		bool gpuReads = true;
		bool isDynamic = false;
		bool isStreaming = false;
	};

	BufferUsage ChooseUsage(const BufferUsageIntent& intent);

	namespace Utils {
		const char* ToString(WrapMode mode);
		const char* ToString(FilterMode mode);
		const char* ToString(BlitFilter filter);
		const char* ToString(BlitMask mask);
		const char* ToString(DataFormat format);
		const char* ToString(ImageFormat format);
		const char* ToString(DataType type);
		const char* ToString(TextureUsage usage);
		const char* ToString(BufferUsage usage);

		bool IsDepthFormat(ImageFormat format);
		bool IsStorageCapable(ImageFormat format);

		bool IsStencilOnlyFormat(ImageFormat format);
		bool IsDepthStencilFormat(ImageFormat format);

		bool IsColorFormat(ImageFormat format);

		TextureUsage GetUsageFromFormat(ImageFormat format);
	}
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