#pragma once

#include "Engine/OpenGL/GLEnums.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Rect.h"

#include <unordered_map>
#include <glad/glad.h>

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

		StencilTest(
			uint32_t mask,
			int ref,
			Function function,
			Operation stencilFail,
			Operation depthFails,
			Operation pass
			) : StencilFail(stencilFail),
			    DepthFails(depthFails),
			    Pass(pass),
			    Mask(mask),
			    TestFunction(function),
			    Ref(ref) {}

		StencilTest(
			PolygonFacing face,
			uint32_t mask,
			int ref,
			Function function,
			Operation stencilFail,
			Operation depthFails,
			Operation pass
			) : Face(face),
			    StencilFail(stencilFail),
			    DepthFails(depthFails),
			    Pass(pass),
			    Mask(mask),
			    TestFunction(function),
			    Ref(ref) {}


		constexpr bool operator==(const StencilTest &test) const
		{
			return Face == test.Face && StencilFail == test.StencilFail && DepthFails == test.DepthFails && Pass == test
				.Pass && Mask == test.Mask && TestFunction == test.TestFunction && Ref == test.Ref;
		}

		constexpr bool operator!=(const StencilTest &test) const
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

		BlendMode(
			Factor sourceFactor,
			Factor destinationFactor,
			Equation blendEquation = Equation::Add
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

		Factor ColorSrcFactor  = Factor::SrcAlpha;
		Factor ColorDstFactor  = Factor::OneMinusSrcAlpha;
		Equation ColorEquation = Equation::Add;
		Factor AlphaSrcFactor  = Factor::One;
		Factor AlphaDstFactor  = Factor::OneMinusSrcAlpha;
		Equation AlphaEquation = Equation::Add;

		constexpr bool operator ==(const BlendMode &mode) const
		{
			return ColorSrcFactor == mode.ColorSrcFactor && ColorDstFactor == mode.ColorDstFactor && ColorEquation ==
				mode.ColorEquation && AlphaSrcFactor == mode.AlphaSrcFactor && AlphaDstFactor == mode.AlphaDstFactor &&
				AlphaEquation == mode.AlphaEquation;
		}

		constexpr bool operator !=(const BlendMode &mode) const
		{
			return !(*this == mode);
		}
	};

	class OpenGlFunctions
	{
		friend Context;

		Context *m_Context;

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
		OpenGlFunctions();
		OpenGlFunctions(const OpenGlFunctions &functions);
		OpenGlFunctions(OpenGlFunctions &&functions);

		OpenGlFunctions& operator=(const OpenGlFunctions &functions);
		OpenGlFunctions& operator=(OpenGlFunctions &&functions);

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
			return SetViewPort(
			                   static_cast<int32_t>(viewPort.X),
			                   static_cast<int32_t>(viewPort.Y),
			                   viewPort.Width,
			                   viewPort.Height
			                  );
		}

		void SetViewPort(const Vector2i &pos, const Vector2u &size)
		{
			return SetViewPort(pos.X, pos.Y, size.Width, size.Height);
		}

		void SetBlendMode(const BlendMode &blendMode);
		void SetStencilTest(const StencilTest &stencilTest);

		BlendMode GetBlendMode() const;
		StencilTest GetStencilTest() const;

		uint32_t GenTexture();
		uint32_t* GenTextures(uint32_t account);
		void GenTextures(uint32_t account, uint32_t *textures);

		void DeleteTexture(uint32_t texture);
		void DeleteTextures(uint32_t account, uint32_t *textures);

		void TextureParameter(uint32_t texture, TextureParamName name, float param);
		void TextureParameter(uint32_t texture, TextureParamName name, int32_t param);
		void TextureParameter(uint32_t texture, TextureParamName name, const float *param);
		void TextureParameter(uint32_t texture, TextureParamName name, const int32_t *param);

		void GenerateMipMap(uint32_t texture);
		void GenerateMipMap(TextureTarget target, uint32_t texture);

		void Image2DBind(
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
			);
		void Image2D(
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
			);
		void SubImage2D(
			uint32_t texture,
			int32_t level,
			int32_t xOffset,
			int32_t yOffset,
			uint32_t width,
			uint32_t height,
			Format format,
			DataType type,
			const void *data
			);

		void Image2DBind(
			uint32_t texture,
			TextureTarget target,
			int32_t level,
			InternalFormat internalFormat,
			const Vector2u &size,
			int32_t border,
			Format format,
			DataType type,
			const void *data
			);
		void Image2D(
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
			);
		void SubImage2D(
			uint32_t texture,
			int32_t level,
			const Vector2i &offset,
			const Vector2u &size,
			Format format,
			DataType type,
			const void *data
			);

		void BindTexture(TextureTarget target, uint32_t texture);
		void BindRenderBuffer(uint32_t buffer);
		void BindFrameBuffer(uint32_t buffer, bool read = false);

		void GetTextureImage(
			uint32_t texture,
			int32_t level,
			Format format,
			DataType type,
			uint32_t bufferSize,
			void *pixels
			);

		uint32_t GenRenderBuffer();
		uint32_t* GenRenderBuffers(uint32_t size);
		void GenRenderBuffers(uint32_t size, uint32_t *buffers);

		void DeleteRenderBuffer(uint32_t buffer);
		void DeleteRenderBuffers(uint32_t size, uint32_t *buffers);

		void RenderBufferStorage(uint32_t renderBuffer, InternalFormat internalFormat, uint32_t width, uint32_t height);
		void RenderBufferStorage(uint32_t renderBuffer, InternalFormat internalFormat, const Vector2u& size);

		void RenderBufferStorage(uint32_t renderBuffer, uint32_t samples, InternalFormat internalFormat, uint32_t width, uint32_t height);
		void RenderBufferStorage(uint32_t renderBuffer, uint32_t samples, InternalFormat internalFormat, const Vector2u& size);

		uint32_t GenFrameBuffer();
		uint32_t *GenFrameBuffers(uint32_t size);
		void GenFrameBuffers(uint32_t size, uint32_t *buffers);

		void DeleteFrameBuffer(uint32_t buffer);
		void DeleteFrameBuffers(uint32_t size, const uint32_t* buffers);

		uint32_t CheckFrameBufferStatus(uint32_t frameBuffer, uint32_t target = GL_DRAW_FRAMEBUFFER);
		void FrameBufferTexture(uint32_t frameBuffer, uint32_t attachment, uint32_t texture, int32_t level);
		void FrameBufferRenderBuffer(uint32_t frameBuffer, uint32_t attachment, uint32_t renderBufferTarget, uint32_t renderBuffer);

		uint32_t CreateShader(uint32_t shaderType);
		void DeleteShader(uint32_t shader);

		void ShaderSource(uint32_t shader, uint32_t count, const char* const* string, const int32_t* length);
		void ShaderSource(uint32_t shader, uint32_t count, const std::string* string, const int32_t* length);
		void ShaderSource(uint32_t shader, uint32_t count, const std::string_view* string);
		void ShaderSource(uint32_t shader, const std::string_view& string);
		void ShaderSource(uint32_t shader, const std::string& string);
		void ShaderSource(uint32_t shader, const char* string, int32_t length);
		void ShaderSource(uint32_t shader, const char* string);

		void CompileShader(uint32_t shader);

		void GetShader(uint32_t shader, ShaderParameterName name, int32_t* params);
		int32_t GetShader(uint32_t shader, ShaderParameterName name);

		std::string ShaderInfoLog(uint32_t shader);


		bool GetBoolean(uint32_t name);
		double GetDouble(uint32_t name);
		float GetFloat(uint32_t name);
		int32_t GetInteger(uint32_t name);
		int64_t GetInteger64(uint32_t name);

		bool GetBoolean(uint32_t name, uint32_t index);
		double GetDouble(uint32_t name, uint32_t index);
		float GetFloat(uint32_t name, uint32_t index);
		int32_t GetInteger(uint32_t name, uint32_t index);
		int64_t GetInteger64(uint32_t name, uint32_t index);

		void Get(uint32_t name, bool *data);
		void Get(uint32_t name, double *data);
		void Get(uint32_t name, float *data);
		void Get(uint32_t name, int32_t *data);
		void Get(uint32_t name, int64_t *data);

		void Get(uint32_t target, uint32_t index, bool *data);
		void Get(uint32_t target, uint32_t index, double *data);
		void Get(uint32_t target, uint32_t index, float *data);
		void Get(uint32_t target, uint32_t index, int32_t *data);
		void Get(uint32_t target, uint32_t index, int64_t *data);

		void Flush();
		void Finish();

		void SetDebugMessageCallback(GLDEBUGPROC callback, const void *userParam);

		Context& GetContext() const { return *m_Context; }

	private:
		static constexpr bool IsBindable(TextureTarget target)
		{
			return target == TextureTarget::Texture1D || target == TextureTarget::Texture2D || target ==
				TextureTarget::Texture3D || target == TextureTarget::Texture1DArray || target ==
				TextureTarget::Texture2DArray || target == TextureTarget::Rectangle || target == TextureTarget::CubeMap
				|| target == TextureTarget::CubeMapArray || target == TextureTarget::Buffer || target ==
				TextureTarget::Multisample2D || target == TextureTarget::Mutlisample2DArray;
		}

	private:
		template <typename T>
		T GetV(uint32_t name)
		{
			T val;

			Get(name, &val);

			return val;
		}

		template <typename T>
		T GetV(uint32_t name, uint32_t index)
		{
			T val;
			Get(name, index, &val);

			return val;
		}
	};
}
