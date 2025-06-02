#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	bool IsDepthFormat(ImageFormat format);
	bool IsStorageCapable(ImageFormat format);

	bool IsStencilOnlyFormat(ImageFormat format);
	bool IsDepthStencilFormat(ImageFormat format);

	bool IsColorFormat(ImageFormat format);

	TextureUsage FormatUsageMapping(ImageFormat format);

	std::pair<DataFormat, DataType> GetDefaultFormatAndType(ImageFormat format);
}