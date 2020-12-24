#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Assert.h"

namespace Game
{
	class VertexBufferLayout
	{
		enum class Type : size_t
		{
			Byte = GL_BYTE,
			UnsignedByte = GL_UNSIGNED_BYTE,
			Short = GL_SHORT,
			UnsignedShort = GL_UNSIGNED_SHORT,
			Int = GL_INT,
			UnsignedInt = GL_UNSIGNED_INT,
			Float = GL_FLOAT,
			Double = GL_DOUBLE
		};

	public:
		struct Element
		{
			size_t Type;
			size_t Count;
			bool Normalized;
			size_t Size;
			std::string TypeName;
		};

	private:
		std::vector<Element> m_Elements;
		size_t m_Stride = 0;

	public:

		const std::vector<Element>& GetElements() const { return m_Elements; }
		const size_t GetStride() const { return m_Stride; }

		template <typename Type>
		void Push(const size_t &count, bool normalized = false)
		{
			assert(false && "Unimplement Type");
		}

		void Clear();
	};

#define CREATE_PUSH_METHOD(type, typeT, glType)\
	template<>\
	inline void Game::VertexBufferLayout::Push<type>(const size_t& count, const bool normalized)\
	{\
		m_Elements.emplace_back(Element{static_cast<size_t>(Type::typeT), count, normalized, sizeof(glType), #type});\
		m_Stride += count * sizeof(glType);\
	}

	CREATE_PUSH_METHOD(char, UnsignedByte, GLubyte);
	CREATE_PUSH_METHOD(unsigned char, UnsignedByte, GLubyte);
	CREATE_PUSH_METHOD(short, Short, GLshort);
	CREATE_PUSH_METHOD(unsigned short, UnsignedShort, GLushort);
	CREATE_PUSH_METHOD(int, Int, GLint);
	CREATE_PUSH_METHOD(unsigned int, UnsignedInt, GLuint);
	CREATE_PUSH_METHOD(float, Float, GLfloat);
	CREATE_PUSH_METHOD(double, Double, GLdouble);

	template<>
	inline void VertexBufferLayout::Push<glm::vec2>(const size_t& count, const bool normalized)
	{
		for (size_t i = 0; i < count; ++i)
			Push<float>(2, normalized);
	}

	template<>
	inline void VertexBufferLayout::Push<glm::vec3>(const size_t& count, const bool normalized)
	{
		for (size_t i = 0; i < count; ++i)
			Push<float>(3, normalized);
	}

	template<>
	inline void VertexBufferLayout::Push<glm::vec4>(const size_t& count, const bool normalized)
	{
		for (size_t i = 0; i < count; ++i)
			Push<float>(4, normalized);
	}
}
