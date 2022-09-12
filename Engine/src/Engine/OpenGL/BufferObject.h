#pragma once
#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

#include "Engine/OpenGL/GLEnums.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

namespace Game
{
	class BufferContent;

	class BufferObject
	{
	public:
		friend class BufferContent;

		using IDType = uint32_t;

	private:
		struct Internals
		{
			IDType Buffer = 0;
			size_t Size = 0;

			BufferUsage Usage = BufferUsage::StaticDraw;
			BufferType Type   = BufferType::Index;

			OpenGlFunctions Functions;

			std::weak_ptr<BufferContent> Map;

			explicit Internals(BufferType type);
			~Internals();

			void Data(BufferUsage usage, size_t size, const void *data = nullptr);
			void SubData(size_t size, size_t offset, const void *data = nullptr);

			void Bind() const;
			void UnBind() const;

			void* MapBuffer(BufferAccess access) const;
			void UnMapBuffer() const;
		};

		Pointer<Internals> m_Internals;

	protected:
		OpenGlFunctions& GetFunctions() const;
		BufferObject(BufferType type);

		void Data(BufferUsage usage, size_t size, const void *data = nullptr)
		{
			m_Internals->Data(usage, size, data);
		}

		void SubData(size_t size, size_t offset, const void *data = nullptr)
		{
			m_Internals->SubData(size, offset, data);
		}

	public:
		size_t Size() const { return m_Internals->Size; }

		void Bind() const { m_Internals->Bind(); }
		void UnBind() const { return m_Internals->UnBind(); }

		BufferType Type() const { return m_Internals->Type; }

		IDType ID() const { return m_Internals->Buffer; }
		operator IDType() const { return m_Internals->Buffer; }

		Pointer<BufferContent> GetContent(BufferAccess access = BufferAccess::ReadOnly) const;

		bool IsMaped() const { return !m_Internals->Map.expired(); }
	private:
		void* MapBuffer(BufferAccess access) const;
		void UnMapBuffer() const;
	};

	class BufferContent
	{
		friend class BufferObject;

		const BufferObject *m_Buffer;
		const BufferAccess m_Access;

		void *m_Data = nullptr;

	protected:
		BufferContent(BufferAccess access, const BufferObject* object);

		void Invalidate();
	public:
		BufferContent() = delete;
		BufferContent(const BufferContent&) = delete;
		BufferContent(BufferContent&&) = delete;

		BufferContent& operator=(const BufferContent&) = delete;
		BufferContent& operator=(BufferContent&&) = delete;

		~BufferContent();

		BufferObject* Parent() { return const_cast<BufferObject *>(m_Buffer); }
		const BufferObject* Parent() const { return m_Buffer; }

		BufferAccess Access() const { return m_Access; }

		const void* Get() const;
		void* Get();

		void Set(const void* data, size_t size, size_t offset = 0);

	public:
		template <typename Type>
		void Set(const Type &value, const size_t offset = 0)
		{
			return Set(&value, sizeof(Type), offset);
		}

		template <typename Type>
		Type Get(const size_t offset = 0)
		{
			const void *data = Get();
			ASSERT(data);
			if (!data)
			{
				GL_LOG_ERROR("Null data");
				return {};
			}

			ASSERT(offset + sizeof(Type) <= m_Buffer->Size(), "Out of Range");

			if (offset + sizeof(Type) > m_Buffer->Size())
				throw std::out_of_range("Out of range");

			Type value;
			std::memcpy(&value, static_cast<const char*>(data) + offset, sizeof(Type));

			return value;
		}
	};
}