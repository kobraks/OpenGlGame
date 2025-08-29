#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	bool IsDepthFormat(ImageFormat format);
	bool IsStorageCapable(ImageFormat format);

	bool IsStencilOnlyFormat(ImageFormat format);
	bool IsDepthStencilFormat(ImageFormat format);

	bool IsDepthOnlyFormat(ImageFormat format);
	bool HasDepthAspect(ImageFormat format);
	bool HasStencilAspect(ImageFormat format);

	bool IsIntegerColorFormat(ImageFormat format);
	bool IsSRGBFormat(ImageFormat format);

	bool IsColorFormat(ImageFormat format);

	TextureUsage FormatUsageMapping(ImageFormat format);

	std::pair<DataFormat, DataType> GetDefaultFormatAndType(ImageFormat format);
}