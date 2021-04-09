#pragma once

#include <glad/glad.h>

namespace Game
{
	enum class UniformType : GLenum
	{
		Float = GL_FLOAT,
		Vec2 = GL_FLOAT_VEC2,
		Vec3 = GL_FLOAT_VEC3,
		Vec4 = GL_FLOAT_VEC4,
		
		Double = GL_DOUBLE,
		DVec2 = GL_DOUBLE_VEC2,
		DVec3 = GL_DOUBLE_VEC3,
		DVec4 = GL_DOUBLE_VEC4,
		
		Int = GL_INT,
		IVec2 = GL_INT_VEC2,
		IVec3 = GL_INT_VEC3,
		IVec4 = GL_INT_VEC4,

		UInt = GL_UNSIGNED_INT,
		UVec2 = GL_UNSIGNED_INT_VEC2,
		UVec3 = GL_UNSIGNED_INT_VEC3,
		UVec4 = GL_UNSIGNED_INT_VEC4,

		Bool = GL_BOOL,
		BVec2 = GL_BOOL_VEC2,
		BVec3 = GL_BOOL_VEC3,
		BVec4 = GL_BOOL_VEC4,

		Mat2 = GL_FLOAT_MAT2,	
		Mat2x3 = GL_FLOAT_MAT2x3,	
		Mat2x4 = GL_FLOAT_MAT2x4,
		
		Mat3 = GL_FLOAT_MAT3,	
		Mat3x2 = GL_FLOAT_MAT3x2,	
		Mat3x4 = GL_FLOAT_MAT3x4,
		
		Mat4 = GL_FLOAT_MAT4,	
		Mat4x2 = GL_FLOAT_MAT4x2,	
		Mat4x3 = GL_FLOAT_MAT4x3,

		Sampler1D = GL_SAMPLER_1D,
		Sampler2D = GL_SAMPLER_2D,
		Sampler3D = GL_SAMPLER_3D
	};
	
	enum class InternalFormat : int32_t
	{
		CompressedRed = GL_COMPRESSED_RED,
		CompressedRedRGTC1 = GL_COMPRESSED_RED_RGTC1,
		CompressedRG = GL_COMPRESSED_RG,
		CompressedRGB = GL_COMPRESSED_RGB,
		CompressedRGBA = GL_COMPRESSED_RGBA,
		CompressedRGRGTC2 = GL_COMPRESSED_RG_RGTC2,
		CompressedSignedRedRGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
		CompressedSignedRGRGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
		CompressedSRGB = GL_COMPRESSED_SRGB,
		DepthStencil = GL_DEPTH_STENCIL,
		Depth24Stencil8 = GL_DEPTH24_STENCIL8,
		Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthComponent16 = GL_DEPTH_COMPONENT16,
		DepthComponent24 = GL_DEPTH_COMPONENT24,
		DepthComponent32F = GL_DEPTH_COMPONENT32F,
		R16F = GL_R16F,
		R16I = GL_R16I,
		R16SNorm = GL_R16_SNORM,
		R16UI = GL_R16UI,
		R32F = GL_R32F,
		R32I = GL_R32I,
		R32UI = GL_R32UI,
		R3G3B2 = GL_R3_G3_B2,
		R8 = GL_R8,
		R8I = GL_R8I,
		R8SNorm = GL_R8_SNORM,
		R8UI = GL_R8UI,
		Red = GL_RED,
		RG = GL_RG,
		RG16 = GL_RG16,
		RG16F = GL_RG16F,
		RG16SNorm = GL_RG16_SNORM,
		RG32F = GL_RG32F,
		RG32I = GL_RG32I,
		RG32UI = GL_RG32UI,
		RG8 = GL_RG8,
		RG8I = GL_RG8I,
		RG8SNorm = GL_RG8_SNORM,
		RG8UI = GL_RG8UI,
		RGB = GL_RGB,
		RGB10 = GL_RGB10,
		RGB10A2 = GL_RGB10_A2,
		RGB12 = GL_RGB12,
		RGB16 = GL_RGB16,
		RGB16F = GL_RGB16F,
		RGB16I = GL_RGB16I,
		RGB16UI = GL_RGB16UI,
		RGB32F = GL_RGB32F,
		RGB32I = GL_RGB32I,
		RGB32UI = GL_RGB32UI,
		RGB4 = GL_RGB4,
		RGB5 = GL_RGB5,
		RGB5A1 = GL_RGB5_A1,
		RGB8 = GL_RGB8,
		RGB8I = GL_RGB8I,
		RGB8UI = GL_RGB8UI,
		RGB9E5 = GL_RGB9_E5,
		RGBA = GL_RGBA,
		RGBA12 = GL_RGBA12,
		RGBA16 = GL_RGBA16,
		RGBA16F = GL_RGBA16F,
		RGBA16I = GL_RGBA16I,
		RGBA16UI = GL_RGBA16UI,
		RGBA2 = GL_RGBA2,
		RGBA32F = GL_RGBA32F,
		RGBA32I = GL_RGBA32I,
		RGBA32UI = GL_RGBA32UI,
		RGBA4 = GL_RGBA4,
		RGBA8 = GL_RGBA8,
		RGBA8UI = GL_RGBA8UI,
		SRGB8 = GL_SRGB8,
		SRGB8A8 = GL_SRGB8_ALPHA8,
		SRGBA = GL_SRGB_ALPHA
	};

	enum class Format : uint32_t
	{
		Red = GL_RED,
		Rg = GL_RG,
		Rgb = GL_RGB,
		Bgr = GL_BGR,
		Rgba = GL_RGBA,
		Bgra = GL_BGRA,

		RedInteger = GL_RED_INTEGER,
		RgInteger = GL_RG_INTEGER,
		RgbInteger = GL_RGB_INTEGER,
		BgrInteger = GL_BGR_INTEGER,
		RgbaInteger = GL_RGBA_INTEGER,
		BgraInteger = GL_BGRA_INTEGER,

		StencilIndex = GL_STENCIL_INDEX,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthStencil = GL_DEPTH_STENCIL
	};

	enum class DataType : uint32_t
	{
		Byte = GL_BYTE,
		UnsignedByte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		UnsignedShort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		UnsignedInt = GL_UNSIGNED_INT,
		Float = GL_FLOAT,
		Double = GL_DOUBLE,

		UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,
		UnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
		UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
		UnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5,
		UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
		UnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
		UnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,
		UnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,
		UnsignedInt101010102 = GL_UNSIGNED_INT_10_10_10_2
	};

	enum class Wrapping
	{
		ClampEdge = GL_CLAMP_TO_EDGE,
		ClampBorder = GL_CLAMP_TO_BORDER,
		Repeat = GL_REPEAT,
		MirroredRepeat = GL_MIRRORED_REPEAT
	};

	enum class Filter
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

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

	enum class TestFunction : uint32_t
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

	enum class StencilAction : uint32_t
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

	enum class BufferUsage : uint32_t
	{
		StreamDraw = GL_STREAM_DRAW,
		StreamRead = GL_STREAM_READ,
		StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW,
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW,
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY
	};

	enum class BufferType : uint32_t
	{
		Index = GL_ELEMENT_ARRAY_BUFFER,
		Vertex = GL_ARRAY_BUFFER,
		Uniform = GL_UNIFORM_BUFFER
	};

	enum class BufferAccess: uint32_t
	{
		ReadOnly = GL_READ_ONLY,
		WriteOnly = GL_WRITE_ONLY,
		ReadWrite = GL_READ_WRITE
	};

	inline BufferBit operator|(const BufferBit &left, const BufferBit &right)
	{
		return static_cast<BufferBit>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
	}
}
