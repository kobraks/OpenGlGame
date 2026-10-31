#pragma once
#include "Engine/Renderer/Resources/TextureEnums.h"

namespace Engine::Utils {
	bool IsDepthFormat(TextureInternalFormat format);
	bool IsStorageCapable(TextureInternalFormat format);

	bool IsStencilOnlyFormat(TextureInternalFormat format);
	bool IsDepthStencilFormat(TextureInternalFormat format);

	bool IsDepthOnlyFormat(TextureInternalFormat format);
	bool HasDepthAspect(TextureInternalFormat format);
	bool HasStencilAspect(TextureInternalFormat format);

	bool IsIntegerColorFormat(TextureInternalFormat format);
	bool IsSRGBFormat(TextureInternalFormat format);

	bool IsColorFormat(TextureInternalFormat format);

	TextureUsage FormatUsageMapping(TextureInternalFormat format);

	std::pair<PixelFormat, PixelDataType> GetDefaultFormatAndType(TextureInternalFormat format);
}