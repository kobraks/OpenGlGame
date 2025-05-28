#pragma once
#include "Engine/Renderer/BufferObject.h"

namespace Engine {
	class ShaderStorageBuffer : public BufferObject {
	public:
		using BufferObject::BufferObject;

		static Ref<ShaderStorageBuffer> Create(uint32_t size, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<ShaderStorageBuffer> Create(uint32_t size, BufferUsage usageHint);

	protected:
		ShaderStorageBuffer();
	};

	template<typename T>
	class TypedShaderStorageBuffer : public ShaderStorageBuffer {
	public:
		void SetData(std::span<const T> data) {
			GetContent(BufferAccess::WriteOnly)->Set(data);
		}

		std::span<T> GetData() {
			return GetContent(BufferAccess::ReadOnly)->template AsSpan<T>();
		}
	};

	using ShaderStorageBufferRef = Ref<ShaderStorageBuffer>;
	template<typename T>
	using TypedShaderStorageBufferRef = Ref<TypedShaderStorageBuffer<T>>;
}