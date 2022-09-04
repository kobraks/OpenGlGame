#include "pch.h"
#include "Engine/OpenGL/RenderBuffer.h"

#include "Engine/Renderer/Context.h"

namespace Game
{
	RenderBuffer::Internals::Internals()
	{
		Functions = Context::GetContext()->GetFunctions();

		ID = Functions.GenRenderBuffer();
	}

	RenderBuffer::Internals::Internals(IDType id)
	{
		m_Provided = true;
		Functions = Context::GetContext()->GetFunctions();
		ID = id;
	}

	RenderBuffer::Internals::~Internals()
	{
		if (!m_Provided)
			Functions.DeleteRenderBuffer(ID);
	}

	RenderBuffer::RenderBuffer(IDType id) : m_Internals(MakePointer<Internals>(id)) {}
	RenderBuffer::RenderBuffer() : m_Internals(MakePointer<Internals>()) {}
	RenderBuffer::RenderBuffer(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples) : RenderBuffer()
	{
		Storage(width, height, format, samples);
	}

	RenderBuffer::RenderBuffer(const Vector2u &size, InternalFormat format, uint32_t samples) : RenderBuffer()
	{
		Storage(size, format, samples);
	}

	void RenderBuffer::Bind() const
	{
		m_Internals->Functions.BindRenderBuffer(*this);
	}

	void RenderBuffer::Storage(uint32_t width, uint32_t height, InternalFormat format, uint32_t samples)
	{
		ASSERT(samples < MaxSamples())

		m_Internals->Size = {width, height};
		m_Internals->Format = format;
		m_Internals->Samples = samples;

		if (samples == 0)
			m_Internals->Functions.RenderBufferStorage(*this, format, width, height);
		else
			m_Internals->Functions.RenderBufferStorage(*this, samples, format, width, height);
	}

	void RenderBuffer::Storage(const Vector2u &size, InternalFormat format, uint32_t samples)
	{
		ASSERT(samples < MaxSamples())

		m_Internals->Size = size;
		m_Internals->Format = format;
		m_Internals->Samples = samples;

		if (samples == 0)
			m_Internals->Functions.RenderBufferStorage(*this, format, size);
		else
			m_Internals->Functions.RenderBufferStorage(*this, samples, format, size);
	}

	RenderBuffer *RenderBuffer::GetDefault()
	{
		static RenderBuffer s_RenderBuffer(0);
		return &s_RenderBuffer;
	}

	uint32_t RenderBuffer::MaxSamples()
	{
		static int samples = 0;
		static bool check = false;

		if (!check)
		{
			check = true;

			samples = Context::GetContext()->GetFunctions().GetInteger(GL_MAX_SAMPLES);
		}

		return static_cast<uint32_t>(samples);
	}
}
