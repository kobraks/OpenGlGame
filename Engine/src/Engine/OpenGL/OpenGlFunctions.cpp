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

	OpenGlFunctions::OpenGlFunctions(OpenGlFunctions &&functions) noexcept : m_Context(functions.m_Context)
	{
		std::swap(m_Internals, functions.m_Internals);
	}

	OpenGlFunctions& OpenGlFunctions::operator=(const OpenGlFunctions &functions) = default;

	OpenGlFunctions& OpenGlFunctions::operator=(OpenGlFunctions &&functions) noexcept
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

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParameterName name, float param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(name != TextureParameterName::SwizzleRGBA || name != TextureParameterName::BorderColor);

		glTextureParameterf(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParameterName name, int32_t param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(name != TextureParameterName::SwizzleRGBA || name != TextureParameterName::BorderColor);

		glTextureParameteri(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParameterName name, const float *param)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glTextureParameterfv(texture, static_cast<GLenum>(name), param);
	}

	void OpenGlFunctions::TextureParameter(uint32_t texture, TextureParameterName name, const int32_t *param)
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

	void OpenGlFunctions::TexStorage2D(
		uint32_t texture,
		uint32_t levels,
		InternalFormat internalFormat,
		uint32_t width,
		uint32_t height
		)
	{
		glTextureStorage2D(texture, static_cast<GLsizei>(levels), static_cast<GLenum>(internalFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}

	void OpenGlFunctions::TexStorage2D(
		uint32_t texture,
		uint32_t levels,
		InternalFormat internalFormat,
		const Vector2u &size
		)
	{
		glTextureStorage2D(texture, static_cast<GLsizei>(levels), static_cast<GLenum>(internalFormat), static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
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

	void OpenGlFunctions::BindTexture(TextureTarget target, uint32_t texture) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		ASSERT(IsBindable(target));
		glBindTexture(static_cast<GLenum>(target), texture);
	}

	void OpenGlFunctions::BindRenderBuffer(uint32_t buffer) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	}

	void OpenGlFunctions::BindFrameBuffer(uint32_t buffer, bool read) const
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
		) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetTextureImage(texture, level, static_cast<GLenum>(format), static_cast<GLenum>(type), static_cast<GLsizei>(bufferSize), pixels);
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

	uint32_t OpenGlFunctions::CreateShader(uint32_t shaderType)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		return glCreateShader(shaderType);
	}

	void OpenGlFunctions::DeleteShader(uint32_t shader)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glDeleteShader(shader);
	}

	void OpenGlFunctions::ShaderSource(
		uint32_t shader,
		uint32_t count,
		const char* const *string,
		const int32_t *length
		)
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glShaderSource(shader, static_cast<GLsizei>(count), string, length);
	}

	void OpenGlFunctions::ShaderSource(
		uint32_t shader,
		uint32_t count,
		const std::string *string,
		const int32_t *length
		)
	{
		for (uint32_t i = 0; i < count; ++i)
			ShaderSource(shader, string[i]);
	}

	void OpenGlFunctions::ShaderSource(uint32_t shader, uint32_t count, const std::string_view *string)
	{
		for (uint32_t i = 0; i < count; ++i)
			ShaderSource(shader, string[i]);
	}

	void OpenGlFunctions::ShaderSource(uint32_t shader, const std::string_view &string)
	{
		const int32_t length = static_cast<int32_t>(string.size());
		const char* cstr = string.data();

		ShaderSource(shader, 1, &cstr, &length);
	}

	void OpenGlFunctions::ShaderSource(uint32_t shader, const std::string &string)
	{
		const char* cstr = string.c_str();

		ShaderSource(shader, 1, &cstr, nullptr);
	}

	void OpenGlFunctions::ShaderSource(uint32_t shader, const char *string, int32_t length)
	{
		ShaderSource(shader, 1, &string, &length);
	}

	void OpenGlFunctions::ShaderSource(uint32_t shader, const char *string)
	{
		ShaderSource(shader, 1, &string, nullptr);
	}

	void OpenGlFunctions::CompileShader(uint32_t shader)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glCompileShader(shader);
	}

	void OpenGlFunctions::GetShader(uint32_t shader, ShaderParameterName name, int32_t *params) const
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glGetShaderiv(shader, static_cast<GLenum>(name), params);
	}

	int32_t OpenGlFunctions::GetShader(uint32_t shader, ShaderParameterName name) const
	{
		int32_t v = 0;

		GetShader(shader, name, &v);

		return v;
	}

	std::string OpenGlFunctions::ShaderInfoLog(uint32_t shader)
	{
		int32_t length = GetShader(shader, ShaderParameterName::LogLength);

		if (length > 0)
		{
			std::string log(length + 1, 0);

			glGetShaderInfoLog(shader, length + 1, nullptr, &log[0]);

			return log;
		}

		return "";
	}

	uint32_t OpenGlFunctions::CreateBuffer()
	{
		uint32_t id = 0;
		CreateBuffers(1, &id);

		return id;
	}

	uint32_t * OpenGlFunctions::CreateBuffers(uint32_t count)
	{
		uint32_t* buffers = new uint32_t[count];

		CreateBuffers(count, buffers);

		return buffers;
	}

	void OpenGlFunctions::CreateBuffers(uint32_t count, uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glCreateBuffers(static_cast<GLsizei>(count), buffers);
	}

	void OpenGlFunctions::DeleteBuffer(uint32_t buffer)
	{
		DeleteBuffers(1, &buffer);
	}
	void OpenGlFunctions::DeleteBuffers(uint32_t count, uint32_t *buffers)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glDeleteBuffers(static_cast<GLsizei>(count), buffers);
	}

	void OpenGlFunctions::BufferData(uint32_t buffer, BufferUsage usage, size_t size, const void *data)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glNamedBufferData(buffer, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
	}
	void OpenGlFunctions::BufferSubData(uint32_t buffer, size_t offset, size_t size, const void *data)
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glNamedBufferSubData(buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
	}

	void OpenGlFunctions::BindBuffer(uint32_t buffer, BufferType type) const
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glBindBuffer(static_cast<GLenum>(type), buffer);
	}

	void * OpenGlFunctions::MapBuffer(uint32_t buffer, BufferAccess access) const
	{
		CHECK_FOR_CURRENT_CONTEXT()

		return glMapNamedBuffer(buffer, static_cast<GLenum>(access));
	}
	void OpenGlFunctions::UnMapBuffer(uint32_t buffer) const
	{
		CHECK_FOR_CURRENT_CONTEXT()

		glUnmapNamedBuffer(buffer);
	}

	std::string OpenGlFunctions::GetString(uint32_t name) const
	{
		return std::string(reinterpret_cast<const char*>(glGetString(name)));
	}

	std::string OpenGlFunctions::GetString(uint32_t name, uint32_t index) const
	{
		return std::string(reinterpret_cast<const char*>(glGetStringi(name, index)));
	}

	std::string_view OpenGlFunctions::GetStringView(uint32_t name) const
	{
		return std::string_view(reinterpret_cast<const char*>(glGetString(name)));
	}

	std::string_view OpenGlFunctions::GetStringView(uint32_t name, uint32_t index) const
	{
		return std::string_view(reinterpret_cast<const char*>(glGetStringi(name, index)));
	}

	void OpenGlFunctions::Get(uint32_t name, bool *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		GLboolean b;
		glGetBooleanv(name, &b);

		*data = b == GL_TRUE ? true : false;
	}

	void OpenGlFunctions::Get(uint32_t name, double *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetDoublev(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, float *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetFloatv(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, int32_t *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetIntegerv(name, data);
	}

	void OpenGlFunctions::Get(uint32_t name, int64_t *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetInteger64v(name, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, bool *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		GLboolean b;
		glGetBooleani_v(target, index, &b);

		*data = b == GL_TRUE ? true : false;
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, double *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetDoublei_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, float *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetFloati_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, int32_t *data) const
	{
		CHECK_FOR_CURRENT_CONTEXT();

		glGetIntegeri_v(target, index, data);
	}

	void OpenGlFunctions::Get(uint32_t target, uint32_t index, int64_t *data) const
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
