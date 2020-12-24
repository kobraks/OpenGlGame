#include "pch.h"
#include "RenderBuffer.h"

namespace Game
{
	static void DestroyRenderBuffer(RenderBuffer::IdType* id)
	{
		glDeleteRenderbuffers(1, id);
		delete id;
	}

	static auto CreateRenderBuffer()
	{
		auto renderBuffer = Pointer<RenderBuffer::IdType>(new RenderBuffer::IdType{}, DestroyRenderBuffer);
		glGenRenderbuffers(1, &*renderBuffer);
		
		return renderBuffer;
	}

	RenderBuffer::RenderBuffer(IdType id)
	{
		m_RenderBuffer = MakePointer<IdType>(id);
	}

	RenderBuffer::RenderBuffer()
	{
		m_RenderBuffer = CreateRenderBuffer();
	}
	
	RenderBuffer::RenderBuffer(uint32_t width, uint32_t height, InternalFormat format) : RenderBuffer()
	{
		Storage(width, height, format);
	}
	
	void RenderBuffer::Bind() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, *this);
	}
	
	void RenderBuffer::Storage(uint32_t width, uint32_t height, InternalFormat format)
	{
		Bind();
		glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(format), width, height);
	}

	RenderBuffer * RenderBuffer::GetDefault()
	{
		static RenderBuffer renderBuffer(0);
		return &renderBuffer;
	}
}
