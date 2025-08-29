#include "pch.h"
#include "Engine/Renderer/Texture.h"

#include "Engine/Core/Image.h"

#include "Engine/Utils/Renderer/GLEnumConverters.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"
#include "Engine/Utils/Renderer/ImageFormatTraits.h"
#include "Engine/Utils/Renderer/FilterModeUtils.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		constexpr GLenum GetTextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		Texture::IDType GenTexture(bool multisampled) {
			Texture::IDType name;

			glCreateTextures(GetTextureTarget(multisampled), 1, &name);

			return name;
		}

		void inline CheckIfValidSize(const Vector2u& size) {
			const uint32_t maxSize = Texture::GetMaxSize();
			const uint32_t buffSize = size.Width * size.Height;

			ENGINE_ASSERT(buffSize != 0)
			ENGINE_ASSERT(size.Width <= maxSize && size.Height <= maxSize);

			if (buffSize == 0)
				throw std::runtime_error("Texture dimensions cannot be 0");


			if (size.Width > maxSize || size.Height > maxSize) {
				throw std::out_of_range(fmt::format(
					"Unable to create texture with size of {}, texture is too big. Maximum texture size is {}", size,
					maxSize));
			}
		}

		void inline CheckSubRegionSize(const Vector2i& offset, const Vector2u& size, const Vector2u& texSize) {
			ENGINE_ASSERT(offset.X >= 0 && offset.Y >= 0);
			if (offset.X < 0 || offset.Y < 0) throw std::out_of_range("Negative subimage offset");

			const uint32_t width = size.Width + offset.X;
			const uint32_t height = size.Height + offset.Y;

			ENGINE_ASSERT(texSize.Width >= width && texSize.Height >= height );
			if (texSize.Width < width || texSize.Height < height)
				throw std::out_of_range("SubImage out of range");
		}
	}

	void Texture::Resize(const Vector2u& size) {
		if (size != m_GLState->Size) {
			LOG_ENGINE_INFO("Resizing texture '{}' from {} to {}", Label(), m_GLState->Size, size);
			const auto Image = ToImage();

			Recreate(size);

			Update(Image);
		}
	}

	void Texture::ResizeNoCopy(const Vector2u& size) {
		if (size != m_GLState->Size) {
			LOG_ENGINE_INFO("Resizing texture '{}' from {} to {}", Label(), m_GLState->Size, size);
			Recreate(size);
		}
	}

	void Texture::Invalidate() {
		m_GLState->Invalidate();
	}

	std::string Texture::DebugInfo() const {
		return fmt::format("Texture: {}x{}, format: {}, samples: {}, usage: {}, label: '{}'",
		                   m_GLState->Size.Width, m_GLState->Size.Height,
		                   m_GLState->ImageFormat,
		                   m_GLState->Samples,
		                   m_GLState->Usage,
		                   m_GLState->Label);
	}

	Texture::Texture(bool multisampled) : m_GLState(MakeRef<GLState>(multisampled)) {
	}

	void Texture::Initialize(uint32_t samples, const Vector2u& size, enum ImageFormat ImageFormat,
	                         TextureUsage usage, const void* pixels, DataType dataType, DataFormat dataFormat) {
		Utils::CheckIfValidSize(size);
		ValidateSize(size);
		SetupStorage(samples, size, usage, ImageFormat);
		SetupDefaultParameters(usage);

		if (pixels)
			UploadPixels(pixels, size, {0, 0}, dataFormat, dataType);
	}

	void Texture::Recreate(const Vector2u& size) {
		Invalidate();
		ReAlloc(size);
		ReapplyParameters();
	}

	void Texture::GenerateMipMaps() const {
		if (m_GLState->Multisampled)
			return;

		m_GLState->MipMapGenerated = true;
		glGenerateTextureMipmap(m_GLState->ID);
	}

	void Texture::Bind() const {
		glBindTexture(Utils::GetTextureTarget(m_GLState->Multisampled), static_cast<GLuint>(*this));
	}

	void Texture::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, static_cast<GLuint>(*this));
	}

	void Texture::Unbind() const {
		glBindTexture(Utils::GetTextureTarget(m_GLState->Multisampled), 0);
	}

	void Texture::UnbindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, 0);
	}

	Ref<Texture> Texture::Create(const TextureSpec& spec) {
		auto texture = Ref<Texture>(new Texture());

		DataType typeUsed;
		DataFormat formatUsed;

		if (!spec.DataFormat.has_value() || !spec.DataType.has_value()) {
			std::tie(formatUsed, typeUsed) = Utils::GetDefaultFormatAndType(spec.ImageFormat);
		}
		else {
			formatUsed = *spec.DataFormat;
			typeUsed = *spec.DataType;
		}

		texture->Initialize(spec.Samples, spec.Size, spec.ImageFormat, spec.Usage, spec.InitialData, typeUsed,
		                    formatUsed);
		texture->SetLabel(spec.Label);

		LOG_ENGINE_DEBUG("Created texture: {}", texture->DebugInfo());
		return texture;
	}

	Ref<Texture> Texture::Create(const Ref<Image>& image, Engine::ImageFormat imageFormat, uint32_t samples,
	                             const std::string& label) {
		TextureSpec spec;

		spec.Size = image->Size();
		spec.ImageFormat = imageFormat;
		spec.Samples = samples;
		spec.Label = label;
		spec.InitialData = image->GetPixels().data();
		spec.Usage = TextureUsage::Default;

		return Create(spec);
	}

	Ref<Texture> Texture::Create(const Ref<Image>& image, TextureUsage usage, Engine::ImageFormat imageFormat,
	                             uint32_t samples,
	                             const std::string& label) {
		TextureSpec spec;

		spec.Size = image->Size();
		spec.ImageFormat = imageFormat;
		spec.Samples = samples;
		spec.Label = label;
		spec.InitialData = image->GetPixels().data();
		spec.Usage = usage;

		return Create(spec);
	}


	void Texture::SetLabel(const std::string& label) {
		if (label.empty() && m_GLState->Label.empty())
			return;

		glObjectLabel(GL_TEXTURE, static_cast<GLuint>(*this), -1, label.data());
		m_GLState->Label = label;
	}

	void Texture::SetWrapping(WrapMode s) {
		SetWrappingS(s);
	}

	void Texture::SetWrapping(WrapMode s, WrapMode t) {
		SetWrappingS(s);
		SetWrappingT(t);
	}

	void Texture::SetWrappingS(WrapMode wrapping) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot specify wrapping S on multisampled texture");
			return;
		}

		m_GLState->Wrapping.S = wrapping;
		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(Utils::EnumToGLConstant(wrapping)));
	}

	void Texture::SetWrappingT(WrapMode wrapping) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot specify wrapping T on multisampled texture");
			return;
		}

		m_GLState->Wrapping.T = wrapping;
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(Utils::EnumToGLConstant(wrapping)));
	}

	void Texture::SetFilters(FilterMode min, FilterMode mag) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set filters on multisampled texture");
			return;
		}

		SetMinFilter(min);
		SetMagFilter(mag);
	}

	void Texture::SetMinFilter(FilterMode filter) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set min filter on multisampled texture");
			return;
		}

		m_GLState->Filter.Min = filter;
		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(Utils::EnumToGLConstant(filter)));
	}

	void Texture::SetMagFilter(FilterMode filter) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set mag filter on multisampled texture");
			return;
		}

		if (filter > FilterMode::Linear) {
			LOG_ENGINE_ERROR("Only possible values for mag filter is Nearest or Linear");
			return;
		}

		m_GLState->Filter.Mag = Utils::SanitizeMag(filter);
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(Utils::EnumToGLConstant(Utils::SanitizeMag(filter))));
	}

	Ref<Image> Texture::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_GLState->Size.Width) * static_cast<uint64_t>(m_GLState->Size.
			Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		GetImage(pixels.data(), static_cast<uint32_t>(size * 4));
		return Image::Create(m_GLState->Size, pixels.data());
	}

	Ref<Image> Texture::GetImage(const Vector2u& size, const Vector2i& offset) const {
		const uint64_t pixelCount = static_cast<uint64_t>(size.Width) * static_cast<uint64_t>(size.Height);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);

		GetImage(pixels.data(), static_cast<uint32_t>(pixelCount * 4), size, offset);
		return Image::Create(size, pixels.data());
	}

	void Texture::Clear(const Color& color) {
		return Clear(&color.Code, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Clear(int value) {
		return Clear(&value, DataFormat::RGBA, DataType::Int);
	}

	void Texture::Clear(const void* pixels, DataFormat dataFormat, DataType dataType) {
		glClearTexImage(static_cast<GLuint>(*this), 0, Utils::EnumToGLConstant(dataFormat), Utils::EnumToGLConstant(dataType),
		                pixels);

		// LOG_GL_TRACE("Clearing texture: label='{}', full size={}, format={}, type={}", Label(), Size(), dataFormat, dataType);
	}

	void Texture::ClearRegion(const Color& color, const Vector2i& offset, const Vector2u& size) {
		ClearRegion(&color.Code, offset, size, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::ClearRegion(int value, const Vector2i& offset, const Vector2u& size) {
		ClearRegion(&value, offset, size, DataFormat::RGBA, DataType::Int);
	}

	void Texture::ClearRegion(const void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                          DataType dataType) {
		Utils::CheckSubRegionSize(offset, size, Size());

		glClearTexSubImage(static_cast<GLuint>(*this), 0, offset.X, offset.Y, 0, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 1,
		                   Utils::EnumToGLConstant(dataFormat),
		                   Utils::EnumToGLConstant(dataType), pixels);

		LOG_GL_TRACE("Clearing texture region: offset={}, size={}, format={}, type={}",
			offset, size, dataFormat, dataType);
	}

	void Texture::GetPixels(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		ENGINE_ASSERT(size != 0);

		if (!pixels || size == 0)
			throw std::runtime_error("Uninitialized memory access");

		const uint64_t expected = static_cast<uint64_t>(m_GLState->Size.Width) * static_cast<uint64_t>(m_GLState->Size.Height) * 4;
		if (static_cast<uint64_t>(size) >= expected)
			GetImage(pixels, size);
		else
			throw std::out_of_range("Specified buffer is too small");
	}

	void Texture::Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
	                     DataType dataType) {
		Update(static_cast<const void*>(pixels), size, offset, format, dataType);
	}

	void Texture::Update(const Color* pixels, const Vector2u& size, const Vector2i& offset) {
		Update(pixels, size, offset, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Update(const Texture& texture) {
		Update(texture, {0, 0});
	}

	void Texture::Update(const Texture& texture, const Vector2i& offset) {
		Update(texture.ToImage(), offset);
	}

	void Texture::Update(Ref<Image> image) {
		Update(image->GetPixels().data(), image->Size(), {0, 0});
	}

	void Texture::Update(Ref<Image> image, const Vector2i& offset) {
		Update(image->GetPixels().data(), image->Size(), offset);
	}

	void Texture::Swap(Texture& to) {
		LOG_ENGINE_TRACE("Swapping texture '{}' <-> '{}'", Label(), to.Label());
		std::swap(m_GLState, to.m_GLState);
	}

	uint32_t Texture::GetMaxSize() {
		static bool checked = false;
		static int32_t size = 0;

		if (!checked) {
			checked = true;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		}

		return static_cast<uint32_t>(size);
	}

	uint32_t Texture::QueryMaxColorSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	uint32_t Texture::QueryMaxIntegerSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	uint32_t Texture::QueryMaxDepthSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	void Texture::Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat,
	                     DataType dataType) {
		UploadPixels(pixels, size, offset, dataFormat, dataType);
	}

	void Texture::ReAlloc(const Vector2u& size) {
		const auto& samples = m_GLState->Samples;
		const auto& imageFormat = m_GLState->ImageFormat;
		const auto& usage = m_GLState->Usage;

		SetupStorage(samples, size, usage, imageFormat);
	}

	void Texture::SetupStorage(uint32_t samples, const Vector2u& size, TextureUsage usage,
	                           enum ImageFormat imageFormat) {
		m_GLState->Size = size;
		m_GLState->ImageFormat = imageFormat;
		m_GLState->Samples = samples;
		m_GLState->Usage = usage;
		const auto& imageFormatGL = m_GLState->ImageFormatGL = Utils::EnumToGLConstant(imageFormat);

		if (usage == TextureUsage::DepthStencil) {
			ENGINE_ASSERT(Utils::IsDepthFormat(imageFormat), "DepthStencil usage requires a depth-capable format.");
		}
		if (usage == TextureUsage::Storage) {
			ENGINE_ASSERT(Utils::IsStorageCapable(imageFormat),
			              "Storage usage requires image load/store-capable format.");
		}

		if (samples > 1) {
			glTextureStorage2DMultisample(static_cast<GLuint>(*this), static_cast<GLsizei>(samples), imageFormatGL,
			                              static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height),
			                              GL_FALSE);
		}
		else {
			glTextureStorage2D(static_cast<GLuint>(*this), 1, imageFormatGL, static_cast<GLsizei>(size.Width),
			                   static_cast<GLsizei>(size.Height));
		}

		LOG_GL_DEBUG("Allocating texture storage: {}x{}, format: {}, usage: {}, samples: {}",
			size.Width, size.Height, imageFormat, usage, samples);
	}

	void Texture::UploadPixels(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
	                           DataType dataType) {
		ENGINE_ASSERT(pixels);

		if (!pixels)
			return;

		Utils::CheckSubRegionSize(offset, size, Size());
		glTextureSubImage2D(static_cast<GLuint>(*this), 0, offset.X, offset.Y, static_cast<GLsizei>(size.Width),
		                    static_cast<GLsizei>(size.Height), Utils::EnumToGLConstant(format),
		                    Utils::EnumToGLConstant(dataType), pixels);

		LOG_GL_TRACE("Uploading texture data: size={}x{}, format={}, type={}", size.Width, size.Height, format, dataType);
	}

	void Texture::GetImage(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		const uint64_t neededSize = static_cast<uint64_t>(m_GLState->Size.Width) * static_cast<uint64_t>(m_GLState->Size.Height) * 4;

		ENGINE_ASSERT(static_cast<uint64_t>(size) >= neededSize);
		if (static_cast<uint64_t>(size) < neededSize)
			throw std::out_of_range("Buffer is too small");

		glGetTextureImage(static_cast<GLuint>(*this), 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size),
		                  pixels);
	}

	void Texture::GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset) const {
		ENGINE_ASSERT(pixels);

		const uint64_t neededSize = static_cast<uint64_t>(size.Width) * static_cast<uint64_t>(size.Height) * 4;

		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(bufSize >= neededSize);
		if (bufSize < neededSize)
			throw std::out_of_range("Buffer is too small");

		Utils::CheckSubRegionSize(offset, size, Size());

		glGetTextureSubImage(static_cast<GLuint>(*this), 0, offset.X, offset.Y, 0, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 1, GL_RGBA,
		                     GL_UNSIGNED_BYTE,
		                     static_cast<GLsizei>(bufSize), pixels);
	}

	void Texture::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(static_cast<GLuint>(*this), name, parameter);
	}

	void Texture::GetParameter(uint32_t name, int* parameter) const {
		glGetTextureParameteriv(static_cast<GLuint>(*this), name, parameter);
	}

	void Texture::SetupDefaultParameters(TextureUsage usage) {
		if (usage == TextureUsage::DepthStencil || usage == TextureUsage::Storage)
			return;

		SetFilters(FilterMode::Nearest, FilterMode::Nearest);
		SetWrapping(WrapMode::Repeat, WrapMode::Repeat);
	}

	void Texture::ValidateSize(Vector2u size) {
		Utils::CheckIfValidSize(size);
	}

	void Texture::ReapplyParameters() {
		if (!m_GLState->Multisampled && (m_GLState->Usage != TextureUsage::DepthStencil && m_GLState->Usage != TextureUsage::Storage)) {
			SetFilters(m_GLState->Filter.Min, m_GLState->Filter.Mag);
			SetWrapping(m_GLState->Wrapping.S, m_GLState->Wrapping.T);
		}
		if (!m_GLState->Label.empty()) {
			glObjectLabel(GL_TEXTURE, static_cast<GLuint>(*this), -1, m_GLState->Label.c_str());
		}
	}

	Texture::GLState::GLState(bool multisampled) : ID(Utils::GenTexture(multisampled)), Multisampled(multisampled) {
	}

	Texture::GLState::~GLState() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
	}

	void Texture::GLState::Invalidate() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
		ID = Utils::GenTexture(Multisampled);
		if (!Label.empty())
			glObjectLabel(GL_TEXTURE, ID, -1, Label.data());
	}
}
