#pragma once
#include <glad/glad.h>
#include <unordered_map>

#include "Types.h"

#include "Color.h"
#include "Rect.h"

namespace Game
{
	enum class Primitive : uint32_t
	{
		Points = GL_POINTS,
		Lines = GL_LINES,
		LinesStrip = GL_LINE_STRIP,
		LinesLoop = GL_LINE_LOOP,
		LinesStripAdjacency = GL_LINE_STRIP_ADJACENCY,
		LinesAdjacency = GL_LINES_ADJACENCY,
		TrianglesStrip = GL_TRIANGLE_STRIP,
		TrianglesFan = GL_TRIANGLE_FAN,
		Triangles = GL_TRIANGLES,
		TrianglesStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
		TrianglesAdjacency = GL_TRIANGLES_ADJACENCY
	};

	enum class BufferBit : uint32_t
	{
		Color = GL_COLOR_BUFFER_BIT,
		Depth = GL_DEPTH_BUFFER_BIT,
		Stencil = GL_STENCIL_BUFFER_BIT
	};

	enum class Capability : uint32_t
	{
		Blend = GL_BLEND,
		ClipDistance0 = GL_CLIP_DISTANCE0,
		ClipDistance1 = GL_CLIP_DISTANCE1,
		ClipDistance2 = GL_CLIP_DISTANCE2,
		ClipDistance3 = GL_CLIP_DISTANCE3,
		ClipDistance4 = GL_CLIP_DISTANCE4,
		ClipDistance5 = GL_CLIP_DISTANCE5,
		ClipDistance6 = GL_CLIP_DISTANCE6,
		ClipDistance7 = GL_CLIP_DISTANCE7,
		ColorLogicOp = GL_COLOR_LOGIC_OP,
		CullFace = GL_CULL_FACE,
		DebugOutput = GL_DEBUG_OUTPUT,
		DebugOutputSynchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS,
		DepthClamp = GL_DEPTH_CLAMP,
		DepthTest = GL_DEPTH_TEST,
		Dither = GL_DITHER,
		FramebufferSrgb = GL_FRAMEBUFFER_SRGB,
		LineSmooth = GL_LINE_SMOOTH,
		Multisample = GL_MULTISAMPLE,
		PolygonSmooth = GL_POLYGON_SMOOTH,
		PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,
		PolygonOffsetLine = GL_POLYGON_OFFSET_LINE,
		PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT,
		ProgramPointSize = GL_PROGRAM_POINT_SIZE,
		PrimitiveRestart = GL_PRIMITIVE_RESTART,
		PrimitiveRestartFixedIndex = GL_PRIMITIVE_RESTART_FIXED_INDEX,
		RasterizerDiscard = GL_RASTERIZER_DISCARD,
		SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
		SampleAlphaToOne = GL_SAMPLE_ALPHA_TO_ONE,
		SampleCoverage = GL_SAMPLE_COVERAGE,
		SampleShading = GL_SAMPLE_SHADING,
		SampleMask = GL_SAMPLE_MASK,
		ScissorTest = GL_SCISSOR_TEST,
		StencilTest = GL_STENCIL_TEST,
		TextureCubeMapSeamless = GL_TEXTURE_CUBE_MAP_SEAMLESS,
	};

	enum class TestFunction
	{
		Never = GL_NEVER,
		Less = GL_LESS,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		GreaterEqual = GL_GEQUAL,
		Equal = GL_EQUAL,
		NotEqual = GL_NOTEQUAL,
		Always = GL_ALWAYS
	};

	enum class StencilAction
	{
		Keep = GL_KEEP,
		Zero = GL_ZERO,
		Replace = GL_REPLACE,
		Increase = GL_INCR,
		IncreaseWrap = GL_INCR_WRAP,
		Decrease = GL_DECR,
		DecreaseWrap = GL_DECR_WRAP,
		Invert = GL_INVERT
	};

	enum class FrontFace : uint32_t
	{
		ClockWise = GL_CW,
		CounterClockWise = GL_CCW
	};

	enum class PolygonFacing : uint32_t
	{
		Front = GL_FRONT,
		Back = GL_BACK,
		FrontBack = GL_FRONT_AND_BACK,
	};

	inline BufferBit operator|(const BufferBit &left, const BufferBit &right)
	{
		return static_cast<BufferBit>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
	}


	struct BlendMode
	{
		enum Factor
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

		enum Equation
		{
			Add = GL_FUNC_ADD,
			Subtract = GL_FUNC_SUBTRACT,
			ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT
		};

		BlendMode() = default;

		BlendMode(
			Factor sourceFactor,
			Factor destinationFactor,
			Equation blendEquation = Add
			) : ColorSrcFactor(sourceFactor),
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

		Factor ColorSrcFactor  = SrcAlpha;
		Factor ColorDstFactor  = OneMinusSrcAlpha;
		Equation ColorEquation = Add;
		Factor AlphaSrcFactor  = One;
		Factor AlphaDstFactor  = OneMinusSrcAlpha;
		Equation AlphaEquation = Add;
	};

	class Context;

	class OpenGlFunctions
	{
		friend class Context;

		std::unordered_map<Capability, bool> m_Capabilities;

		OpenGlFunctions() = default;
	public:
		void Clear(BufferBit buffer);

		void Enable(Capability capability);
		void Disable(Capability capability);

		bool IsEnabled(Capability capability);

		void SetClearColor(const Color &color);
		void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void SetViewPort(const UIntRect &viewPort)
		{
			return SetViewPort(viewPort.X, viewPort.Y, viewPort.Width, viewPort.Height);
		}

		void SetBlendMode(const BlendMode& mode);

		void Flush();

		void SetDebugMessageCallback(GLDEBUGPROC callback, const void* userParam);
	};
}
