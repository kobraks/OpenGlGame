#pragma once
#include "Engine/Renderer/FramebufferSpecification.h"

namespace Engine {
	class TextureAttachmentBuilder {
	public:
		TextureAttachmentBuilder() = default;

		TextureAttachmentBuilder& Format(ImageFormat format);
		TextureAttachmentBuilder& Label(const std::string& label);

		TextureAttachmentBuilder& Filters(FilterMode min, FilterMode mag);
		TextureAttachmentBuilder& MagFilter(FilterMode mag);
		TextureAttachmentBuilder& MinFilter(FilterMode min);

		TextureAttachmentBuilder& Wrapping(WrapMode s, WrapMode t);
		TextureAttachmentBuilder& WrapS(WrapMode s);
		TextureAttachmentBuilder& WrapT(WrapMode t);

		TextureAttachmentBuilder& MipLevel(uint32_t level);
		TextureAttachmentBuilder& Layer(uint32_t layer);
		TextureAttachmentBuilder& SRGB(bool enabled);

		FramebufferTextureAttachmentSpecification Build() const;

	private:
		FramebufferTextureAttachmentSpecification m_Spec;

	};
}