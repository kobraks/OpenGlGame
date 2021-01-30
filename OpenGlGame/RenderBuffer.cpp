#include "pch.h"
#include "RenderBuffer.h"

#include "GLCheck.h"

namespace Game
{
	static void DestroyRenderBuffer(RenderBuffer::IdType *id)
	{
		GL_CHECK(glDeleteRenderbuffers(1, id));
		delete id;
	}

	static auto CreateRenderBuffer()
	{
		auto renderBuffer = Pointer<RenderBuffer::IdType>(new RenderBuffer::IdType{}, DestroyRenderBuffer);
		GL_CHECK(glGenRenderbuffers(1, &*renderBuffer));

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

	RenderBuffer::RenderBuffer(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples)
	{
		Storage(width, height, format, samples);
	}

	void RenderBuffer::Bind() const
	{
		GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, *this));
	}

	void RenderBuffer::Storage(uint32_t width, uint32_t height, InternalFormat format)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_Samples = 1;
		
		GL_CHECK(glNamedRenderbufferStorage(*m_RenderBuffer, static_cast<GLenum>(format), width, height));
	}

	void RenderBuffer::Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_Samples = samples;
		
		GL_CHECK(glNamedRenderbufferStorageMultisample(*m_RenderBuffer, samples, static_cast<GLenum>(format), width, height));
	}

	RenderBuffer* RenderBuffer::GetDefault()
	{
		static RenderBuffer renderBuffer(0);
		return &renderBuffer;
	}

	uint32_t RenderBuffer::MaxSamples()
	{
		static int samples = 0;
		static bool checked = false;

		if (!checked)
		{
			checked = true;

			glGetIntegerv(GL_MAX_SAMPLES, &samples);
		}

		return static_cast<unsigned>(samples);
	}
}
