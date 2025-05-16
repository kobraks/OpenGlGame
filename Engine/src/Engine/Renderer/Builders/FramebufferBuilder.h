#pragma once
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/FramebufferSpecification.h"
#include "Engine/Renderer/GraphicEnums.h"


namespace Engine {
	class Framebuffer;
	class TextureAttachmentBuilder;
	class RenderBufferAttachmentBuilder;

	class FramebufferBuilder {
	public:
		FramebufferBuilder() = default;
		explicit FramebufferBuilder(const Vector2u& size);

		FramebufferBuilder& Size(const Vector2u& size);

		FramebufferBuilder& AddColorAttachment(const FramebufferTextureAttachmentSpecification& specs);
		FramebufferBuilder& AddColorAttachment(const FramebufferRenderBufferAttachmentSpecification& specs);

		FramebufferBuilder& AddColorAttachment(const TextureAttachmentBuilder& builder);
		FramebufferBuilder& AddColorAttachment(const RenderBufferAttachmentBuilder& builder);

		FramebufferBuilder& AddTextureColorAttachment(ImageFormat format, FilterMode minFilter = FilterMode::Linear, FilterMode magFilter = FilterMode::Linear, WrapMode wrapS = WrapMode::ClampEdge, WrapMode wrapT = WrapMode::ClampEdge, bool useSRGB = false, uint32_t mipLevel = 0, uint32_t layer = 0, const std::string& label = {});
		FramebufferBuilder& AddRenderBufferColorAttachment(ImageFormat format, const std::string& label = {});

		FramebufferBuilder& DepthAttachment(const FramebufferTextureAttachmentSpecification& specs);
		FramebufferBuilder& DepthAttachment(const FramebufferRenderBufferAttachmentSpecification& specs);

		FramebufferBuilder& DepthAttachment(const TextureAttachmentBuilder& builder);
		FramebufferBuilder& DepthAttachment(const RenderBufferAttachmentBuilder& builder);

		FramebufferBuilder& DepthTextureAttachment(ImageFormat format, FilterMode minFilter = FilterMode::Nearest, FilterMode magFilter = FilterMode::Nearest, WrapMode wrapS = WrapMode::ClampEdge, WrapMode wrapT = WrapMode::ClampEdge, bool useSRGB = false, uint32_t mipLevel = 0, uint32_t layer = 0, const std::string& label = {});
		FramebufferBuilder& DepthRenderBufferAttachment(ImageFormat format, const std::string& label = {});

		FramebufferBuilder& Samples(uint32_t samples);
		FramebufferBuilder& SwapchainTarget(bool value = true);
		FramebufferBuilder& AllowBlit(bool value = true);
		FramebufferBuilder& Layered(bool value = true);
		FramebufferBuilder& Label(std::string label);

		FramebufferBuilder& Clear();

		FramebufferSpecification BuildSpecification() const;
		Ref<Framebuffer> Build() const;
	private:

		FramebufferSpecification m_Spec;
	};
}
