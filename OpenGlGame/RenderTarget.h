#pragma once

namespace Game
{
	class FrameBufferObject;
	class Texture;
	
	class RenderTarget
	{
		friend class Renderer;

		Pointer<FrameBufferObject> m_FrameBuffer;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		bool m_HasStencilAndDepth = true;
	
	public:
		RenderTarget(uint32_t width, uint32_t height, uint32_t nrColorAttachments = 1, bool depthAndStencil = true);

		Pointer<Texture> GetDepthStencilTexture() const;
		Pointer<Texture> GetColorAttachment(uint32_t index) const;

		void Resize(uint32_t width, uint32_t height);
		
	};
}
