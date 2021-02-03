#include "pch.h"
#include "OpenGlFunctions.h"

#include "GLCheck.h"
#include "Context.h"

#define CHECK_IF_VALID_CONTEXT()\
	if(!m_Context.IsContextCurrent())\
	{\
		ASSERT(false, "Not current OpenGl context");\
		throw std::runtime_error("Not current OpenGl context");\
	}

namespace Game
{
	OpenGlFunctions* OpenGlFunctions::s_Instance = nullptr;
	
	OpenGlFunctions::OpenGlFunctions(Context &context) : m_Context(context)
	{
		s_Instance = this;
	}

	void OpenGlFunctions::MakeCurrent()
	{
		s_Instance = this;
	}

	OpenGlFunctions & OpenGlFunctions::GetFunctions()
	{
		return *s_Instance;
	}

	void OpenGlFunctions::Clear(const BufferBit buffer)
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glClear(static_cast<GLbitfield>(buffer)));
	}

	void OpenGlFunctions::Enable(const Capability capability)
	{
		CHECK_IF_VALID_CONTEXT();

		m_Capabilities[capability] = true;
		GL_CHECK(glEnable(static_cast<GLenum>(capability)));
	}

	void OpenGlFunctions::Disable(const Capability capability)
	{
		CHECK_IF_VALID_CONTEXT();

		m_Capabilities[capability] = false;
		GL_CHECK(glDisable(static_cast<GLenum>(capability)));
	}

	bool OpenGlFunctions::IsEnabled(const Capability capability)
	{
		CHECK_IF_VALID_CONTEXT();

		const auto iter = m_Capabilities.find(capability);

		if(iter != m_Capabilities.end())
			return iter->second;

		GL_CHECK(m_Capabilities[capability] = glIsEnabled(static_cast<GLenum>(capability)));

		return m_Capabilities[capability];
	}

	void OpenGlFunctions::SetClearColor(const Color &color)
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glClearColor(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f));
	}

	void OpenGlFunctions::SetViewPort(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glViewport(x, y, width, height));
	}

	void OpenGlFunctions::SetBlendMode(const BlendMode &mode)
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(
		         glBlendFuncSeparate(static_cast<GLenum>(mode.ColorSrcFactor), static_cast<GLenum>(mode.ColorDstFactor), static_cast<GLenum>(mode.AlphaSrcFactor
		         ), static_cast<GLenum>(mode.AlphaDstFactor))
		        );
		GL_CHECK(glBlendEquationSeparate(static_cast<GLenum>(mode.ColorEquation), static_cast<GLenum>(mode.AlphaEquation)));
	}

	void OpenGlFunctions::Flush()
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glFlush());
	}

	void OpenGlFunctions::SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam)
	{
		if(!callback)
			return;

		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glDebugMessageCallback(callback, userParam));
	}
}
