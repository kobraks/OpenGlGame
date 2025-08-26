#pragma once

#include "Engine/Renderer/BufferObject.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat4x3.hpp"
#include "glm/mat4x4.hpp"

namespace Engine {
	class UniformBuffer : public BufferObject {
	public:
		static Ref<UniformBuffer> Create(uint32_t size, BufferStorageFlags flags);
		static Ref<UniformBuffer> Create(uint32_t size, BufferUsage usageHint);

		static uint32_t MaxBindings();
		static uint32_t AlignToUBOOffset(uint32_t size); //Align a size to the GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT requirement

		void BindRange(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const override;
		void BindRangeAligned(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const;

		void Write(float value, uint32_t offset = 0);
		void Write(int32_t value, uint32_t offset = 0);
		void Write(uint32_t value, uint32_t offset = 0);
		void Write(bool value, uint32_t offset = 0);

		void Write(const glm::vec2& value, uint32_t offset = 0);
		void Write(const glm::vec3& value, uint32_t offset = 0);
		void Write(const glm::vec4& value, uint32_t offset = 0);

		void Write(const glm::mat2x2& value, uint32_t offset = 0);
		void Write(const glm::mat2x3& value, uint32_t offset = 0);
		void Write(const glm::mat2x4& value, uint32_t offset = 0);

		void Write(const glm::mat3x2& value, uint32_t offset = 0);
		void Write(const glm::mat3x3& value, uint32_t offset = 0);
		void Write(const glm::mat3x4& value, uint32_t offset = 0);

		void Write(const glm::mat4x2& value, uint32_t offset = 0);
		void Write(const glm::mat4x3& value, uint32_t offset = 0);
		void Write(const glm::mat4x4& value, uint32_t offset = 0);
	private:
		UniformBuffer() : BufferObject(BufferTarget::Uniform) {}
	};
}