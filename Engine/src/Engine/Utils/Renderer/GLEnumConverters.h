#pragma once
#include "Engine/Renderer/GraphicEnums.h"
#include "Engine/Renderer/BufferEnums.h"

namespace Engine::Utils {
	uint32_t EnumToGLConstant(ImageFormat format);
	uint32_t EnumToGLConstant(FilterMode filter);
	uint32_t EnumToGLConstant(WrapMode wrapMode);
	uint32_t EnumToGLConstant(DataFormat format);
	uint32_t EnumToGLConstant(DataType type);

	uint32_t EnumToGLConstant(BlitFilter filter);
	uint32_t EnumToGLConstant(BlitMask mask);

	uint32_t EnumToGLConstant(BufferAccess access);
	uint32_t EnumToGLConstant(BufferStorageFlags flags);
	uint32_t EnumToGLConstant(BufferTarget target);
	uint32_t EnumToGLConstant(BufferUsage usage);

	uint32_t EnumToGLConstant(CompareFunction function);

	UniformTypeDesc GLToUniformTypeDesc(uint32_t glEnum);
}