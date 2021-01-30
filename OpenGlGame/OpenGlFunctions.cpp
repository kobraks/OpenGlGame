#include "pch.h"
#include "OpenGlFunctions.h"

#include "GLCheck.h"

namespace Game
{
	void OpenGlFunctions::Clear(const BufferBit buffer)
	{
		GL_CHECK(glClear(static_cast<GLbitfield>(buffer)));
	}
	void OpenGlFunctions::Enable(const Capability capability)
	{
		m_Capabilities[capability] = true;
		GL_CHECK(glEnable(static_cast<GLenum>(capability)));
	}
	void OpenGlFunctions::Disable(const Capability capability)
	{
		m_Capabilities[capability] = false;
		GL_CHECK(glDisable(static_cast<GLenum>(capability)));
	}
	
	bool OpenGlFunctions::IsEnabled(const Capability capability)
	{
		const auto iter = m_Capabilities.find(capability);

		if (iter != m_Capabilities.end())
			return iter->second;

		return m_Capabilities[capability] = glIsEnabled(static_cast<GLenum>(capability));
	}

	void OpenGlFunctions::SetClearColor(const Color &color)
	{
		GL_CHECK(glClearColor(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f));
	}
	
	void OpenGlFunctions::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		GL_CHECK(glViewport(x, y, width, height));
	}

	void OpenGlFunctions::SetBlendMode(const BlendMode &mode)
	{
		glBlendFuncSeparate(mode.ColorSrcFactor, mode.ColorDstFactor, mode.AlphaSrcFactor, mode.AlphaDstFactor);
		glBlendEquationSeparate(mode.ColorEquation, mode.AlphaEquation);
	}

	void OpenGlFunctions::Flush()
	{
		glFlush();
	}

	void OpenGlFunctions::SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam)
	{
		glDebugMessageCallback(callback, userParam);
	}
}
