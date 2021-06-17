#include "pch.h"
#include "OpenGlFunctions.h"

#include "GLCheck.h"
#include "Context.h"

#define CHECK_IF_VALID_CONTEXT()\
	if(!m_Context.IsContextCurrent())\
	{\
		ASSERT(false, "Not current OpenGl context");\
	}

namespace Game
{
	OpenGlFunctions *OpenGlFunctions::s_Instance = nullptr;

	OpenGlFunctions::OpenGlFunctions(Context &context) : m_Context(context), m_Internals(MakePointer<Internals>())
	{
		s_Instance = this;
	}

	void OpenGlFunctions::MakeCurrent()
	{
		s_Instance = this;
	}

	OpenGlFunctions& OpenGlFunctions::GetFunctions()
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

		m_Internals->Capabilities[capability] = true;
		GL_CHECK(glEnable(static_cast<GLenum>(capability)));
	}

	void OpenGlFunctions::Disable(const Capability capability)
	{
		CHECK_IF_VALID_CONTEXT();

		m_Internals->Capabilities[capability] = false;
		GL_CHECK(glDisable(static_cast<GLenum>(capability)));
	}

	bool OpenGlFunctions::IsEnabled(const Capability capability) const
	{
		CHECK_IF_VALID_CONTEXT();

		const auto iter = m_Internals->Capabilities.find(capability);

		if(iter != m_Internals->Capabilities.end())
			return iter->second;

		GL_CHECK(m_Internals->Capabilities[capability] = glIsEnabled(static_cast<GLenum>(capability)));

		return m_Internals->Capabilities[capability];
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

	void OpenGlFunctions::SetClearColor(const glm::vec4&color)
	{
		CHECK_IF_VALID_CONTEXT();

		auto& clearColor = m_Internals->ClearColor;
		
		clearColor = color;

		clearColor.r = std::clamp(color.r, 0.f, 1.f);
		clearColor.g = std::clamp(color.g, 0.f, 1.f);
		clearColor.b = std::clamp(color.b, 0.f, 1.f);
		clearColor.a = std::clamp(color.a, 0.f, 1.f);

		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}
	
	void OpenGlFunctions::SetClearColor(float red, float green, float blue, float alpha)
	{
		SetClearColor(glm::vec4(red, green, blue, alpha));
	}
	
	glm::vec4 OpenGlFunctions::GetClearColor() const
	{
		CHECK_IF_VALID_CONTEXT();
		
		return m_Internals->ClearColor;
	}

	void OpenGlFunctions::SetFrontFace(FrontFace face)
	{
		CHECK_IF_VALID_CONTEXT();
		
		m_Internals->FrontFace = face;
		GL_CHECK(glFrontFace(static_cast<GLenum>(face)));
	}

	FrontFace OpenGlFunctions::GetFrontFace() const
	{
		CHECK_IF_VALID_CONTEXT();
		
		return m_Internals->FrontFace;
	}

	void OpenGlFunctions::SetPolygonFacing(PolygonFacing facing)
	{
		CHECK_IF_VALID_CONTEXT();
		
		m_Internals->PolygonFacing = facing;
		GL_CHECK(glCullFace(static_cast<GLenum>(facing)));
	}

	PolygonFacing OpenGlFunctions::GetPolygonFacing() const
	{
		CHECK_IF_VALID_CONTEXT();
		
		return m_Internals->PolygonFacing;
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

	void OpenGlFunctions::SetStencilTest(const StencilTest &test)
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glStencilFuncSeparate(static_cast<GLenum>(test.Face), static_cast<GLenum>(test.TestFunction), test.Ref, test.Mask));
		GL_CHECK(glStencilOpSeparate(static_cast<GLenum>(test.Face), static_cast<GLenum>(test.StencilFail), static_cast<GLenum>(test.DepthFails), static_cast<GLenum>(test.Pass)));
	}

	void OpenGlFunctions::Flush()
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glFlush());
	}

	void OpenGlFunctions::Finish()
	{
		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glFinish());
	}

	void OpenGlFunctions::SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam)
	{
		if(!callback)
			return;

		CHECK_IF_VALID_CONTEXT();

		GL_CHECK(glDebugMessageCallback(callback, userParam));
	}
}
