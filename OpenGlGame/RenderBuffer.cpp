#include "pch.h"
#include "RenderBuffer.h"

#include "GLCheck.h"

namespace Game
{
	RenderBuffer::Internals::Internals()
	{
		glGenRenderbuffers(1, &Id);
	}

	RenderBuffer::Internals::Internals(IdType id)
	{
		m_IdProvided = true;
		Id           = id;
	}

	RenderBuffer::Internals::~Internals()
	{
		if(!m_IdProvided)
			glDeleteRenderbuffers(1, &Id);
	}

	RenderBuffer::RenderBuffer(IdType id)
	{
		m_Internals = MakePointer<Internals>(id);
	}

	RenderBuffer::RenderBuffer()
	{
		m_Internals = MakePointer<Internals>();
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
		Storage(width, height, format, 1);
	}

	void RenderBuffer::Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples)
	{
		m_Internals->Size    = {width, height};
		m_Internals->Format  = format;
		m_Internals->Samples = samples;

		GL_CHECK(glNamedRenderbufferStorageMultisample(m_Internals->Id, samples, static_cast<GLenum>(format), width, height));
	}

	void RenderBuffer::Storage(const Vector2u &size, InternalFormat format)
	{
		Storage(size, format, 1);
	}

	void RenderBuffer::Storage(const Vector2u &size, InternalFormat format, uint32_t samples)
	{
		m_Internals->Size    = size;
		m_Internals->Format  = format;
		m_Internals->Samples = samples;

		glNamedRenderbufferStorage(m_Internals->Id, static_cast<GLenum>(format), size.Width, size.Height);
	}

	RenderBuffer* RenderBuffer::GetDefault()
	{
		static RenderBuffer renderBuffer(0);
		return &renderBuffer;
	}

	uint32_t RenderBuffer::MaxSamples()
	{
		static int samples  = 0;
		static bool checked = false;

		if(!checked)
		{
			checked = true;

			glGetIntegerv(GL_MAX_SAMPLES, &samples);
		}

		return static_cast<unsigned>(samples);
	}
}
