#include "pch.h"
#include "Engine/Renderer/RenderBuffer.h"
#include "Engine/Utils/Renderer/GLEnumConverters.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"

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
		m_GLState->Label = label;
	}

	void RenderBuffer::Bind() const {
		glBindRenderbuffer(GL_RENDERBUFFER, *this);
	}

	void RenderBuffer::Unbind() const {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	uint32_t RenderBuffer::QueryMaxSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	Vector2u RenderBuffer::QueryMaxSize() {
		static Vector2u maxSize = {};

		if (maxSize == Vector2u()) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &tmp);
			maxSize = {static_cast<uint32_t>(tmp), static_cast<uint32_t>(tmp)};
		}

		return maxSize;
	}

	RenderBuffer::RenderBuffer(bool multisampled) : m_GLState(MakeRef<GLState>(multisampled)){
	}

	RenderBuffer::GLState::GLState(bool multisampled) : ID(Utils::GenRenderBuffer()), Multisampled(multisampled) {
	}

	RenderBuffer::GLState::~GLState() {
		glDeleteRenderbuffers(1, &ID);
	}

	void RenderBuffer::CreateBuffer(uint32_t samples, const Vector2u& size, enum ImageFormat imageFormat) {
		ENGINE_ASSERT(size.Width > 0 && size.Height > 0);
		ENGINE_ASSERT(samples > 0);

		if (size.Width == 0 || size.Height == 0)
			throw std::runtime_error(fmt::format("Invalid size of RenderBuffer {}!", size));

		const auto maxSize = QueryMaxSize();
		ENGINE_ASSERT(size.Width <= maxSize.Width && size.Height <= maxSize.Height);
		if (size.Width > maxSize.Width || size.Height > maxSize.Height) {
			throw std::out_of_range(fmt::format("Requested size {} exceeds maximum supported size {}!", size, QueryMaxSize()));
		}

		if (samples == 0)
			throw std::runtime_error("Samples must be at least 1 in RenderBuffer!");

		if (samples > 1) {
			const auto maxSamples = QueryMaxSamples();
			ENGINE_ASSERT(samples <= maxSamples);
			if (samples > maxSamples) {
				throw std::out_of_range("Requested samples exceed maximum supported samples!");
			}

			Allocate(samples, size, imageFormat);
		}
		else {
			Allocate(size, imageFormat);
		}
	}

	void RenderBuffer::Allocate(uint32_t samples, const Vector2u& size, enum Engine::ImageFormat imageFormat) {
		m_GLState->Samples = samples;
		m_GLState->ImageFormat = imageFormat;
		m_GLState->Size = size;

		glNamedRenderbufferStorageMultisample(*this, static_cast<GLsizei>(samples), Utils::EnumToGLConstant(imageFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void RenderBuffer::Allocate(const Vector2u& size, enum Engine::ImageFormat imageFormat) {
		m_GLState->Samples = 1;
		m_GLState->ImageFormat = imageFormat;
		m_GLState->Size = size;

		glNamedRenderbufferStorage(*this, Utils::EnumToGLConstant(imageFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}
}
