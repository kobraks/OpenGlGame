#pragma once
#include "Engine/Core/Base.h"

#include <vector>

namespace Engine {
	enum class ShaderDataType {
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool
	};

	static constexpr uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 4 * 2;
		case ShaderDataType::Float3:
			return 4 * 3;
		case ShaderDataType::Float4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 4 * 2;
		case ShaderDataType::Int3:
			return 4 * 3;
		case ShaderDataType::Int4:
			return 4 * 4;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Bool:
			return 4;
		}

		ENGINE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size = 0;
		uint64_t Offset = 0;
		bool Normalized = false;

		bool UseInstancing = false;
		uint32_t Divisor = 0;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const {
			switch (Type) {
			case ShaderDataType::Int:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			case ShaderDataType::Bool:
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Int2:
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Int3:
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Int4:
			case ShaderDataType::Float4: return 4;
			}

			ENGINE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout {
	public:
		BufferLayout() = default;
		BufferLayout(const std::vector<BufferElement>& elements) : m_Elements(elements) {
			CalculateOffsetAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		const BufferElement& GetElement(uint32_t index) const {
			ENGINE_ASSERT(index < m_Elements.size(), "Index out of bounds!");
			if (index >= m_Elements.size())
				throw std::out_of_range("Index out of bounds!");

			return m_Elements[index];
		}

		std::size_t GetElementCount() const { return m_Elements.size(); }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		std::vector<BufferElement>::reverse_iterator rbegin() { return m_Elements.rbegin(); }
		std::vector<BufferElement>::reverse_iterator rend() { return m_Elements.rend(); }

		std::vector<BufferElement>::const_reverse_iterator rbegin() const { return m_Elements.rbegin(); }
		std::vector<BufferElement>::const_reverse_iterator rend() const { return m_Elements.rend(); }


	protected:
		void CalculateOffsetAndStride() {
			uint64_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Elements) {
				element.Offset = offset;

				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}