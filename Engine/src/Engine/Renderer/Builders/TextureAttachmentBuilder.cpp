#include "pch.h"
#include "TextureAttachmentBuilder.h"

namespace Engine {
	TextureAttachmentBuilder& TextureAttachmentBuilder::Format(ImageFormat format) {
		m_Spec.Format = format;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::Label(const std::string& label) {
		m_Spec.Label = label;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::Filters(FilterMode min, FilterMode mag){
		m_Spec.MinFilter = min;
		m_Spec.MagFilter = mag;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::MagFilter(FilterMode mag) {
		m_Spec.MagFilter = mag;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::MinFilter(FilterMode min) {
		m_Spec.MinFilter = min;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::Wrapping(WrapMode s, WrapMode t) {
		m_Spec.WrapS = s;
		m_Spec.WrapT = t;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::WrapS(WrapMode s) {
		m_Spec.WrapS = s;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::WrapT(WrapMode t) {
		m_Spec.WrapT = t;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::MipLevel(uint32_t level) {
		m_Spec.MipLevel = level;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::Layer(uint32_t layer) {
		m_Spec.Layer = layer;

		return *this;
	}

	TextureAttachmentBuilder& TextureAttachmentBuilder::SRGB(bool enabled) {
		m_Spec.UseSRGB = enabled;

		return *this;
	}

	FramebufferTextureAttachmentSpecification TextureAttachmentBuilder::Build() const {
		return m_Spec;
	}
}
