#pragma once
#include "Engine/Renderer/FramebufferSpecification.h"

namespace Engine {
	class RenderBufferAttachmentBuilder {
	public:
		RenderBufferAttachmentBuilder() = default;

		RenderBufferAttachmentBuilder& Format(ImageFormat format);
		RenderBufferAttachmentBuilder& Label(const std::string& label);

		RenderBufferAttachmentBuilder& Clear();

		FramebufferRenderBufferAttachmentSpecification Build() const;

	private:
		FramebufferRenderBufferAttachmentSpecification m_Spec;

	};

}