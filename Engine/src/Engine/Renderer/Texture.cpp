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

			if (multisampled)
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &name);
			else
				glCreateTextures(GL_TEXTURE_2D, 1, &name);

			return name;
		}
	}

	void Texture::Resize(const Vector2u& size) {
		if (size.Width != m_Internals->Size.Width || size.Height != m_Internals->Size.Height) {
			const auto Image = ToImage();

			m_Internals->Invalidate();
			m_Internals->Allocate(size, m_Internals->Samples, m_Internals->ImageFormat);

			Update(Image);
		}
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
		glBindTexture(Utils::GetTextureTarget(m_Internals->Multisampled), *this);
	}

	void Texture::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, *this);
	}

	void Texture::Unbind() const {
		glBindTexture(Utils::GetTextureTarget(m_Internals->Multisampled), *this);
	}

	void Texture::UnbindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, *this);
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

		if (CheckSize(size))
			texture->CreateTexture(samples, size, imageFormat, pixels, typeUsed, formatUsed);

		texture->SetLabel(label);

		return texture;
	}

	Ref<Texture> Texture::Create(Ref<Image> image, Engine::ImageFormat imageFormat, uint32_t samples,
		const std::string& label) {
		auto texture = Ref<Texture>(new Texture());

		if (CheckSize(image->Size()))
			texture->CreateTexture(samples, image->Size(), imageFormat, image->GetPixels().data());

		texture->SetLabel(label);

		return texture;
	}


	void Texture::SetLabel(const std::string& label) {
		if (label.empty())
			return;

		glObjectLabel(GL_TEXTURE, *this, -1, label.c_str());
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
		m_Internals->Wrapping.S = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_S, Utils::ToGLWrapMode(wrapping));
	}

	void Texture::SetWrappingT(WrapMode wrapping) {
		m_Internals->Wrapping.T = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_T, Utils::ToGLWrapMode(wrapping));
	}

	void Texture::SetFilters(FilterMode min, FilterMode mag) {
		SetMinFilter(min);
		SetMagFilter(mag);
	}

	void Texture::SetMinFilter(FilterMode filter) {
		m_Internals->Filter.Min = filter;
		m_Internals->SetParameter(GL_TEXTURE_MIN_FILTER, Utils::ToGLFilterMode(filter));
	}

	void Texture::SetMagFilter(FilterMode filter) {
		if (filter > FilterMode::Linear) {
			LOG_ENGINE_ERROR("Only possible values for mag filter is Nearest or Linear");
			return;
		}

		m_Internals->Filter.Mag = filter;
		m_Internals->SetParameter(GL_TEXTURE_MAG_FILTER, Utils::ToGLFilterMode(filter));
	}

	Ref<Image> Texture::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.
			Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		m_Internals->GetImage(pixels.data(), static_cast<uint32_t>(size));
		return MakeRef<Image>(m_Internals->Size, pixels.data());
	}

	Ref<Image> Texture::GetImage(const Vector2u& size, const Vector2i& offset) const {
		const uint64_t pixelCount = static_cast<uint64_t>(size.Width - offset.X) * static_cast<uint64_t>(size.Height -
			offset.Y);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);

		m_Internals->GetImage(pixels.data(), static_cast<uint32_t>(pixelCount), size, offset);
		return MakeRef<Image>(size, pixels.data());
	}

	void Texture::Clear(int value) {
		m_Internals->Clear(&value, DataFormat::RGBA, DataType::Int);
	}

	void Texture::Clear(void* pixels, DataFormat dataFormat, DataType dataType) {
		m_Internals->Clear(pixels, dataFormat, dataType);
	}

	void Texture::Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                    DataType dataType) {
		m_Internals->Clear(pixels, offset, size, dataFormat, dataType);
	}

	void Texture::GetPixels(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		ENGINE_ASSERT(size != 0);

		if (!pixels || size == 0)
			throw std::runtime_error("Uninitialized memory access");

		if (size >= m_Internals->Size.Width * m_Internals->Size.Height)
			m_Internals->GetImage(pixels, size);
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

	bool Texture::CheckSize(const Vector2u& size) {
		const uint32_t maxSize = GetMaxSize();
		const uint32_t texSize = size.Width * size.Height;

		ENGINE_ASSERT(texSize != 0)
		ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);

		if (texSize == 0)
			throw std::runtime_error("Texture dimensions cannot be 0");


		if (size.Width > maxSize || size.Height > maxSize) {
			throw std::out_of_range(fmt::format(
				"Unable to create texture with size of {}, texture is too big. Maximum texture size is {}", size,
				maxSize));
		}

		return true;
	}

	void Texture::CreateTexture(uint32_t samples, const Vector2u& size, enum ImageFormat ImageFormat,
	                            const void* pixels, DataType dataType, DataFormat dataFormat) {
		if (samples > 1)
			m_Internals->Allocate(size, samples, ImageFormat);
		else {
			m_Internals->Allocate(size, ImageFormat);

			SetFilters(FilterMode::Nearest, FilterMode::Nearest);
			SetWrapping(WrapMode::Repeat, WrapMode::Repeat);
		}

		if (pixels)
			m_Internals->SendImage(pixels, size, {0, 0}, dataFormat, dataType);
	}

	void Texture::Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat,
	                     DataType dataType) {
		m_Internals->SendImage(pixels, size, offset, dataFormat, dataType);
	}

	Texture::Internals::Internals(bool multisampled) : ID(Utils::GenTexture(multisampled)), Multisampled(multisampled) {
	}

	Texture::Internals::~Internals() {
		glDeleteTextures(1, &ID);
	}

	void Texture::Internals::Allocate(const Vector2u& size, enum ImageFormat imageFormat) {
		Size = size;
		ImageFormat = imageFormat;
		Samples = 1;

		glTextureStorage2D(ID, 1, Utils::ToGLImageFormat(imageFormat), static_cast<GLsizei>(size.Width),
		                   static_cast<GLsizei>(size.Height));
	}

	void Texture::Internals::Allocate(const Vector2u& size, uint32_t samples, enum ImageFormat imageFormat) {
		Size = size;
		ImageFormat = imageFormat;
		Samples = samples;

		ENGINE_ASSERT(samples > 0);
		if (Samples == 0)
			throw std::runtime_error("Must be at least 1 sample!");

		glTextureStorage2DMultisample(ID, static_cast<GLsizei>(samples), Utils::ToGLImageFormat(imageFormat),
		                              static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), GL_FALSE);
	}

	void Texture::Internals::SendImage(const void* pixels, const Vector2u& size, const Vector2i& offset,
	                                   DataFormat format, DataType dataType) {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		CheckSubRegionSize(offset, size);

		glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y),
		                    Utils::ToGLDataFormat(format), Utils::ToGLDataType(dataType), pixels);
	}

	void Texture::Internals::GetImage(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(
			static_cast<uint64_t>(size) < static_cast<uint64_t>(Size.Width) * static_cast<uint64_t>(Size.Height));

		glGetTextureImage(ID, 0, GL_RGBA8, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size), pixels);
	}

	void Texture::Internals::GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(pixels);

		const uint32_t gettingSize = (size.Width - offset.X) * (size.Height - offset.Y);

		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(bufSize >= gettingSize);
		if (bufSize < gettingSize)
			throw std::out_of_range("Buffer is too small");

		CheckSubRegionSize(offset, size);

		glGetTextureSubImage(ID, 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, GL_RGBA8, GL_UNSIGNED_BYTE,
		                     bufSize, pixels);
	}

	void Texture::Internals::Clear(void* pixels, DataFormat dataFormat, DataType dataType) {
		glClearTexImage(ID, 0, Utils::ToGLDataFormat(dataFormat), Utils::ToGLDataType(dataType), pixels);
	}

	void Texture::Internals::Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                               DataType dataType) {
		CheckSubRegionSize(offset, size);

		glClearTexSubImage(ID, 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, Utils::ToGLDataFormat(dataFormat),
		                   Utils::ToGLDataType(dataType), nullptr);
	}

	void Texture::Internals::CheckSubRegionSize(const Vector2i& offset, const Vector2u& size) const {
		ENGINE_ASSERT(Size.Width >= size.Width + offset.X && Size.Height >= size.Height + offset.Y);
		if (Size.Width < size.Width + offset.X || Size.Height < size.Height + offset.Y)
			throw std::out_of_range("SubImage out of range");
	}

	void Texture::Internals::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(ID, name, parameter);
	}

	void Texture::Internals::GetParameter(uint32_t name, int* parameter) const {
		glGetTextureParameteriv(ID, name, parameter);
	}

	void Texture::Internals::Invalidate() {
		glDeleteTextures(1, &ID);
		ID = Utils::GenTexture(Multisampled);
	}
}
