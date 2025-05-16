#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Vector2.h"

#include "Engine/Renderer/GraphicEnums.h"

#include <variant>

namespace Engine {
	struct FramebufferAttachmentSpecificationBase {
		ImageFormat Format = ImageFormat::RGBA8;

		std::string Label;
	};

	struct FramebufferTextureAttachmentSpecification : FramebufferAttachmentSpecificationBase{
		uint32_t MipLevel = 0;
		uint32_t Layer = 0;

		bool UseSRGB = false;

		FilterMode MinFilter = FilterMode::Linear;
		FilterMode MagFilter = FilterMode::Linear;

		WrapMode WrapS = WrapMode::ClampEdge;
		WrapMode WrapT = WrapMode::ClampEdge;

	};

	struct FramebufferRenderBufferAttachmentSpecification : FramebufferAttachmentSpecificationBase {
		//EMPTY for now :D
	};

	using FramebufferAttachmentSpecification = std::variant<FramebufferTextureAttachmentSpecification, FramebufferRenderBufferAttachmentSpecification>;

	struct FramebufferSpecification {
		Vector2u Size = { 1, 1 };

		std::vector<FramebufferAttachmentSpecification> ColorAttachments;
		std::optional<FramebufferAttachmentSpecification> DepthAttachment = std::nullopt;

		uint32_t Samples = 1;
		bool SwapchainTarget = false;
		bool AllowBlit = false;
		bool Layered = false;

		std::string Label;

		constexpr bool IsMultisampled() const { return Samples > 1; }
	};
}