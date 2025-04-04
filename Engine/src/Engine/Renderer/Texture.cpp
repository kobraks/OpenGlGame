#include "pch.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Image.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		Texture::IDType GenTexture(bool multisampled) {
			Texture::IDType name;

			if (multisampled)
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &name);
			else
				glCreateTextures(GL_TEXTURE_2D, 1, &name);

			return name;
		}

		constexpr GLenum GLInternalFormat(Texture::InternalFormat format) {
			switch (format) {
			case Texture::InternalFormat::CompressedRed:
				return GL_COMPRESSED_RED;
			case Texture::InternalFormat::CompressedRedRGTC1:
				return GL_COMPRESSED_RED_RGTC1;
			case Texture::InternalFormat::CompressedRG:
				return GL_COMPRESSED_RG;
			case Texture::InternalFormat::CompressedRGB:
				return GL_COMPRESSED_RGB;
			case Texture::InternalFormat::CompressedRGBA:
				return GL_COMPRESSED_RGBA;
			case Texture::InternalFormat::CompressedRGRGTC2:
				return GL_COMPRESSED_RG_RGTC2;
			case Texture::InternalFormat::CompressedSignedRedRGTC1:
				return GL_COMPRESSED_SIGNED_RED_RGTC1;
			case Texture::InternalFormat::CompressedSignedRGRGTC2:
				return GL_COMPRESSED_SIGNED_RG_RGTC2;
			case Texture::InternalFormat::CompressedSRGB:
				return GL_COMPRESSED_SRGB;
			case Texture::InternalFormat::DepthStencil:
				return GL_DEPTH_STENCIL;
			case Texture::InternalFormat::Depth24Stencil8:
				return GL_DEPTH24_STENCIL8;
			case Texture::InternalFormat::Depth32FStencil8:
				return GL_DEPTH32F_STENCIL8;
			case Texture::InternalFormat::DepthComponent:
				return GL_DEPTH_COMPONENT;
			case Texture::InternalFormat::DepthComponent16:
				return GL_DEPTH_COMPONENT16;
			case Texture::InternalFormat::DepthComponent24:
				return GL_DEPTH_COMPONENT24;
			case Texture::InternalFormat::DepthComponent32F:
				return GL_DEPTH_COMPONENT32F;
			case Texture::InternalFormat::DepthComponent32:
				return GL_DEPTH_COMPONENT32;
			case Texture::InternalFormat::R16F:
				return GL_R16F;
			case Texture::InternalFormat::R16I:
				return GL_R16I;
			case Texture::InternalFormat::R16SNorm:
				return GL_R16_SNORM;
			case Texture::InternalFormat::R16UI:
				return GL_R16UI;
			case Texture::InternalFormat::R32F:
				return GL_R32F;
			case Texture::InternalFormat::R32I:
				return GL_R32I;
			case Texture::InternalFormat::R32UI:
				return GL_R32UI;
			case Texture::InternalFormat::R3G3B2:
				return GL_R3_G3_B2;
			case Texture::InternalFormat::R8:
				return GL_R8;
			case Texture::InternalFormat::R8I:
				return GL_R8I;
			case Texture::InternalFormat::R8SNorm:
				return GL_R8_SNORM;
			case Texture::InternalFormat::R8UI:
				return GL_R8UI;
			case Texture::InternalFormat::Red:
				return GL_RED;
			case Texture::InternalFormat::RG:
				return GL_RG;
			case Texture::InternalFormat::RG16:
				return GL_RG16;
			case Texture::InternalFormat::RG16F:
				return GL_RG16F;
			case Texture::InternalFormat::RG16SNorm:
				return GL_RG16_SNORM;
			case Texture::InternalFormat::RG32F:
				return GL_RG32F;
			case Texture::InternalFormat::RG32I:
				return GL_RG32I;
			case Texture::InternalFormat::RG32UI:
				return GL_RG32UI;
			case Texture::InternalFormat::RG8:
				return GL_RG8;
			case Texture::InternalFormat::RG8I:
				return GL_RGB8I;
			case Texture::InternalFormat::RG8SNorm:
				return GL_RG8_SNORM;
			case Texture::InternalFormat::RG8UI:
				return GL_RG8UI;
			case Texture::InternalFormat::RGB:
				return GL_RGB;
			case Texture::InternalFormat::RGB10:
				return GL_RGB10;
			case Texture::InternalFormat::RGB10A2:
				return GL_RGB10_A2;
			case Texture::InternalFormat::RGB12:
				return GL_RGB12;
			case Texture::InternalFormat::RGB16:
				return GL_RGB16;
			case Texture::InternalFormat::RGB16F:
				return GL_RGB16F;
			case Texture::InternalFormat::RGB16I:
				return GL_RGB16I;
			case Texture::InternalFormat::RGB16UI:
				return GL_RGB16UI;
			case Texture::InternalFormat::RGB32F:
				return GL_RGB32F;
			case Texture::InternalFormat::RGB32I:
				return GL_RGB32I;
			case Texture::InternalFormat::RGB32UI:
				return GL_RGB32UI;
			case Texture::InternalFormat::RGB4:
				return GL_RGB4;
			case Texture::InternalFormat::RGB5:
				return GL_RGB5;
			case Texture::InternalFormat::RGB5A1:
				return GL_RGB5_A1;
			case Texture::InternalFormat::RGB8:
				return GL_RGB8;
			case Texture::InternalFormat::RGB8I:
				return GL_RGB8I;
			case Texture::InternalFormat::RGB8UI:
				return GL_RGB8UI;
			case Texture::InternalFormat::RGB9E5:
				return GL_RGB9_E5;
			case Texture::InternalFormat::RGBA:
				return GL_RGBA;
			case Texture::InternalFormat::RGBA12:
				return GL_RGBA12;
			case Texture::InternalFormat::RGBA16:
				return GL_RGBA16;
			case Texture::InternalFormat::RGBA16F:
				return GL_RGBA16F;
			case Texture::InternalFormat::RGBA16I:
				return GL_RGBA16I;
			case Texture::InternalFormat::RGBA16UI:
				return GL_RGBA16UI;
			case Texture::InternalFormat::RGBA2:
				return GL_RGBA2;
			case Texture::InternalFormat::RGBA32F:
				return GL_RGBA32F;
			case Texture::InternalFormat::RGBA32I:
				return GL_RGBA32I;
			case Texture::InternalFormat::RGBA32UI:
				return GL_RGBA32UI;
			case Texture::InternalFormat::RGBA4:
				return GL_RGBA4;
			case Texture::InternalFormat::RGBA8:
				return GL_RGBA8;
			case Texture::InternalFormat::RGBA8UI:
				return GL_RGBA8I;
			case Texture::InternalFormat::SRGB8:
				return GL_SRGB8;
			case Texture::InternalFormat::SRGB8A8:
				return GL_SRGB8_ALPHA8;
			case Texture::InternalFormat::SRGBA:
				return GL_SRGB_ALPHA;
			}
		}

		constexpr static GLenum GLDataType(Texture::DataType dataType) {
			switch (dataType) {
			case Texture::DataType::Byte:
				return GL_BYTE;
			case Texture::DataType::UnsignedByte:
				return GL_UNSIGNED_BYTE;
			case Texture::DataType::Short:
				return GL_SHORT;
			case Texture::DataType::UnsignedShort:
				return GL_UNSIGNED_SHORT;
			case Texture::DataType::Int:
				return GL_INT;
			case Texture::DataType::UnsignedInt:
				return GL_UNSIGNED_INT;
			case Texture::DataType::Float:
				return GL_FLOAT;
			case Texture::DataType::Double:
				return GL_DOUBLE;
			}
		}

		constexpr GLenum GLFormat(Texture::DataFormat format) {
			switch (format) {
			case Texture::DataFormat::Red:
				return GL_RED;
			case Texture::DataFormat::RG:
				return GL_RG;
			case Texture::DataFormat::RGB:
				return GL_RGB;
			case Texture::DataFormat::BGR:
				return GL_BGR;
			case Texture::DataFormat::RGBA:
				return GL_RGBA;
			case Texture::DataFormat::BGRA:
				return GL_BGRA;
			case Texture::DataFormat::RedInteger:
				return GL_RED_INTEGER;
			case Texture::DataFormat::RGInteger:
				return GL_RG_INTEGER;
			case Texture::DataFormat::RGBInteger:
				return GL_RGB_INTEGER;
			case Texture::DataFormat::BGRInteger:
				return GL_BGR_INTEGER;
			case Texture::DataFormat::RGBAInteger:
				return GL_RGBA_INTEGER;
			case Texture::DataFormat::BGRAInteger:
				return GL_BGRA_INTEGER;
			case Texture::DataFormat::StencilIndex:
				return GL_STENCIL_INDEX;
			case Texture::DataFormat::DepthComponent:
				return GL_DEPTH_COMPONENT;
			case Texture::DataFormat::DepthStencil:
				return GL_DEPTH_STENCIL;
			}
		}

		constexpr static int GetOpenWrapping(Wrapping wrapping) {
			switch (wrapping) {
			case Wrapping::Repeat:
				return GL_REPEAT;
			case Wrapping::MirroredRepeat:
				return GL_MIRRORED_REPEAT;
			case Wrapping::ClampEdge:
				return GL_CLAMP_TO_EDGE;
			case Wrapping::ClampBorder:
				return GL_CLAMP_TO_BORDER;
			}
		}

		constexpr int GetOpenFilter(Filter filter) {
			switch (filter) {
			case Filter::Nearest:
				return GL_NEAREST;
			case Filter::Linear:
				return GL_LINEAR;
			case Filter::NearestMipmapNearest:
				return GL_NEAREST_MIPMAP_NEAREST;
			case Filter::LinearMipmapNearest:
				return GL_LINEAR_MIPMAP_NEAREST;
			case Filter::NearestMipmapLinear:
				return GL_NEAREST_MIPMAP_LINEAR;
			case Filter::LinearMipmapLinear:
				return GL_LINEAR_MIPMAP_LINEAR;
			}
		}
	}

	void Texture::Resize(const Vector2u& size) {
		if (size.Width != m_Internals->Size.Width || size.Height != m_Internals->Size.Height) {
			const auto Image = ToImage();

			m_Internals->Invalidate();
			m_Internals->Allocate(size, m_Internals->Samples, m_Internals->InternalFormat);

			Update(Image);
		}
	}

	Texture::Texture(bool multisampled) : m_Internals(MakeRef<Internals>(multisampled)) {
	}

	void Texture::GenerateMipMaps() const {
		m_Internals->MipMapGenerated = true;

		glGenerateTextureMipmap(m_Internals->ID);
	}

	void Texture::Bind() const {
		m_Internals->Bind();
	}

	void Texture::BindUnit(uint32_t sampler) const {
		m_Internals->BindUnit(sampler);
	}

	Ref<Texture> Texture::Create(const Vector2u& size, InternalFormat internalFormat, const uint8_t* pixels,
	                             DataType dataType, DataFormat dataFormat) {
		auto texture = Ref<Texture>(new Texture());

		if (CheckSize(size))
			texture->CreateTexture(1, size, internalFormat, pixels, dataType, dataFormat);

		return texture;
	}

	Ref<Texture> Texture::Create(const Vector2u& size, uint32_t samples, InternalFormat internalFormat,
	                             const uint8_t* pixels, DataType dataType, DataFormat dataFormat) {
		auto texture = Ref<Texture>(new Texture(samples > 1));

		if (CheckSize(size))
			texture->CreateTexture(samples, size, internalFormat, pixels, dataType, dataFormat);

		return texture;
	}

	Ref<Texture> Texture::Create(Ref<Image> image) {
		auto texture = Ref<Texture>(new Texture());

		if (CheckSize(image->Size()))
			texture->CreateTexture(1, image->Size(), InternalFormat::RGBA8, image->GetPixels().data(),
			                       DataType::UnsignedByte, DataFormat::RGBA);

		return texture;
	}

	void Texture::SetWrapping(Wrapping s) {
		SetWrappingS(s);
	}

	void Texture::SetWrapping(Wrapping s, Wrapping t) {
		SetWrappingS(s);
		SetWrappingS(t);
	}

	void Texture::SetWrappingS(Wrapping wrapping) {
		m_Internals->Wrapping.S = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_S, Utils::GetOpenWrapping(wrapping));
	}

	void Texture::SetWrappingT(Wrapping wrapping) {
		m_Internals->Wrapping.T = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_T, Utils::GetOpenWrapping(wrapping));
	}

	void Texture::SetFilters(Filter min, Filter mag) {
		SetMinFilter(min);
		SetMagFilter(mag);
	}

	void Texture::SetMinFilter(Filter filter) {
		m_Internals->Filter.Min = filter;
		m_Internals->SetParameter(GL_TEXTURE_MIN_FILTER, Utils::GetOpenFilter(filter));
	}

	void Texture::SetMagFilter(Filter filter) {
		if (filter > Filter::Linear) {
			LOG_ENGINE_ERROR("Only possible values for mag filter is Nearest or Linear");
			return;
		}

		m_Internals->Filter.Mag = filter;
		m_Internals->SetParameter(GL_TEXTURE_MAG_FILTER, Utils::GetOpenFilter(filter));
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

	void Texture::CreateTexture(uint32_t samples, const Vector2u& size, InternalFormat internalFormat,
	                            const void* pixels, DataType dataType, DataFormat dataFormat) {
		if (samples > 1)
			m_Internals->Allocate(size, samples, internalFormat);
		else {
			m_Internals->Allocate(size, internalFormat);

			SetFilters(Filter::Nearest, Filter::Nearest);
			SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
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

	void Texture::Internals::Bind() const {
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Texture::Internals::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, ID);
	}

	void Texture::Internals::Allocate(const Vector2u& size, enum InternalFormat internalFormat) {
		Size = size;
		InternalFormat = internalFormat;
		Samples = 1;

		glTextureStorage2D(ID, 1, Utils::GLInternalFormat(internalFormat), static_cast<GLsizei>(size.Width),
		                   static_cast<GLsizei>(size.Height));
	}

	void Texture::Internals::Allocate(const Vector2u& size, uint32_t samples, enum InternalFormat internalFormat) {
		Size = size;
		InternalFormat = internalFormat;
		Samples = samples;

		glTextureStorage2DMultisample(ID, static_cast<GLsizei>(samples), Utils::GLInternalFormat(internalFormat),
		                              static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), GL_FALSE);
	}

	void Texture::Internals::SendImage(const void* pixels, const Vector2u& size, const Vector2i& offset,
	                                   DataFormat format, DataType dataType) {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		CheckSubRegionSize(offset, size);

		glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y),
		                    Utils::GLFormat(format), Utils::GLDataType(dataType), pixels);
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
		glClearTexImage(ID, 0, Utils::GLFormat(dataFormat), Utils::GLDataType(dataType), pixels);
	}

	void Texture::Internals::Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                               DataType dataType) {
		CheckSubRegionSize(offset, size);

		glClearTexSubImage(ID, 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, Utils::GLFormat(dataFormat),
		                   Utils::GLDataType(dataType), nullptr);
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
