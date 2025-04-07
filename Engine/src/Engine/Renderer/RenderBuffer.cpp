#include "pch.h"
#include "Engine/Renderer/RenderBuffer.h"
#include "Engine/Utils/OpenGlUtils.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		RenderBuffer::IDType GenRenderBuffer() {
			RenderBuffer::IDType name;

			glCreateRenderbuffers(1, &name);
			return name;
		}
	}

	Ref<RenderBuffer> RenderBuffer::Create(const Vector2u& size, uint32_t samples, Engine::ImageFormat imageFormat,
		const std::string& label) {
		auto renderBuffer = Ref<RenderBuffer>(new RenderBuffer(samples > 1));

		renderBuffer->CreateBuffer(samples, size, imageFormat);
		renderBuffer->SetLabel(label);

		return renderBuffer;
	}

	void RenderBuffer::SetLabel(const std::string& label) {
		if (label.empty())
			return;

		glObjectLabel(GL_RENDERBUFFER, *this, -1, label.c_str());
		m_Internals->Label = label;
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

	void RenderBuffer::CreateBuffer(uint32_t samples, const Vector2u& size, enum ImageFormat imageFormat) {
		ENGINE_ASSERT(size.Width > 0 && size.Height > 0);
		ENGINE_ASSERT(samples > 0);

		if (size.Width == 0 || size.Height == 0)
			throw std::runtime_error(fmt::format("Invalid size of RenderBuffer {}!", size));

		if (samples == 0)
			throw std::runtime_error("Samples must be at least 1 in RenderBuffer!");

		if (samples > 1) {
			Allocate(samples, size, imageFormat);
		}
		else {
			Allocate(size, imageFormat);
		}
	}

	void RenderBuffer::Allocate(uint32_t samples, const Vector2u& size, enum Engine::ImageFormat imageFormat) {
		m_Internals->Samples = samples;
		m_Internals->ImageFormat = imageFormat;
		m_Internals->Size = size;

		glNamedRenderbufferStorageMultisample(*this, static_cast<GLsizei>(samples), Utils::ToGLImageFormat(imageFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void RenderBuffer::Allocate(const Vector2u& size, enum Engine::ImageFormat imageFormat) {
		m_Internals->Samples = 1;
		m_Internals->ImageFormat = imageFormat;
		m_Internals->Size = size;

		glNamedRenderbufferStorage(*this, Utils::ToGLImageFormat(imageFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}
}
