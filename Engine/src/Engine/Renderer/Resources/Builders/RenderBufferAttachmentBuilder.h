#pragma once
#include "Engine/Renderer/Resources/FramebufferSpecification.h"

namespace Engine {
	class RenderBufferAttachmentBuilder {
	public:
		RenderBufferAttachmentBuilder() = default;

		RenderBufferAttachmentBuilder& Format(TextureInternalFormat format);
		RenderBufferAttachmentBuilder& Label(const std::string& label);

		RenderBufferAttachmentBuilder& Clear();

		FramebufferRenderBufferAttachmentSpecification Build() const;

	private:
		FramebufferRenderBufferAttachmentSpecification m_Spec;

	};

}