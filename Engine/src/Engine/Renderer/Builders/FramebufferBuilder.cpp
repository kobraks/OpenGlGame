#include "pch.h"
#include "FramebufferBuilder.h"

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

		constexpr bool IsDepth(ImageFormat format) {
			return format >= ImageFormat::DepthComponent && format <= ImageFormat::StencilIndex16;
		}
	}

	FramebufferBuilder::FramebufferBuilder(const Vector2u& size) {
		m_Specification.Size = size;
	}

	FramebufferBuilder& FramebufferBuilder::Size(const Vector2u& size) {
		m_Specification.Size = size;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(const FramebufferTextureAttachmentSpecification& specs) {
		m_Specification.ColorAttachments.emplace_back(specs);
		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::AddColorAttachment(
		const FramebufferRenderBufferAttachmentSpecification& specs) {
		m_Specification.ColorAttachments.emplace_back(specs);
		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetDepthAttachment(const FramebufferTextureAttachmentSpecification& specs) {
		if (Utils::IsDepth(specs.Format)) {
			m_Specification.DepthAttachment = specs;
		} else {
			ENGINE_ASSERT(FALSE);
			throw std::runtime_error("Attempted to set not depth formated texture");
		}

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetDepthAttachment(
		const FramebufferRenderBufferAttachmentSpecification& specs) {
		if (Utils::IsDepth(specs.Format)) {
			m_Specification.DepthAttachment = specs;
		}
		else {
			ENGINE_ASSERT(FALSE);
			throw std::runtime_error("Attempted to set not depth formated texture");
		}

		m_Specification.DepthAttachment = specs;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetDepthTextureAttachment(ImageFormat format, FilterMode minFilter,
	                                                                  FilterMode magFilter, WrapMode wrapS,
	                                                                  WrapMode wrapT, bool useSRGB, uint32_t mipLevel,
	                                                                  uint32_t layer,
	                                                                  const std::string& label) {
		return SetDepthAttachment(Utils::CreateAttachmentTextureSpecification(
			format, minFilter, magFilter, wrapS, wrapT, useSRGB,
			mipLevel, layer, label));
	}

	FramebufferBuilder& FramebufferBuilder::SetDepthRenderBufferAttachment(ImageFormat format, const std::string& label) {
		return SetDepthAttachment(Utils::CreateAttachmentRenderBufferSpecification(format, label));
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

	FramebufferBuilder& FramebufferBuilder::SetSamples(uint32_t samples) {
		m_Specification.Samples = samples;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetSwapchainTarget(bool value) {
		m_Specification.SwapchainTarget = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetAllowBlit(bool value) {
		m_Specification.AllowBlit = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetLayered(bool value) {
		m_Specification.Layered = value;

		return *this;
	}

	FramebufferBuilder& FramebufferBuilder::SetLabel(std::string label) {
		m_Specification.Label = std::move(label);

		return *this;
	}

	FramebufferSpecification FramebufferBuilder::BuildSpecification() const {
		return m_Specification;
	}

	Ref<Framebuffer> FramebufferBuilder::Build() const {
		return Framebuffer::Create(BuildSpecification());
	}
}
