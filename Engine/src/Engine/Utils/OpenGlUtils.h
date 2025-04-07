#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	uint32_t ToGLImageFormat(ImageFormat format);
	uint32_t ToGLFilterMode(FilterMode filter);
	uint32_t ToGLWrapMode(WrapMode wrapMode);
	uint32_t ToGLDataFormat(DataFormat format);
	uint32_t ToGLDataType(DataType type);

	uint32_t ToGLFilter(BlitFilter filter);
	uint32_t ToGLMask(BlitMask mask);

	std::pair<DataFormat, DataType> GetDefaultFormatAndType(ImageFormat format);
}