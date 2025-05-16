#include "pch.h"
#include "FramebufferBuilder.h"

#include "Engine/Renderer/Builders/TextureAttachmentBuilder.h"
#include "Engine/Renderer/Builders/RenderBufferAttachmentBuilder.h"
#include "Engine/Renderer/Framebuffer.h"

namespace Engine {
	namespace Utils {
		constexpr static void SetBaseAttachmentSpecification(FramebufferAttachmentSpecificationBase& baseSpecs,
		                                                     ImageFormat format,
		                                                     const std::string& label) {
			baseSpecs.Format = format;
			baseSpecs.Label = label;
		}

		constexpr FramebufferTextureAttachmentSpecification CreateAttachmentTextureSpecification(
			ImageFormat format, FilterMode minFilter, FilterMode magFilter, WrapMode wrapS, WrapMode wrapT,
			bool useSRGB, uint32_t mipLevel, uint32_t layer, const std::string& label) {
			FramebufferTextureAttachmentSpecification textureSpecs;

			SetBaseAttachmentSpecification(textureSpecs, format, label);

			textureSpecs.Layer = layer;
			textureSpecs.MipLevel = mipLevel;

			textureSpecs.MinFilter = minFilter;
			textureSpecs.MagFilter = magFilter;

			textureSpecs.UseSRGB = useSRGB;

			textureSpecs.WrapS = wrapS;
			textureSpecs.WrapT = wrapT;

			return textureSpecs;
		}

		constexpr FramebufferAttachmentSpecification CreateAttachmentSpecification(
			bool renderBuffer, ImageFormat format, uint32_t mipLevel, uint32_t layer, const std::string& label) {
			FramebufferAttachmentSpecification specs;

			if (renderBuffer) {
				FramebufferRenderBufferAttachmentSpecification renderBufferSpecs;

				SetBaseAttachmentSpecification(renderBufferSpecs, format, label);

				specs = renderBufferSpecs;
			}
			else {
				FramebufferTextureAttachmentSpecification textureSpecs;

				SetBaseAttachmentSpecification(textureSpecs, format, label);

				textureSpecs.MipLevel = mipLevel;
				textureSpecs.Layer = layer;

				specs = textureSpecs;
			}

			return specs;
		}

		constexpr FramebufferRenderBufferAttachmentSpecification CreateAttachmentRenderBufferSpecification(
			ImageFormat format, const std::string& label) {
			FramebufferRenderBufferAttachmentSpecification specs;

			SetBaseAttachmentSpecification(specs, format, label);

			return specs;
		}
	}

	FramebufferBuilder::FramebufferBuilder(const Vector2u& size) {
		m_Spec.Size = size;
	}

	FramebufferBuilder& FramebufferBuilder::Size(const Vector2u& size) {
		m_Spec.Size = size;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(const FramebufferTextureAttachmentSpecification& specs) {
		m_Spec.ColorAttachments.emplace_back(specs);
		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(
		const FramebufferRenderBufferAttachmentSpecification& specs) {
		m_Spec.ColorAttachments.emplace_back(specs);
		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(const TextureAttachmentBuilder& builder) {
		return AddColorAttachment(builder.Build());
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(const RenderBufferAttachmentBuilder& builder) {
		return AddColorAttachment(builder.Build());
	}

	FramebufferBuilder& FramebufferBuilder::DepthAttachment(const FramebufferTextureAttachmentSpecification& specs) {
		if (Utils::IsDepthFormat(specs.Format)) {
			m_Spec.DepthAttachment = specs;
		} else {
			ENGINE_ASSERT(FALSE);
			throw std::runtime_error("Attempted to set not depth formated texture");
		}

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::DepthAttachment(
		const FramebufferRenderBufferAttachmentSpecification& specs) {
		if (Utils::IsDepthFormat(specs.Format)) {
			m_Spec.DepthAttachment = specs;
		}
		else {
			ENGINE_ASSERT(FALSE);
			throw std::runtime_error("Attempted to set not depth formated texture");
		}

		m_Spec.DepthAttachment = specs;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::DepthAttachment(const TextureAttachmentBuilder& builder) {
		return DepthAttachment(builder.Build());
	}

	FramebufferBuilder& FramebufferBuilder::DepthAttachment(const RenderBufferAttachmentBuilder& builder) {
		return DepthAttachment(builder.Build());
	}

	FramebufferBuilder& FramebufferBuilder::DepthTextureAttachment(ImageFormat format, FilterMode minFilter,
	                                                               FilterMode magFilter, WrapMode wrapS,
	                                                               WrapMode wrapT, bool useSRGB, uint32_t mipLevel,
	                                                               uint32_t layer,
	                                                               const std::string& label) {
		return DepthAttachment(Utils::CreateAttachmentTextureSpecification(
			format, minFilter, magFilter, wrapS, wrapT, useSRGB,
			mipLevel, layer, label));
	}

	FramebufferBuilder& FramebufferBuilder::DepthRenderBufferAttachment(ImageFormat format, const std::string& label) {
		return DepthAttachment(Utils::CreateAttachmentRenderBufferSpecification(format, label));
	}

	FramebufferBuilder& FramebufferBuilder::AddTextureColorAttachment(ImageFormat format, FilterMode minFilter,
	                                                                  FilterMode magFilter, WrapMode wrapS,
	                                                                  WrapMode wrapT, bool useSRGB, uint32_t mipLevel,
	                                                                  uint32_t layer,
	                                                                  const std::string& label) {
		return AddColorAttachment(
			Utils::CreateAttachmentTextureSpecification(format, minFilter, magFilter, wrapS, wrapT, useSRGB, mipLevel,
			                                            layer, label));
	}

	FramebufferBuilder& FramebufferBuilder::AddRenderBufferColorAttachment(ImageFormat format, const std::string& label) {
		return AddColorAttachment(Utils::CreateAttachmentRenderBufferSpecification(format, label));
	}

	FramebufferBuilder& FramebufferBuilder::Samples(uint32_t samples) {
		m_Spec.Samples = samples;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SwapchainTarget(bool value) {
		m_Spec.SwapchainTarget = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AllowBlit(bool value) {
		m_Spec.AllowBlit = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::Layered(bool value) {
		m_Spec.Layered = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::Label(std::string label) {
		m_Spec.Label = std::move(label);

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::Clear() {
		m_Spec = {};

		return *this;
	}

	FramebufferSpecification FramebufferBuilder::BuildSpecification() const {
		return m_Spec;
	}

	Ref<Framebuffer> FramebufferBuilder::Build() const {
		return Framebuffer::Create(BuildSpecification());
	}
}
