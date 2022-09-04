#include "pch.h"
#include "Engine/Renderer/Context.h"


#define CHECK_FOR_CURRENT_CONTEXT() { \
	ASSERT(m_Context->IsCurrent(), "Not in current OpenGL context");\
	if (!m_Context->IsCurrent())\
		throw std::runtime_error("Not in current OpenGL Context");\
	}

namespace Game
{
	OpenGlFunctions *OpenGlFunctions::s_Instance = nullptr;

	OpenGlFunctions::OpenGlFunctions(Context &context) : m_Context(&context),
	                                                     m_Internals(MakePointer<Internals>())
	{
		s_Instance = this;
	}

	void OpenGlFunctions::MakeCurrent()
	{
		s_Instance = this;
	}

	OpenGlFunctions::OpenGlFunctions() : m_Context(Context::GetCurrentContext()),
	                                     m_Internals(MakePointer<Internals>()) {}

	OpenGlFunctions::OpenGlFunctions(const OpenGlFunctions &functions) : m_Context(functions.m_Context)
	{
		m_Internals = functions.m_Internals;
	}

	OpenGlFunctions::OpenGlFunctions(OpenGlFunctions &&functions) : m_Context(functions.m_Context)
	{
		std::swap(m_Internals, functions.m_Internals);
	}

	OpenGlFunctions& OpenGlFunctions::operator=(const OpenGlFunctions &functions)
	{
		m_Context   = functions.m_Context;
		m_Internals = functions.m_Internals;

		return *this;
	}

	OpenGlFunctions& OpenGlFunctions::operator=(OpenGlFunctions &&functions)
	{
		std::swap(m_Context, functions.m_Context);
		std::swap(m_Internals, functions.m_Internals);

		return *this;
	}

	OpenGlFunctions& OpenGlFunctions::GetFunctions()
	{
		ASSERT(s_Instance, "Insance of OpenGLFunctions does not exists");

		if(s_Instance->m_Context->IsCurrent())
		{
			ASSERT(false, "Not in current OpenGL Context");
		}

		return *s_Instance;
	}

	void OpenGlFunctions::Clear(const BufferBit buffer)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glClear(static_cast<GLbitfield>(buffer));
	}

	void OpenGlFunctions::Enable(const Capability capability)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		m_Internals->Capabilities[capability] = true;
		glEnable(static_cast<GLenum>(capability));
	}

	void OpenGlFunctions::Disable(const Capability capability)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		m_Internals->Capabilities[capability] = false;
		glDisable(static_cast<GLenum>(capability));
	}

	bool OpenGlFunctions::IsEnabled(const Capability capability) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		const auto iter = m_Internals->Capabilities.find(capability);

		if(iter != m_Internals->Capabilities.end())
			return iter->second;

		m_Internals->Capabilities[capability] = glIsEnabled(static_cast<GLenum>(capability));

		return m_Internals->Capabilities[capability];
	}

	void OpenGlFunctions::SetClearColor(const Color &color)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glClearColor(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f);
	}

	void OpenGlFunctions::SetViewPort(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glViewport(x, y, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}

	void OpenGlFunctions::SetClearColor(const glm::vec4 &color)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		auto &clearColor = m_Internals->ClearColor;

		clearColor = color;

		clearColor.r = std::clamp(color.r, 0.f, 1.f);
		clearColor.g = std::clamp(color.g, 0.f, 1.f);
		clearColor.b = std::clamp(color.b, 0.f, 1.f);
		clearColor.a = std::clamp(color.a, 0.f, 1.f);

		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void OpenGlFunctions::SetClearColor(float red, float green, float blue, float alpha)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		SetClearColor(glm::vec4(red, green, blue, alpha));
	}

	glm::vec4 OpenGlFunctions::GetClearColor() const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return m_Internals->ClearColor;
	}

	void OpenGlFunctions::SetFrontFace(FrontFace face)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		m_Internals->FrontFace = face;
		glFrontFace(static_cast<GLenum>(face));
	}

	FrontFace OpenGlFunctions::GetFrontFace() const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return m_Internals->FrontFace;
	}

	void OpenGlFunctions::SetPolygonFacing(PolygonFacing facing)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		m_Internals->PolygonFacing = facing;
		glCullFace(static_cast<GLenum>(facing));
	}

	PolygonFacing OpenGlFunctions::GetPolygonFacing() const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return m_Internals->PolygonFacing;
	}

	void OpenGlFunctions::SetBlendMode(const BlendMode &blendMode)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glBlendFuncSeparate(
		                    static_cast<GLenum>(blendMode.ColorSrcFactor),
		                    static_cast<GLenum>(blendMode.ColorDstFactor),
		                    static_cast<GLenum>(blendMode.AlphaSrcFactor),
		                    static_cast<GLenum>(blendMode.AlphaDstFactor)
		                   );

		glBlendEquationSeparate(
		                        static_cast<GLenum>(blendMode.ColorEquation),
		                        static_cast<GLenum>(blendMode.AlphaEquation)
		                       );

		m_Internals->BlendMode = blendMode;
	}

	void OpenGlFunctions::SetStencilTest(const StencilTest &stencilTest)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glStencilFuncSeparate(
		                      static_cast<GLenum>(stencilTest.Face),
		                      static_cast<GLenum>(stencilTest.TestFunction),
		                      stencilTest.Ref,
		                      stencilTest.Mask
		                     );

		glStencilOpSeparate(
		                    static_cast<GLenum>(stencilTest.Face),
		                    static_cast<GLenum>(stencilTest.StencilFail),
		                    static_cast<GLenum>(stencilTest.DepthFails),
		                    static_cast<GLenum>(stencilTest.Pass)
		                   );

		m_Internals->StencilTest = stencilTest;
	}

	BlendMode OpenGlFunctions::GetBlendMode() const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return m_Internals->BlendMode;
	}

	StencilTest OpenGlFunctions::GetStencilTest() const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return m_Internals->StencilTest;
	}

	uint32_t OpenGlFunctions::GenTexture()
	{
		CHECK_FOR_CURRENT_CONTEXT();

		uint32_t texture = 0;
		glGenTextures(1, &texture);

		return texture;
	}

	uint32_t* OpenGlFunctions::GenTextures(uint32_t account)
	{
		auto textures = new uint32_t[account];

		GenTextures(account, textures);

		return textures;
	}

	void OpenGlFunctions::GenTextures(uint32_t account, uint32_t *textures)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGenTextures(static_cast<GLsizei>(account), textures);
	}

	void OpenGlFunctions::DeleteTexture(uint32_t texture)
	{
		DeleteTextures(1, &texture);
	}

	void OpenGlFunctions::DeleteTextures(uint32_t account, uint32_t *textures)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glDeleteTextures(static_cast<GLsizei>(account), textures);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParamName name, float param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(name != TextureParamName::SwizzleRGBA || name != TextureParamName::BorderColor);

		glTextureParameterf(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParamName name, int32_t param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(name != TextureParamName::SwizzleRGBA || name != TextureParamName::BorderColor);

		glTextureParameteri(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParamName name, const float *param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glTextureParameterfv(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParamName name, const int32_t *param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glTextureParameteriv(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::GenerateMipMap(uint32_t texture)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGenerateTextureMipmap(texture);
	}

	void OpenGlFunctions::GenerateMipMap(TextureTarget target, uint32_t texture)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(target));
		glBindTexture(static_cast<GLenum>(target), texture);
		glGenerateMipmap(static_cast<GLenum>(target));
	}

	void OpenGlFunctions::Image2DBind(
		uint32_t texture,
		TextureTarget target,
		int32_t level,
		InternalFormat internalFormat,
		uint32_t width,
		uint32_t height,
		int32_t border,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(target));
		glBindTexture(static_cast<GLenum>(target), texture);
		glTexImage2D(
		             static_cast<GLenum>(target),
		             level,
		             static_cast<GLint>(internalFormat),
		             static_cast<GLsizei>(width),
		             static_cast<GLsizei>(height),
		             border,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            );
	}

	void OpenGlFunctions::Image2D(
		uint32_t texture,
		TextureTarget bindTarget,
		TextureTarget target,
		int32_t level,
		InternalFormat internalFormat,
		uint32_t width,
		uint32_t height,
		int32_t border,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(bindTarget));
		glBindTexture(static_cast<GLenum>(bindTarget), texture);

		glTexImage2D(
		             static_cast<GLenum>(target),
		             level,
		             static_cast<GLint>(internalFormat),
		             static_cast<GLsizei>(width),
		             static_cast<GLsizei>(height),
		             border,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            );
	}

	void OpenGlFunctions::SubImage2D(
		uint32_t texture,
		int32_t level,
		int32_t xOffset,
		int32_t yOffset,
		uint32_t width,
		uint32_t height,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glTextureSubImage2D(
		                    texture,
		                    level,
		                    xOffset,
		                    yOffset,
		                    static_cast<GLsizei>(width),
		                    static_cast<GLsizei>(height),
		                    static_cast<GLenum>(format),
		                    static_cast<GLenum>(type),
		                    data
		                   );
	}

	void OpenGlFunctions::Image2DBind(
		uint32_t texture,
		TextureTarget target,
		int32_t level,
		InternalFormat internalFormat,
		const Vector2u &size,
		int32_t border,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(target));
		glBindTexture(static_cast<GLenum>(target), texture);
		glTexImage2D(
		             static_cast<GLenum>(target),
		             level,
		             static_cast<GLint>(internalFormat),
		             static_cast<GLsizei>(size.Width),
		             static_cast<GLsizei>(size.Height),
		             border,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            );
	}

	void OpenGlFunctions::Image2D(
		uint32_t texture,
		TextureTarget bindTarget,
		TextureTarget target,
		int32_t level,
		InternalFormat internalFormat,
		const Vector2u &size,
		int32_t border,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(bindTarget));
		glBindTexture(static_cast<GLenum>(bindTarget), texture);

		glTexImage2D(
		             static_cast<GLenum>(target),
		             level,
		             static_cast<GLint>(internalFormat),
		             static_cast<GLsizei>(size.Width),
		             static_cast<GLsizei>(size.Height),
		             border,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            );
	}

	void OpenGlFunctions::SubImage2D(
		uint32_t texture,
		int32_t level,
		const Vector2i &offset,
		const Vector2u &size,
		Format format,
		DataType type,
		const void *data
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glTextureSubImage2D(
		                    texture,
		                    level,
		                    offset.X,
		                    offset.Y,
		                    static_cast<GLsizei>(size.Width),
		                    static_cast<GLsizei>(size.Height),
		                    static_cast<GLenum>(format),
		                    static_cast<GLenum>(type),
		                    data
		                   );
	}

	void OpenGlFunctions::BindTexture(TextureTarget target, uint32_t texture)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(target));
		glBindTexture(static_cast<GLenum>(target), texture);
	}

	void OpenGlFunctions::BindRenderBuffer(uint32_t buffer)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	}

	void OpenGlFunctions::BindFrameBuffer(uint32_t buffer, bool read)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		if (read)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer);
		else
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);
	}

	void OpenGlFunctions::GetTextureImage(
		uint32_t texture,
		int32_t level,
		Format format,
		DataType type,
		uint32_t bufferSize,
		void *pixels
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetTextureImage(texture, level, static_cast<GLenum>(format), static_cast<GLenum>(type), bufferSize, pixels);
	}

	uint32_t OpenGlFunctions::GenRenderBuffer()
	{
		uint32_t buffer;
		GenRenderBuffers(1, &buffer);

		return buffer;
	}

	uint32_t* OpenGlFunctions::GenRenderBuffers(uint32_t size)
	{
		auto buffers = new uint32_t[size];

		GenRenderBuffers(size, buffers);
		return buffers;
	}

	void OpenGlFunctions::GenRenderBuffers(uint32_t size, uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGenRenderbuffers(static_cast<GLsizei>(size), buffers);
	}

	void OpenGlFunctions::DeleteRenderBuffer(uint32_t buffer)
	{
		DeleteRenderBuffers(1, &buffer);
	}

	void OpenGlFunctions::DeleteRenderBuffers(uint32_t size, uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glDeleteRenderbuffers(static_cast<GLsizei>(size), buffers);
	}

	void OpenGlFunctions::RenderBufferStorage(
		uint32_t renderBuffer,
		InternalFormat internalFormat,
		uint32_t width,
		uint32_t height
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedRenderbufferStorage(
		                           renderBuffer,
		                           static_cast<GLenum>(internalFormat),
		                           static_cast<GLsizei>(width),
		                           static_cast<GLsizei>(height)
		                          );
	}

	void OpenGlFunctions::RenderBufferStorage(
		uint32_t renderBuffer,
		InternalFormat internalFormat,
		const Vector2u &size
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedRenderbufferStorage(
		                           renderBuffer,
		                           static_cast<GLenum>(internalFormat),
		                           static_cast<GLsizei>(size.Width),
		                           static_cast<GLsizei>(size.Height)
		                          );
	}

	void OpenGlFunctions::RenderBufferStorage(
		uint32_t renderBuffer,
		uint32_t samples,
		InternalFormat internalFormat,
		uint32_t width,
		uint32_t height
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedRenderbufferStorageMultisample(
		                                      renderBuffer,
		                                      static_cast<GLsizei>(samples),
		                                      static_cast<GLenum>(internalFormat),
		                                      static_cast<GLsizei>(width),
		                                      static_cast<GLsizei>(height)
		                                     );
	}

	void OpenGlFunctions::RenderBufferStorage(
		uint32_t renderBuffer,
		uint32_t samples,
		InternalFormat internalFormat,
		const Vector2u &size
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedRenderbufferStorageMultisample(
		                                      renderBuffer,
		                                      static_cast<GLsizei>(samples),
		                                      static_cast<GLenum>(internalFormat),
		                                      static_cast<GLsizei>(size.Width),
		                                      static_cast<GLsizei>(size.Height)
		                                     );
	}

	uint32_t OpenGlFunctions::GenFrameBuffer()
	{
		uint32_t buffer;
		GenFrameBuffers(1, &buffer);

		return buffer;
	}

	uint32_t * OpenGlFunctions::GenFrameBuffers(uint32_t size)
	{
		uint32_t *buffers = new uint32_t[size];

		GenFrameBuffers(size, buffers);

		return buffers;
	}

	void OpenGlFunctions::GenFrameBuffers(uint32_t size, uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGenFramebuffers(static_cast<GLsizei>(size), buffers);
	}

	void OpenGlFunctions::DeleteFrameBuffer(uint32_t buffer)
	{
		DeleteFrameBuffers(1, &buffer);
	}

	void OpenGlFunctions::DeleteFrameBuffers(uint32_t size, const uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glDeleteFramebuffers(static_cast<GLsizei>(size), buffers);
	}

	uint32_t OpenGlFunctions::CheckFrameBufferStatus(uint32_t frameBuffer, uint32_t target)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return glCheckNamedFramebufferStatus(frameBuffer, target);
	}

	void OpenGlFunctions::FrameBufferTexture(uint32_t frameBuffer, uint32_t attachment, uint32_t texture, int32_t level)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedFramebufferTexture(frameBuffer, attachment, texture, level);
	}

	void OpenGlFunctions::FrameBufferRenderBuffer(
		uint32_t frameBuffer,
		uint32_t attachment,
		uint32_t renderBufferTarget,
		uint32_t renderBuffer
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glNamedFramebufferRenderbuffer(frameBuffer, attachment, renderBufferTarget, renderBuffer);
	}

	bool OpenGlFunctions::GetBoolean(uint32_t name)
	{
		return GetV<bool>(name);
	}

	double OpenGlFunctions::GetDouble(uint32_t name)
	{
		return GetV<double>(name);
	}

	float OpenGlFunctions::GetFloat(uint32_t name)
	{
		return GetV<float>(name);
	}

	int32_t OpenGlFunctions::GetInteger(uint32_t name)
	{
		return GetV<int32_t>(name);
	}

	int64_t OpenGlFunctions::GetInteger64(uint32_t name)
	{
		return GetV<int64_t>(name);
	}

	bool OpenGlFunctions::GetBoolean(uint32_t name, uint32_t index)
	{
		return GetV<bool>(name, index);
	}

	double OpenGlFunctions::GetDouble(uint32_t name, uint32_t index)
	{
		return GetV<double>(name, index);
	}

	float OpenGlFunctions::GetFloat(uint32_t name, uint32_t index)
	{
		return GetV<float>(name, index);
	}

	int32_t OpenGlFunctions::GetInteger(uint32_t name, uint32_t index)
	{
		return GetV<int32_t>(name, index);
	}

	int64_t OpenGlFunctions::GetInteger64(uint32_t name, uint32_t index)
	{
		return GetV<int64_t>(name, index);
	}

	void OpenGlFunctions::Get(uint32_t name, bool *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		GLboolean b;
		glGetBooleanv(name, &b);

		*data = b == GL_TRUE ? true : false;
	}

	void OpenGlFunctions::Get(uint32_t name, double *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetDoublev(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, float *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetFloatv(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, int32_t *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetIntegerv(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, int64_t *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetInteger64v(name, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, bool *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		GLboolean b;
		glGetBooleani_v(target, index, &b);

		*data = b == GL_TRUE ? true : false;
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, double *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetDoublei_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, float *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetFloati_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, int32_t *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetIntegeri_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, int64_t *data)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetInteger64i_v(target, index, data);
	}

	void OpenGlFunctions::Flush()
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glFlush();
	}

	void OpenGlFunctions::Finish()
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glFinish();
	}

	void OpenGlFunctions::SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		if(!callback)
			return;

		glDebugMessageCallback(callback, userParam);
	}
}
