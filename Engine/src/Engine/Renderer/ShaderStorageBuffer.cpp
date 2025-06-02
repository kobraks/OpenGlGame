#include "pch.h"
#include "ShaderStorageBuffer.h"

#include <glad/glad.h>

namespace Engine {
	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t size,
	                                                     BufferStorageFlags flags) {
		auto buffer = Ref<ShaderStorageBuffer>(new ShaderStorageBuffer());
		buffer->Allocate(nullptr, size, flags);
		return buffer;
	}

	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t size, BufferUsage usageHint) {
		auto buffer = Ref<ShaderStorageBuffer>(new ShaderStorageBuffer());
		buffer->Allocate(nullptr, size, usageHint);
		return buffer;
	}

	ShaderStorageBuffer::ShaderStorageBuffer() : BufferObject(BufferTarget::ShaderStorage) {
	}
}
