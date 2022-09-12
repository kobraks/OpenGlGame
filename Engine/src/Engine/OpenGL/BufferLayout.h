#pragma once

#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

#include "Engine/OpenGL/GLEnums.h"

#include <initializer_list>
#include <string>

namespace Game
{
	class ShaderProgram;

	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static constexpr uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch(type)
		{
			case ShaderDataType::Float:
				return 4;
			case ShaderDataType::Float2:
				return 4 * 2;
			case ShaderDataType::Float3:
				return 4 * 3;
			case ShaderDataType::Float4:
				return 4 * 4;
			case ShaderDataType::Mat3:
				return 4 * 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4 * 4;
			case ShaderDataType::Int:
				return 4;
			case ShaderDataType::Int2:
				return 4 * 2;
			case ShaderDataType::Int3:
				return 4 * 3;
			case ShaderDataType::Int4:
				return 4 * 4;
			case ShaderDataType::Bool:
				return 1;

			default:
				ASSERT(false, "Unknown ShaderDataType");
				return 0;
		}
	}

	struct BufferElement
	{
		std::string Name;

		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size       = 0;
		size_t Offset       = 0;
		bool Normalized     = false;

		BufferElement() = default;

		BufferElement(ShaderDataType type, std::string name, bool normalized = false) : Name(std::move(name)),
			Type(type),
			Size(ShaderDataTypeSize(type)),
			Offset(0),
			Normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch(Type)
			{
				case ShaderDataType::Int:
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				case ShaderDataType::Bool:
				case ShaderDataType::Float:
					return 1;
				case ShaderDataType::Int2:
				case ShaderDataType::Float2:
					return 2;
				case ShaderDataType::Int3:
				case ShaderDataType::Float3:
					return 3;
				case ShaderDataType::Int4:
				case ShaderDataType::Float4:
					return 4;
				default: ;
			}
			ASSERT(false, "Unknown ShaderDataType");
			return 0;
		}
	};

	class BufferLayout
	{
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;

	public:
		BufferLayout() = default;

		explicit BufferLayout(std::vector<BufferElement>&& elements) : m_Elements(std::move(elements))
		{
			CalculateOffsetAndStride();
		}

		explicit BufferLayout(std::vector<BufferElement> elements) : m_Elements(std::move(elements))
		{
			CalculateOffsetAndStride();
		}

		BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride();
	};
}
