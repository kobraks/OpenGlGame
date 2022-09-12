#pragma once
#include "Engine/OpenGL/BufferObject.h"

namespace Game
{
	class ShaderProgram;

	class UniformBuffer: public BufferObject
	{
		struct Internals
		{
			char *Data;
			BufferUsage Usage;

			Internals(uint32_t size, BufferUsage usage);
			~Internals();
		};

		Pointer<Internals> m_Internals;

	public:
		UniformBuffer(const ShaderProgram &program, const std::string &name, BufferUsage usage);
		UniformBuffer(uint32_t size, BufferUsage usage);
		UniformBuffer(const void *data, uint32_t size, BufferUsage usage);

		BufferUsage Usage() const { return m_Internals->Usage; }
		void Usage(BufferUsage usage);

		void* Get() { return m_Internals->Data; }
		const void* Get() const { return m_Internals->Data; }

		void Set(const void *data, size_t size, size_t offset = 0);

		void Set(glm::vec2 &value, size_t offset = 0);
		void Set(glm::vec3 &value, size_t offset = 0);
		void Set(glm::vec4 &value, size_t offset = 0);
		void Set(glm::mat3 &value, size_t offset = 0);
		void Set(glm::mat4 &value, size_t offset = 0);

	public:
		template <typename Type>
		Type Get(size_t offset = 0) const
		{
			ASSERT(offset + sizeof(Type) <= Size(), "Out of range");
			if(offset + sizeof(Type) > Size())
				throw std::out_of_range("Out of range");

			Type value;
			std::memcpy(&value, m_Internals->Data + offset, sizeof(Type));
			return value;
		}

		template <typename Type>
		void Set(const Type &value, size_t offset = 0)
		{
			Set(&value, sizeof(Type), offset);
		}
	};
}
