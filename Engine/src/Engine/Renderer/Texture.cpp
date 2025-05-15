#include "pch.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Image.h"
#include "Engine/Utils/OpenGlUtils.h"

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
			ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);

			if (buffSize == 0)
				throw std::runtime_error("Texture dimensions cannot be 0");


			if (size.Width > maxSize || size.Height > maxSize) {
				throw std::out_of_range(fmt::format(
					"Unable to create texture with size of {}, texture is too big. Maximum texture size is {}", size,
					maxSize));
			}
		}

		void inline CheckSubRegionSize(const Vector2i& offset, const Vector2u& size, const Vector2u& texSize) {
			ENGINE_ASSERT(texSize.Width >= (size.Width + offset.X) && texSize.Height >= (size.Height + offset.Y));
			if (texSize.Width < (size.Width + offset.X) || texSize.Height < (size.Height + offset.Y))
				throw std::out_of_range("SubImage out of range");
		}
	}

	void Texture::Resize(const Vector2u& size) {
		if (size.Width != m_Internals->Size.Width || size.Height != m_Internals->Size.Height) {
			const auto Image = ToImage();

			m_Internals->Invalidate();
			ReAlloc(size);

			Update(Image);
		}
	}

	void Texture::ResizeNoCopy(const Vector2u& size) {
		if (size.Width != m_Internals->Size.Width || size.Height != m_Internals->Size.Height) {
			m_Internals->Invalidate();
			ReAlloc(size);
		}
	}

	void Texture::Invalidate() {
		m_Internals->Invalidate();
	}

	Texture::Texture(bool multisampled) : m_Internals(MakeRef<Internals>(multisampled)) {
	}

	void Texture::GenerateMipMaps() const {
		if (m_Internals->Multisampled)
			return;

		m_Internals->MipMapGenerated = true;
		glGenerateTextureMipmap(m_Internals->ID);
	}

	void Texture::Bind() const {
		glBindTexture(Utils::GetTextureTarget(m_Internals->Multisampled), static_cast<GLuint>(*this));
	}

	void Texture::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, static_cast<GLuint>(*this));
	}

	void Texture::Unbind() const {
		glBindTexture(Utils::GetTextureTarget(m_Internals->Multisampled), static_cast<GLuint>(*this));
	}

	void Texture::UnbindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, static_cast<GLuint>(*this));
	}

	Ref<Texture> Texture::Create(const Vector2u& size, Engine::ImageFormat imageFormat, uint32_t samples,
		const std::string& label, const uint8_t* pixels, std::optional<DataType> dataType,
		std::optional<DataFormat> dataFormat) {
		auto texture = Ref<Texture>(new Texture());

		DataType typeUsed;
		DataFormat formatUsed;

		if (!dataFormat.has_value() || !dataType.has_value()) {
			std::tie(formatUsed, typeUsed) = Utils::GetDefaultFormatAndType(imageFormat);
		} else {
			formatUsed = *dataFormat;
			typeUsed = *dataType;
		}

		texture->CreateTexture(samples, size, imageFormat, pixels, typeUsed, formatUsed);

		texture->SetLabel(label);

		return texture;
	}

	Ref<Texture> Texture::Create(Ref<Image> image, Engine::ImageFormat imageFormat, uint32_t samples,
		const std::string& label) {
		auto texture = Ref<Texture>(new Texture());

		texture->CreateTexture(samples, image->Size(), imageFormat, image->GetPixels().data());

		texture->SetLabel(label);

		return texture;
	}


	void Texture::SetLabel(const std::string& label) {
		if (label.empty() && m_Internals->Label.empty())
			return;

		glObjectLabel(GL_TEXTURE, static_cast<GLuint>(*this), -1, label.data());
		m_Internals->Label = label;
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
			LOG_GL_WARN("Cannot specify wrapping T on multisampled texture");
			return;
		}

		m_Internals->Wrapping.S = wrapping;
		SetParameter(GL_TEXTURE_WRAP_S, Utils::ToGLWrapMode(wrapping));
	}

	void Texture::SetWrappingT(WrapMode wrapping) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot specify wrapping T on multisampled texture");
			return;
		}

		m_Internals->Wrapping.T = wrapping;
		SetParameter(GL_TEXTURE_WRAP_T, Utils::ToGLWrapMode(wrapping));
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

		m_Internals->Filter.Min = filter;
		SetParameter(GL_TEXTURE_MIN_FILTER, Utils::ToGLFilterMode(filter));
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

		m_Internals->Filter.Mag = filter;
		SetParameter(GL_TEXTURE_MAG_FILTER, Utils::ToGLFilterMode(filter));
	}

	Ref<Image> Texture::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.
			Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		GetImage(pixels.data(), static_cast<uint32_t>(size));
		return Image::Create(m_Internals->Size, pixels.data());
	}

	Ref<Image> Texture::GetImage(const Vector2u& size, const Vector2i& offset) const {
		const uint64_t pixelCount = static_cast<uint64_t>(size.Width - offset.X) * static_cast<uint64_t>(size.Height -
			offset.Y);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);

		GetImage(pixels.data(), static_cast<uint32_t>(pixelCount), size, offset);
		return Image::Create(m_Internals->Size, pixels.data());
	}

	void Texture::Clear(const Color& color) {
		return Clear(static_cast<const void*>(&color.Code), DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Clear(int value) {
		return Clear(&value, DataFormat::RGBA, DataType::Int);
	}

	void Texture::Clear(const void* pixels, DataFormat dataFormat, DataType dataType) {
		glClearTexImage(static_cast<GLuint>(*this), 0, Utils::ToGLDataFormat(dataFormat), Utils::ToGLDataType(dataType), pixels);
	}

	void Texture::Clear(const Color& color, const Vector2i& offset, const Vector2u& size) {
		Clear(static_cast<const void*>(&color.Code), offset, size, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Clear(int value, const Vector2i& offset, const Vector2u& size) {
		Clear(static_cast<const void*>(&value), offset, size, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Clear(const void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                    DataType dataType) {
		Utils::CheckSubRegionSize(offset, size, Size());

		glClearTexSubImage(static_cast<GLuint>(*this), 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, Utils::ToGLDataFormat(dataFormat),
			Utils::ToGLDataType(dataType), pixels);
	}

	void Texture::GetPixels(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		ENGINE_ASSERT(size != 0);

		if (!pixels || size == 0)
			throw std::runtime_error("Uninitialized memory access");

		if (size >= m_Internals->Size.Width * m_Internals->Size.Height)
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
		std::swap(m_Internals, to.m_Internals);
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

	void Texture::CreateTexture(uint32_t samples, const Vector2u& size, enum ImageFormat ImageFormat,
	                            const void* pixels, DataType dataType, DataFormat dataFormat) {

		Utils::CheckIfValidSize(size);

		Allocate(samples, size, ImageFormat);
		if (!IsMultisampled()) {
			SetFilters(FilterMode::Nearest, FilterMode::Nearest);
			SetWrapping(WrapMode::Repeat, WrapMode::Repeat);
		}

		if (pixels)
			UploadPixels(pixels, size, {0, 0}, dataFormat, dataType);
	}

	void Texture::Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat,
	                     DataType dataType) {
		UploadPixels(pixels, size, offset, dataFormat, dataType);
	}

	void Texture::ReAlloc(const Vector2u& size) {
		const auto& samples = m_Internals->Samples;
		const auto& imageFormat = m_Internals->ImageFormat;

		Allocate(samples, size, imageFormat);
	}

	void Texture::Allocate(uint32_t samples, const Vector2u& size, enum Engine::ImageFormat imageFormat) {
		m_Internals->Size = size;
		m_Internals->ImageFormat = imageFormat;
		m_Internals->Samples = samples;
		const auto& imageFormatGL = m_Internals->ImageFormatGL = Utils::ToGLImageFormat(imageFormat);

		if (samples > 1) {
			glTextureStorage2DMultisample(static_cast<GLuint>(*this), static_cast<GLsizei>(samples), imageFormatGL, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), GL_FALSE);
		} else {
			glTextureStorage2D(static_cast<GLuint>(*this), 1, imageFormatGL, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
		}
	}

	void Texture::UploadPixels(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
		DataType dataType) {
		ENGINE_ASSERT(pixels);

		if (!pixels)
			return;

		Utils::CheckSubRegionSize(offset, size, Size());
		glTextureSubImage2D(static_cast<GLuint>(*this), 0, offset.X, offset.Y, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), Utils::ToGLDataFormat(format), Utils::ToGLDataType(dataType), pixels);
	}

	void Texture::GetImage(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(
			static_cast<uint64_t>(size) < static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.Height));

		glGetTextureImage(static_cast<GLuint>(*this), 0, GL_RGBA8, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size), pixels);
	}

	void Texture::GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset) const {
		ENGINE_ASSERT(pixels);

		const uint32_t gettingSize = (size.Width - offset.X) * (size.Height - offset.Y);

		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(bufSize >= gettingSize);
		if (bufSize < gettingSize)
			throw std::out_of_range("Buffer is too small");

		Utils::CheckSubRegionSize(offset, size, Size());

		glGetTextureSubImage(static_cast<GLuint>(*this), 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, GL_RGBA8, GL_UNSIGNED_BYTE,
			static_cast<GLsizei>(bufSize), pixels);
	}

	void Texture::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(static_cast<GLuint>(*this), name, parameter);
	}

	void Texture::GetParameter(uint32_t name, int* parameter) const {
		glGetTextureParameteriv(static_cast<GLuint>(*this), name, parameter);
	}

	Texture::Internals::Internals(bool multisampled) : ID(Utils::GenTexture(multisampled)), Multisampled(multisampled) {
	}

	Texture::Internals::~Internals() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
	}

	void Texture::Internals::Invalidate() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
		ID = Utils::GenTexture(Multisampled);
		if (!Label.empty())
			glObjectLabel(GL_TEXTURE, ID, -1, Label.data());
	}
}
