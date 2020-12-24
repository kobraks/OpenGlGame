#pragma once
#include <glad/glad.h>
#include <unordered_map>

#include "Types.h"

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
		ColorLogicOp = GL_COLOR_LOGIC_OP,
		CullFace = GL_CULL_FACE,
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
		SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
		SampleAlphaToOne = GL_SAMPLE_ALPHA_TO_ONE,
		SampleCoverage = GL_SAMPLE_COVERAGE,
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

	class Context;
	
	class OpenGlFunctions
	{
		friend class Context;
		
		std::unordered_map<Capability, bool> m_Capabilities;

		OpenGlFunctions() = default;
	public:
		void Clear(const BufferBit buffer);
		
		void Enable(const Capability capability);
		void Disable(const Capability capability);

		bool Enabled(const Capability capability);
	};
}
