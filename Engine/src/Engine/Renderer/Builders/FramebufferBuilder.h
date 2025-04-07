#pragma once
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/FramebufferSpecification.h"
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine {
	class Framebuffer;

	class FramebufferBuilder {
	public:
		FramebufferBuilder() = default;
		explicit FramebufferBuilder(const Vector2u& size);

		FramebufferBuilder& Size(const Vector2u& size);

		FramebufferBuilder& AddColorAttachment(const FramebufferTextureAttachmentSpecification& specs);
		FramebufferBuilder& AddColorAttachment(const FramebufferRenderBufferAttachmentSpecification& specs);

		FramebufferBuilder& AddTextureColorAttachment(ImageFormat format, FilterMode minFilter = FilterMode::Linear, FilterMode magFilter = FilterMode::Linear, WrapMode wrapS = WrapMode::ClampEdge, WrapMode wrapT = WrapMode::ClampEdge, bool useSRGB = false, uint32_t mipLevel = 0, uint32_t layer = 0, const std::string& label = {});
		FramebufferBuilder& AddRenderBufferColorAttachment(ImageFormat format, const std::string& label = {});

		FramebufferBuilder& SetDepthAttachment(const FramebufferTextureAttachmentSpecification& specs);
		FramebufferBuilder& SetDepthAttachment(const FramebufferRenderBufferAttachmentSpecification& specs);

		FramebufferBuilder& SetDepthTextureAttachment(ImageFormat format, FilterMode minFilter = FilterMode::Nearest, FilterMode magFilter = FilterMode::Nearest, WrapMode wrapS = WrapMode::ClampEdge, WrapMode wrapT = WrapMode::ClampEdge, bool useSRGB = false, uint32_t mipLevel = 0, uint32_t layer = 0, const std::string& label = {});
		FramebufferBuilder& SetDepthRenderBufferAttachment(ImageFormat format, const std::string& label = {});

		FramebufferBuilder& SetSamples(uint32_t samples);
		FramebufferBuilder& SetSwapchainTarget(bool value = true);
		FramebufferBuilder& SetAllowBlit(bool value = true);
		FramebufferBuilder& SetLayered(bool value = true);
		FramebufferBuilder& SetLabel(std::string label);

		FramebufferSpecification BuildSpecification() const;
		Ref<Framebuffer> Build() const;
	private:

		FramebufferSpecification m_Specification;
	};
}
