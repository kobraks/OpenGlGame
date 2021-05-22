#include "pch.h"
#include "RenderTarget.h"

#include "FrameBuffer.h"
#include "Texture.h"

namespace Game
{
	RenderTarget::RenderTarget(uint32_t width, uint32_t height, uint32_t nrColorAttachments, bool depthAndStencil)
	{
		m_FrameBuffer = MakePointer<FrameBuffer<Texture, Texture>>(width, height, nrColorAttachments, 32, 24, depthAndStencil);
	}
	Pointer<Texture> RenderTarget::GetDepthStencilTexture() const
	{
		return std::dynamic_pointer_cast<FrameBuffer<Texture, Texture>>(m_FrameBuffer)->GetDepthBuffer();
	}
	Pointer<Texture> RenderTarget::GetColorAttachment(uint32_t index) const
	{
		return std::dynamic_pointer_cast<FrameBuffer<Texture, Texture>>(m_FrameBuffer)->GetColorBuffer(index);
	}
	
	void RenderTarget::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		auto buffer = std::dynamic_pointer_cast<FrameBuffer<Texture, Texture>>(m_FrameBuffer);

		for (uint32_t i = 0; i < buffer->GetNumColorAttachments(); ++i)
		{
			buffer->GetColorAttachment(i)->Resize(width, height);
		}
	}
}
