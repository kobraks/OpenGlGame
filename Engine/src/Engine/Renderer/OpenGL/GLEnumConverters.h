#pragma once
#include "Engine/Renderer/Resources/TextureEnums.h"
#include "Engine/Renderer/Resources/FrameBufferEnums.h"
#include "Engine/Renderer/Resources/ShaderTypes.h"

#include "Engine/Renderer/Resources/BufferEnums.h"

namespace Engine::OpenGL {
	uint32_t EnumToGLConstant(TextureInternalFormat format);
	uint32_t EnumToGLConstant(FilterMode filter);
	uint32_t EnumToGLConstant(WrapMode wrapMode);
	uint32_t EnumToGLConstant(PixelFormat format);
	uint32_t EnumToGLConstant(PixelDataType type);

	uint32_t EnumToGLConstant(BlitFilter filter);
	uint32_t EnumToGLConstant(BlitMask mask);

	uint32_t EnumToGLConstant(BufferAccess access);
	uint32_t EnumToGLConstant(BufferStorageFlags flags);
	uint32_t EnumToGLConstant(BufferTarget target);
	uint32_t EnumToGLConstant(BufferUsage usage);

	uint32_t EnumToGLConstant(CompareFunction function);

	UniformTypeDesc GLToUniformTypeDesc(uint32_t glEnum);
}