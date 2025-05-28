#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	uint32_t ToGL(ImageFormat format);
	uint32_t ToGL(FilterMode filter);
	uint32_t ToGL(WrapMode wrapMode);
	uint32_t ToGL(DataFormat format);
	uint32_t ToGL(DataType type);

	uint32_t ToGL(BlitFilter filter);
	uint32_t ToGL(BlitMask mask);

	uint32_t ToGL(BufferUsage usage);

	std::pair<DataFormat, DataType> GetDefaultFormatAndType(ImageFormat format);
}