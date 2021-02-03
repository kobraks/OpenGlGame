#pragma once
#include <glad/glad.h>
#include <unordered_map>

#include "Types.h"

#include "Color.h"
#include "Rect.h"

#include "GLEnums.h"

namespace Game
{
	class Context;

	struct BlendMode
	{
		enum class Factor : uint32_t
		{
			Zero = GL_ZERO,
			One = GL_ONE,
			SrcColor = GL_SRC_COLOR,
			OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
			DstColor = GL_DST_COLOR,
			OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
			SrcAlpha = GL_SRC_ALPHA,
			OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
			DstAlpha = GL_DST_ALPHA,
			OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA
		};

		enum class Equation : uint32_t
		{
			Add = GL_FUNC_ADD,
			Subtract = GL_FUNC_SUBTRACT,
			ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT
		};

		BlendMode() = default;

		BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Equation::Add) : ColorSrcFactor(sourceFactor),
		                                                                                                   ColorDstFactor(destinationFactor),
		                                                                                                   ColorEquation(blendEquation),
		                                                                                                   AlphaSrcFactor(sourceFactor),
		                                                                                                   AlphaDstFactor(destinationFactor),
		                                                                                                   AlphaEquation(blendEquation) {}

		BlendMode(
			Factor colorSourceFactor,
			Factor colorDestinationFactor,
			Equation colorBlendEquation,
			Factor alphaSourceFactor,
			Factor alphaDestinationFactor,
			Equation alphaBlendEquation
			) : ColorSrcFactor(colorSourceFactor),
			    ColorDstFactor(colorDestinationFactor),
			    ColorEquation(colorBlendEquation),
			    AlphaSrcFactor(alphaSourceFactor),
			    AlphaDstFactor(alphaDestinationFactor),
			    AlphaEquation(alphaBlendEquation) {}

		Factor ColorSrcFactor  = Factor::SrcAlpha;
		Factor ColorDstFactor  = Factor::OneMinusSrcAlpha;
		Equation ColorEquation = Equation::Add;
		Factor AlphaSrcFactor  = Factor::One;
		Factor AlphaDstFactor  = Factor::OneMinusSrcAlpha;
		Equation AlphaEquation = Equation::Add;
	};

	class OpenGlFunctions
	{
		friend Context;
		
		Context &m_Context;

		std::unordered_map<Capability, bool> m_Capabilities;

		static OpenGlFunctions *s_Instance;

		OpenGlFunctions(Context &context);
		void MakeCurrent();
		static OpenGlFunctions& GetFunctions();
	public:
		void Clear(BufferBit buffer);

		void Enable(Capability capability);
		void Disable(Capability capability);

		bool IsEnabled(Capability capability);

		void SetClearColor(const Color &color);
		void SetViewPort(int32_t x, int32_t y, uint32_t width, uint32_t height);

		void SetViewPort(const UIntRect &viewPort)
		{
			return SetViewPort(static_cast<int32_t>(viewPort.X), static_cast<int32_t>(viewPort.Y), viewPort.Width, viewPort.Height);
		}

		void SetViewPort(const Vector2i &pos, const Vector2u &size)
		{
			return SetViewPort(pos.X, pos.Y, size.Width, size.Height);
		}

		void SetBlendMode(const BlendMode &mode);

		void Flush();

		void SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam);

		Context& GetContext() const { return m_Context; }
	};
}
