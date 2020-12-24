#pragma once
#include <unordered_map>

#include <glad/glad.h>
#include "Types.h"

namespace Game
{
	enum class BufferUsage : uint32_t
	{
		StreamDraw = GL_STREAM_DRAW,
		StreamRead = GL_STREAM_READ,
		StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW,
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW,
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY
	};

	enum class BufferType : uint32_t
	{
		Index = GL_ELEMENT_ARRAY_BUFFER,
		Vertex = GL_ARRAY_BUFFER,
		Uniform = GL_UNIFORM_BUFFER
	};

	constexpr std::string_view GetBufferUsageAsString(const BufferUsage &usage)
	{
		switch(usage)
		{
			case BufferUsage::StreamDraw:
				return "stream draw";
			case BufferUsage::StreamRead:
				return "stream read";
			case BufferUsage::StreamCopy:
				return "stream copy";

			case BufferUsage::StaticDraw:
				return "static draw";
			case BufferUsage::StaticRead:
				return "static read";
			case BufferUsage::StaticCopy:
				return "static copy";

			case BufferUsage::DynamicDraw:
				return "dynamic draw";
			case BufferUsage::DynamicRead:
				return "dynamic read";
			case BufferUsage::DynamicCopy:
				return "dynamic copy";
		}

		return "";
	}

	constexpr std::string_view GetBufferTypeAsString(const BufferType &type)
	{
		switch(type)
		{
			case BufferType::Index:
				return "index";
			case BufferType::Vertex:
				return "vertex";
			case BufferType::Uniform:
				return "uniform";
		}

		return "";
	}

	class BufferObject
	{
	public:
		using IdType = uint32_t;

	private:
		Pointer<IdType> m_Buffer;

		BufferUsage m_Usage;
		BufferType m_Type;

		mutable size_t m_Size = 0;

		mutable bool m_Changed = true;

		inline static std::unordered_map<BufferType, BufferObject> m_Buffers = std::unordered_map<
			BufferType, BufferObject>();
		inline static bool m_Initialized = false;

		BufferObject(BufferType type, bool def);
	protected:
		explicit BufferObject(BufferType type, BufferUsage usage = BufferUsage::StaticDraw);

		virtual void SendValues() const  {}

		void SendData(const void* data, const size_t size) const;
		void SendSubData(const void* data, const size_t size, const size_t offset) const;

		template<class Type>
		void SendData(const void* data, const size_t count) const;

		template<class Type>
		void SendSubData(const void* data, const size_t count, const size_t offset) const;
	public:
		virtual ~BufferObject() = default;

		void Allocate(const size_t size);
		
		operator IdType() const { return *m_Buffer; }
		IdType ID() const { return *m_Buffer; }

		BufferUsage Usage() const { return m_Usage; }
		BufferType Type() const { return m_Type; }

		size_t Size() const { return m_Size; }
		
		void Bind() const;
		void UnBind() const;

		bool IsChanged() const { return m_Changed; }
		void MarkAsChanged() const { m_Changed = true; }

		static BufferObject* GetDefault(BufferType type);

		void Flush() const;

		void GetData(void* data, const size_t offset, const size_t length) const;

		virtual Pointer<BufferObject> Clone() const;
	private:
		static void Init();
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
