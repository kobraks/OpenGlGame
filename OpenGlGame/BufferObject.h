#pragma once
#include <unordered_map>

#include "GLEnums.h"

#include "Types.h"

namespace Game
{
	constexpr std::string_view GetBufferUsageAsString(const BufferUsage &usage)
	{
		switch(usage)
		{
			case BufferUsage::StreamDraw: return "stream draw";
			case BufferUsage::StreamRead: return "stream read";
			case BufferUsage::StreamCopy: return "stream copy";

			case BufferUsage::StaticDraw: return "static draw";
			case BufferUsage::StaticRead: return "static read";
			case BufferUsage::StaticCopy: return "static copy";

			case BufferUsage::DynamicDraw: return "dynamic draw";
			case BufferUsage::DynamicRead: return "dynamic read";
			case BufferUsage::DynamicCopy: return "dynamic copy";
		}

		return "";
	}

	constexpr std::string_view GetBufferTypeAsString(const BufferType &type)
	{
		switch(type)
		{
			case BufferType::Index: return "index";
			case BufferType::Vertex: return "vertex";
			case BufferType::Uniform: return "uniform";
		}

		return "";
	}

	class BufferContent;

	class BufferObject
	{
	public:
		using IdType = uint32_t;

	private:
		class Internals
		{
			bool m_ProvidedId = false;
		public:
			IdType Id = 0;
			
			size_t Size = 0;
			bool Changed = true;
			
			BufferUsage Usage;
			BufferType Type;

			Internals();
			explicit Internals(IdType id);
			~Internals();
		};

		Pointer<Internals> m_Internals = nullptr;

		inline static std::unordered_map<BufferType, BufferObject> m_Buffers = std::unordered_map<
			BufferType, BufferObject>();
		inline static bool m_Initialized = false;

		BufferObject(BufferType type, bool def);
	protected:
		explicit BufferObject(BufferType type, BufferUsage usage = BufferUsage::StaticDraw);

		virtual void SendValues() const {}

		void SendData(const void *data, size_t size) const;
		void SendSubData(const void *data, size_t size, size_t offset) const;

		template <class Type>
		void SendData(const void *data, size_t count) const;

		template <class Type>
		void SendSubData(const void *data, size_t count, size_t offset) const;
	public:
		virtual ~BufferObject() = default;

		void Allocate(size_t size);
		void ReAlloc(size_t size);

		operator IdType() const;
		IdType Id() const;

		BufferUsage Usage() const;
		BufferType Type() const;

		size_t Size() const;

		virtual void Bind() const;
		virtual void UnBind() const;

		bool IsChanged() const;
		void MarkAsChanged() const;

		static BufferObject* GetDefault(BufferType type);

		void Flush() const;

		void GetData(void *data, size_t offset, size_t length) const;

		virtual Pointer<BufferObject> Clone() const;

		[[nodiscard]] Pointer<BufferContent> GetContent(const BufferAccess access) const;
	private:
		static void Init();
	};

	class BufferContent
	{
		friend class BufferObject;

		const BufferObject& m_Buffer;
		const BufferAccess m_Access;
		
		void* m_Data = nullptr;
	
	protected:
		BufferContent(const BufferAccess access, const BufferObject &buffer);

	public:
		~BufferContent();

		const void* Get() const;
		void* Get();
		void Set(const void* data, const size_t size, const size_t offset = 0);

		template<typename Type>
		void Set(const Type& value, const size_t offset)
		{
			return Set(&value, sizeof(Type), offset);
		}

		template<typename Type>
		Type Get(const size_t offset) const
		{
			Type value;
			std::memcpy(&value, static_cast<const char*>(Get()) + offset, sizeof(Type));
			return value;
		}
		
		BufferAccess Access() const { return m_Access; }
		const BufferObject& GetBuffer() const { return m_Buffer; }
	};


	template <class Type>
	void BufferObject::SendData(const void *data, const size_t count) const
	{
		SendData(data, count * sizeof(Type));
	}

	template <class Type>
	void BufferObject::SendSubData(const void *data, const size_t count, const size_t offset) const
	{
		SendSubData(data, count * sizeof(Type), offset * sizeof(Type));
	}
}
