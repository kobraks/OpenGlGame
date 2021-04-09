#pragma once

#include <string>
#include <initializer_list>

#include "Types.h"
#include "Assert.h"
#include "GLEnums.h"

namespace Game
{
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
			case ShaderDataType::Float: return 4;
			case ShaderDataType::Float2: return 4 * 2;
			case ShaderDataType::Float3: return 4 * 3;
			case ShaderDataType::Float4: return 4 * 4;
			case ShaderDataType::Mat3: return 4 * 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4 * 4;
			case ShaderDataType::Int: return 4;
			case ShaderDataType::Int2: return 4 * 2;
			case ShaderDataType::Int3: return 4 * 3;
			case ShaderDataType::Int4: return 4 * 4;
			case ShaderDataType::Bool: return 1;
		}

		ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size       = 0;
		size_t Offset       = 0;
		bool Normalized     = false;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string &name, bool normalized = false) : Name(name),
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
				case ShaderDataType::Float: return 1;
				case ShaderDataType::Int2:
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Int3:
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Int4:
				case ShaderDataType::Float4: return 4;
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

		BufferLayout(const std::vector<BufferElement> &elements) : m_Elements(elements)
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
		void CalculateOffsetAndStride()
		{
			size_t offset = 0;
			m_Stride      = 0;

			for(auto &element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	};

	class BufferObject;
	
	class BufferContent
	{
		friend class BufferObject;

		const BufferObject& m_Buffer;
		const BufferAccess m_Access;

		void *m_Data = nullptr;

	protected:
		BufferContent(const BufferAccess access, const BufferObject &buffer);

	public:
		~BufferContent();

		const void* Get() const;
		void* Get();

		void Set(const void* data, const size_t size, const size_t offset = 0);

		template<typename Type>
		void Set(const Type& value, const size_t offset = 0)
		{
			return Set(&value, sizeof(Type), offset);
		}

		template<typename Type>
		Type Get(const size_t offset = 0) const
		{
			Type value;
			std::memcpy(&value, static_cast<const char*>(Get()), sizeof(Type));

			return value;
		}

		BufferAccess Access() const { return m_Access; }
	};
	
	class BufferObject
	{
		struct Internals
		{
			uint32_t Id   = 0;
			uint32_t Size = 0;

			BufferUsage Usage = BufferUsage::StaticDraw;
			BufferType Type   = BufferType::Index;

			Internals(BufferType type);
			~Internals();

			void Data(BufferUsage usage, uint32_t size, const void *data = nullptr);
			void SubData(uint32_t size, uint32_t offset, const void *data = nullptr);

			void Bind() const;
			void Unbind() const;
		};

		Pointer<Internals> m_Internals = nullptr;
	protected:
		BufferObject(BufferType type);

		void Data(BufferUsage usage, uint32_t size, const void *data = nullptr)
		{
			m_Internals->Data(usage, size, data);
		}

		void SubData(uint32_t size, uint32_t offset, const void *data = nullptr)
		{
			m_Internals->SubData(size, offset, data);
		}

	public:
		uint32_t Size() const { return m_Internals->Size; }

		void Bind() const { m_Internals->Bind(); }
		void Unbind() const { m_Internals->Unbind(); }

		uint32_t Id() const { return m_Internals->Id; }
		operator uint32_t() const { return Id(); }

		Pointer<BufferContent> GetContent(BufferAccess access = BufferAccess::ReadOnly) const;
	};

	class VertexBuffer : public BufferObject
	{
		struct Internals
		{
			BufferLayout Layout;
		};

		std::shared_ptr<Internals> m_Internals;
	public:
		VertexBuffer(uint32_t size);
		VertexBuffer(const void *vertices, uint32_t size);

		void SetData(const void *data, uint32_t size, uint32_t offset = 0);

		const BufferLayout& GetLayout() const { return m_Internals->Layout; }
		void SetLayout(const BufferLayout &layout) { m_Internals->Layout = layout; }
	};

	class IndexBuffer : public BufferObject
	{
		struct Internals
		{
			uint32_t Count = 0;
		};

		Pointer<Internals> m_Internals;

	public:
		IndexBuffer(const uint32_t *indices, uint32_t count);

		uint32_t Count() const { return m_Internals->Count; }
	};
}
