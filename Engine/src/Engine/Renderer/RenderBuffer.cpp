#include "pch.h"
#include "Engine/Renderer/RenderBuffer.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		RenderBuffer::IDType GenRenderBuffer() {
			RenderBuffer::IDType name;

			glCreateRenderbuffers(1, &name);
			return name;
		}

		constexpr GLenum GLInternalFormat(RenderBuffer::InternalFormat format) {
			switch (format) {
			case RenderBuffer::InternalFormat::CompressedRed:
				return GL_COMPRESSED_RED;
			case RenderBuffer::InternalFormat::CompressedRedRGTC1:
				return GL_COMPRESSED_RED_RGTC1;
			case RenderBuffer::InternalFormat::CompressedRG:
				return GL_COMPRESSED_RG;
			case RenderBuffer::InternalFormat::CompressedRGB:
				return GL_COMPRESSED_RGB;
			case RenderBuffer::InternalFormat::CompressedRGBA:
				return GL_COMPRESSED_RGBA;
			case RenderBuffer::InternalFormat::CompressedRGRGTC2:
				return GL_COMPRESSED_RG_RGTC2;
			case RenderBuffer::InternalFormat::CompressedSignedRedRGTC1:
				return GL_COMPRESSED_SIGNED_RED_RGTC1;
			case RenderBuffer::InternalFormat::CompressedSignedRGRGTC2:
				return GL_COMPRESSED_SIGNED_RG_RGTC2;
			case RenderBuffer::InternalFormat::CompressedSRGB:
				return GL_COMPRESSED_SRGB;
			case RenderBuffer::InternalFormat::DepthStencil:
				return GL_DEPTH_STENCIL;
			case RenderBuffer::InternalFormat::Depth24Stencil8:
				return GL_DEPTH24_STENCIL8;
			case RenderBuffer::InternalFormat::Depth32FStencil8:
				return GL_DEPTH32F_STENCIL8;
			case RenderBuffer::InternalFormat::DepthComponent:
				return GL_DEPTH_COMPONENT;
			case RenderBuffer::InternalFormat::DepthComponent16:
				return GL_DEPTH_COMPONENT16;
			case RenderBuffer::InternalFormat::DepthComponent24:
				return GL_DEPTH_COMPONENT24;
			case RenderBuffer::InternalFormat::DepthComponent32F:
				return GL_DEPTH_COMPONENT32F;
			case RenderBuffer::InternalFormat::DepthComponent32:
				return GL_DEPTH_COMPONENT32;
			case RenderBuffer::InternalFormat::R16F:
				return GL_R16F;
			case RenderBuffer::InternalFormat::R16I:
				return GL_R16I;
			case RenderBuffer::InternalFormat::R16SNorm:
				return GL_R16_SNORM;
			case RenderBuffer::InternalFormat::R16UI:
				return GL_R16UI;
			case RenderBuffer::InternalFormat::R32F:
				return GL_R32F;
			case RenderBuffer::InternalFormat::R32I:
				return GL_R32I;
			case RenderBuffer::InternalFormat::R32UI:
				return GL_R32UI;
			case RenderBuffer::InternalFormat::R3G3B2:
				return GL_R3_G3_B2;
			case RenderBuffer::InternalFormat::R8:
				return GL_R8;
			case RenderBuffer::InternalFormat::R8I:
				return GL_R8I;
			case RenderBuffer::InternalFormat::R8SNorm:
				return GL_R8_SNORM;
			case RenderBuffer::InternalFormat::R8UI:
				return GL_R8UI;
			case RenderBuffer::InternalFormat::Red:
				return GL_RED;
			case RenderBuffer::InternalFormat::RG:
				return GL_RG;
			case RenderBuffer::InternalFormat::RG16:
				return GL_RG16;
			case RenderBuffer::InternalFormat::RG16F:
				return GL_RG16F;
			case RenderBuffer::InternalFormat::RG16SNorm:
				return GL_RG16_SNORM;
			case RenderBuffer::InternalFormat::RG32F:
				return GL_RG32F;
			case RenderBuffer::InternalFormat::RG32I:
				return GL_RG32I;
			case RenderBuffer::InternalFormat::RG32UI:
				return GL_RG32UI;
			case RenderBuffer::InternalFormat::RG8:
				return GL_RG8;
			case RenderBuffer::InternalFormat::RG8I:
				return GL_RGB8I;
			case RenderBuffer::InternalFormat::RG8SNorm:
				return GL_RG8_SNORM;
			case RenderBuffer::InternalFormat::RG8UI:
				return GL_RG8UI;
			case RenderBuffer::InternalFormat::RGB:
				return GL_RGB;
			case RenderBuffer::InternalFormat::RGB10:
				return GL_RGB10;
			case RenderBuffer::InternalFormat::RGB10A2:
				return GL_RGB10_A2;
			case RenderBuffer::InternalFormat::RGB12:
				return GL_RGB12;
			case RenderBuffer::InternalFormat::RGB16:
				return GL_RGB16;
			case RenderBuffer::InternalFormat::RGB16F:
				return GL_RGB16F;
			case RenderBuffer::InternalFormat::RGB16I:
				return GL_RGB16I;
			case RenderBuffer::InternalFormat::RGB16UI:
				return GL_RGB16UI;
			case RenderBuffer::InternalFormat::RGB32F:
				return GL_RGB32F;
			case RenderBuffer::InternalFormat::RGB32I:
				return GL_RGB32I;
			case RenderBuffer::InternalFormat::RGB32UI:
				return GL_RGB32UI;
			case RenderBuffer::InternalFormat::RGB4:
				return GL_RGB4;
			case RenderBuffer::InternalFormat::RGB5:
				return GL_RGB5;
			case RenderBuffer::InternalFormat::RGB5A1:
				return GL_RGB5_A1;
			case RenderBuffer::InternalFormat::RGB8:
				return GL_RGB8;
			case RenderBuffer::InternalFormat::RGB8I:
				return GL_RGB8I;
			case RenderBuffer::InternalFormat::RGB8UI:
				return GL_RGB8UI;
			case RenderBuffer::InternalFormat::RGB9E5:
				return GL_RGB9_E5;
			case RenderBuffer::InternalFormat::RGBA:
				return GL_RGBA;
			case RenderBuffer::InternalFormat::RGBA12:
				return GL_RGBA12;
			case RenderBuffer::InternalFormat::RGBA16:
				return GL_RGBA16;
			case RenderBuffer::InternalFormat::RGBA16F:
				return GL_RGBA16F;
			case RenderBuffer::InternalFormat::RGBA16I:
				return GL_RGBA16I;
			case RenderBuffer::InternalFormat::RGBA16UI:
				return GL_RGBA16UI;
			case RenderBuffer::InternalFormat::RGBA2:
				return GL_RGBA2;
			case RenderBuffer::InternalFormat::RGBA32F:
				return GL_RGBA32F;
			case RenderBuffer::InternalFormat::RGBA32I:
				return GL_RGBA32I;
			case RenderBuffer::InternalFormat::RGBA32UI:
				return GL_RGBA32UI;
			case RenderBuffer::InternalFormat::RGBA4:
				return GL_RGBA4;
			case RenderBuffer::InternalFormat::RGBA8:
				return GL_RGBA8;
			case RenderBuffer::InternalFormat::RGBA8UI:
				return GL_RGBA8I;
			case RenderBuffer::InternalFormat::SRGB8:
				return GL_SRGB8;
			case RenderBuffer::InternalFormat::SRGB8A8:
				return GL_SRGB8_ALPHA8;
			case RenderBuffer::InternalFormat::SRGBA:
				return GL_SRGB_ALPHA;
			}
		}
	}

	Ref<RenderBuffer> RenderBuffer::Create(const Vector2u& size, uint32_t samples, InternalFormat internalFormat) {
		auto renderBuffer = Ref<RenderBuffer>(new RenderBuffer(samples > 1));

		renderBuffer->CreateBuffer(samples, size, internalFormat);

		return renderBuffer;
	}

	void RenderBuffer::Bind() const {
		glBindRenderbuffer(GL_RENDERBUFFER, *this);
	}

	void RenderBuffer::Unbind() const {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	RenderBuffer::RenderBuffer(bool multisampled) : m_Internals(MakeRef<Internals>(multisampled)){
	}

	RenderBuffer::Internals::Internals(bool multisampled) : ID(Utils::GenRenderBuffer()), Multisampled(multisampled) {
	}

	RenderBuffer::Internals::~Internals() {
		glDeleteRenderbuffers(1, &ID);
	}

	void RenderBuffer::Internals::Allocate(uint32_t samples, const Vector2u& size,
		enum RenderBuffer::InternalFormat internalFormat) {
		Samples = samples;
		InternalFormat = internalFormat;
		Size = size;

		glNamedRenderbufferStorageMultisample(ID, samples, Utils::GLInternalFormat(internalFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void RenderBuffer::Internals::Allocate(const Vector2u& size, enum RenderBuffer::InternalFormat internalFormat) {
		Samples = 1;
		InternalFormat = internalFormat;
		Size = size;

		glNamedRenderbufferStorage(ID, Utils::GLInternalFormat(internalFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void RenderBuffer::CreateBuffer(uint32_t samples, const Vector2u& size, enum InternalFormat internalFormat) {
		if (samples > 1) {
			m_Internals->Allocate(samples, size, internalFormat);
		}
		else {
			m_Internals->Allocate(size, internalFormat);
		}
	}
}
