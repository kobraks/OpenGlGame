#include "pch.h"
#include "RenderBufferAttachmentBuilder.h"

namespace Engine {
	RenderBufferAttachmentBuilder& RenderBufferAttachmentBuilder::Format(ImageFormat format) {
		m_Spec.Format = format;

		return *this;
	}

	RenderBufferAttachmentBuilder& RenderBufferAttachmentBuilder::Label(const std::string& label) {
		m_Spec.Label = label;

		return *this;
	}

	RenderBufferAttachmentBuilder& RenderBufferAttachmentBuilder::Clear() {
		m_Spec = {};

		return *this;
	}

	FramebufferRenderBufferAttachmentSpecification RenderBufferAttachmentBuilder::Build() const {
		return m_Spec;
	}
}
