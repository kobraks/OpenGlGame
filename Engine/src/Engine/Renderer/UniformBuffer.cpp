#include "pch.h"
#include "Engine/Renderer/UniformBuffer.h"

#include <glad/glad.h>

#include "Engine/Core/BufferView.h"

namespace Engine {
	namespace Utils {
		static inline void WriteVec4Padded(BufferObject& buffer, const glm::vec4& value, uint32_t offset) {
			buffer.Write(BufferView(&value, sizeof(glm::vec4)), offset);
		}

		template <typename MakeColFunc>
		static inline void WriteMatrixColumnsStd140(BufferObject& buffer, uint32_t columnCount, uint32_t startOffset, const MakeColFunc& makeCol) {
			uint32_t offset = startOffset;

			for (uint32_t i = 0; i < columnCount; ++i) {
				const glm::vec4 col = makeCol(i);
				WriteVec4Padded(buffer, col, offset);
				offset += 16; // std140 column stride is 16 bytes
			}
		}
	}

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, BufferStorageFlags flags) {
		Ref<UniformBuffer> buffer = Ref<UniformBuffer>(new UniformBuffer());
		buffer->Allocate(size, flags);

		return buffer;
	}

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, BufferUsage usageHint) {
		Ref<UniformBuffer> buffer = Ref<UniformBuffer>(new UniformBuffer());
		buffer->Allocate(size, usageHint);

		return buffer;
	}

	uint32_t UniformBuffer::MaxBindings() {
		static uint32_t maxBindings = 0;
		static bool initialized = false;

		if (!initialized) {
			GLint v = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &v);
			maxBindings = static_cast<uint32_t>(v);

			initialized = true;
		}

		return maxBindings;
	}

	uint32_t UniformBuffer::AlignToUBOOffset(uint32_t size) {
		static uint32_t alignment = 0;
		static bool initialized = false;

		if (!initialized) {
			GLint v = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &v);
			alignment = static_cast<uint32_t>(v);

			initialized = true;
		}

		const uint32_t rem = size % alignment;
		return rem ? (size + (alignment - rem)) : size;
	}

	void UniformBuffer::BindRange(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const {
		const uint32_t aligned = AlignToUBOOffset(1);
		ENGINE_ASSERT((aligned % offset) == 0)
		BufferObject::BindRange(bindingPoint, size, offset);
	}

	void UniformBuffer::BindRangeAligned(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const {
		const uint32_t aligned = AlignToUBOOffset(offset);
		BufferObject::BindRange(bindingPoint, size, aligned);
	}

	void UniformBuffer::Write(float value, uint32_t offset) {
		BufferObject::Write(BufferView(&value, sizeof(float)), offset);
	}

	void UniformBuffer::Write(int32_t value, uint32_t offset) {
		BufferObject::Write(BufferView(&value, sizeof(int32_t)), offset);
	}

	void UniformBuffer::Write(uint32_t value, uint32_t offset) {
		BufferObject::Write(BufferView(&value, sizeof(uint32_t)), offset);
	}

	void UniformBuffer::Write(bool value, uint32_t offset) {
		const int32_t i = value ? true : false;
		BufferObject::Write(BufferView(&i, sizeof(int32_t)), offset);
	}

	void UniformBuffer::Write(const glm::vec2& value, uint32_t offset) {
		BufferObject::Write(BufferView(&value, sizeof(glm::vec2)), offset);
	}

	void UniformBuffer::Write(const glm::vec3& value, uint32_t offset) {
		const glm::vec4 padded(value, 0.f);
		BufferObject::Write(BufferView(&padded, sizeof(glm::vec4)), offset);
	}

	void UniformBuffer::Write(const glm::vec4& value, uint32_t offset) {
		BufferObject::Write(BufferView(&value, sizeof(glm::vec4)), offset);
	}

	void UniformBuffer::Write(const glm::mat2x2& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 2, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f, 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat2x3& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 2, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat2x4& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 2, offset, [&](uint32_t colIndex) {
			return value[static_cast<uint32_t>(colIndex)];
			});
	}

	void UniformBuffer::Write(const glm::mat3x2& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 3, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f, 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat3x3& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 3, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat3x4& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 3, offset, [&](uint32_t colIndex) {
			return value[static_cast<uint32_t>(colIndex)];
			});
	}

	void UniformBuffer::Write(const glm::mat4x2& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 4, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f, 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat4x3& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 4, offset, [&](uint32_t colIndex) {
			return glm::vec4(value[static_cast<uint32_t>(colIndex)], 0.f);
			});
	}

	void UniformBuffer::Write(const glm::mat4x4& value, uint32_t offset) {
		Utils::WriteMatrixColumnsStd140(*this, 4, offset, [&](uint32_t colIndex) {
			return value[static_cast<uint32_t>(colIndex)];
			});
	}
}
