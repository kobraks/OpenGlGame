#pragma once
#include "Engine/Renderer/BufferObject.h"

namespace Engine {
	class ShaderStorageBuffer : public BufferObject {
	public:
		using BufferObject::BufferObject;

		inline void BindToArrayIndex(uint32_t bindPoint, uint32_t index, uint32_t size, uint32_t offset = 0) const {
			BindRange(bindPoint + index, size, offset);
		}

		static Ref<ShaderStorageBuffer> Create(uint32_t size, BufferStorageFlags flags = BufferStorageFlags::None);
		static Ref<ShaderStorageBuffer> Create(uint32_t size, BufferUsage usageHint);

	protected:
		ShaderStorageBuffer();
	};

	template<typename T>
	class TypedShaderStorageBuffer : public ShaderStorageBuffer {
	public:
		void SetData(std::span<const T> data, uint32_t offset = 0) {
			Upload({ data.data(), data.size_bytes()}, offset);
		}

		std::span<T> GetData() {
			return GetContent(BufferAccess::ReadOnly)->template AsSpan<T>();
		}
	};

	using ShaderStorageBufferRef = Ref<ShaderStorageBuffer>;
	template<typename T>
	using TypedShaderStorageBufferRef = Ref<TypedShaderStorageBuffer<T>>;
}