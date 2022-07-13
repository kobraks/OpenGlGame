#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Rect.h"
#include "GLEnums.h"

#include <glad/glad.h>
#include <unordered_map>

namespace Game
{
	class Context;

	struct StencilTest
	{
		enum class Operation : uint32_t
		{
			Keep = GL_KEEP,
			Zero = GL_ZERO,
			Replace = GL_REPLACE,
			Incr = GL_INCR,
			Decr = GL_DECR,
			Invert = GL_INVERT,
			IncrWrap = GL_INCR_WRAP,
			DecrWarp = GL_DECR_WRAP
		};

		enum class Function : uint32_t
		{
			Never = GL_NEVER,
			// <
			Less = GL_LESS,
			//>
			Greater = GL_GREATER,
			// ==
			Equal = GL_EQUAL,
			Always = GL_ALWAYS,
			//<=
			LessEqual = GL_LEQUAL,
			//>=
			GreaterEqual = GL_GEQUAL,
			//!=
			NotEqual = GL_NOTEQUAL
		};

		PolygonFacing Face    = PolygonFacing::FrontBack;
		Operation StencilFail = Operation::Keep;
		Operation DepthFails  = Operation::Keep;
		Operation Pass        = Operation::Keep;

		uint32_t Mask         = 0xFF;
		Function TestFunction = Function::Always;
		int Ref               = 1;

		StencilTest() = default;
		
		StencilTest(uint32_t mask, int ref, Function function, Operation stencilFail, Operation depthFails, Operation pass) : StencilFail(stencilFail),
			DepthFails(depthFails),
			Pass(pass),
			Mask(mask),
			TestFunction(function),
			Ref(ref) {}

		StencilTest(PolygonFacing face, uint32_t mask, int ref, Function function, Operation stencilFail, Operation depthFails, Operation pass) : Face(face),
			StencilFail(stencilFail),
			DepthFails(depthFails),
			Pass(pass),
			Mask(mask),
			TestFunction(function),
			Ref(ref) {}


		constexpr bool operator== (const StencilTest& test) const
		{
			return Face == test.Face &&
				StencilFail == test.StencilFail &&
				DepthFails == test.DepthFails &&
				Pass == test.Pass &&
				Mask == test.Mask &&
				TestFunction == test.TestFunction &&
				Ref == test.Ref;
		}

		constexpr bool operator!= (const StencilTest& test) const
		{
			return !(*this == test);
		}
	};

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

		constexpr bool operator == (const BlendMode& mode) const
		{
			return ColorSrcFactor == mode.ColorSrcFactor && 
				ColorDstFactor == mode.ColorDstFactor &&
				ColorEquation == mode.ColorEquation &&
				AlphaSrcFactor == mode.AlphaSrcFactor &&
				AlphaDstFactor == mode.AlphaDstFactor &&
				AlphaEquation == mode.AlphaEquation;
		}

		constexpr bool operator != (const BlendMode& mode) const
		{
			return !(*this == mode);
		}
	};

	class OpenGlFunctions
	{
		friend Context;

		Context &m_Context;

		struct Internals
		{
			std::unordered_map<Capability, bool> Capabilities;

			FrontFace FrontFace = FrontFace::CounterClockWise;

			PolygonFacing PolygonFacing = PolygonFacing::Back;

			glm::vec4 ClearColor = glm::vec4(0.f, 0.f, 0.f, 0.f);

			StencilTest StencilTest;
			BlendMode BlendMode;
		};

		Pointer<Internals> m_Internals;

		static OpenGlFunctions *s_Instance;

		OpenGlFunctions(Context &context);
		void MakeCurrent();
	public:
		static OpenGlFunctions& GetFunctions();
		
		void Clear(BufferBit buffer);

		void Enable(Capability capability);
		void Disable(Capability capability);

		bool IsEnabled(Capability capability) const;

		void SetClearColor(const Color &color);
		void SetViewPort(int32_t x, int32_t y, uint32_t width, uint32_t height);

		void SetClearColor(const glm::vec4 &color);
		void SetClearColor(float red, float green, float blue, float alpha);

		glm::vec4 GetClearColor() const;

		void SetFrontFace(FrontFace face);
		FrontFace GetFrontFace() const;

		void SetPolygonFacing(PolygonFacing facing);
		PolygonFacing GetPolygonFacing() const;

		void SetViewPort(const UIntRect &viewPort)
		{
			return SetViewPort(static_cast<int32_t>(viewPort.X), static_cast<int32_t>(viewPort.Y), viewPort.Width, viewPort.Height);
		}

		void SetViewPort(const Vector2i &pos, const Vector2u &size)
		{
			return SetViewPort(pos.X, pos.Y, size.Width, size.Height);
		}

		void SetBlendMode(const BlendMode &blendMode);
		void SetStencilTest(const StencilTest &stencilTest);

		BlendMode GetBlendMode() const;
		StencilTest GetStencilTest() const;

		void Flush();
		void Finish();

		void SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam);

		Context& GetContext() const { return m_Context; }
	};
}
