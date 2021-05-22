#include "pch.h"
#include "Texture.h"

#include <vector>
#include <glad/glad.h>

#include "Assert.h"
#include "GLCheck.h"
#include "CubeMap.h"

#include "OpenGlFunctions.h"

namespace Game
{
	Texture::Internals::Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glGenTextures(1, &Id));
	}

	Texture::Internals::~Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glDeleteTextures(1, &Id));
	}

	Texture::Texture()
	{
		m_Internals = MakePointer<Internals>();
	}

	Texture::Texture(uint32_t width, uint32_t height) : Texture()
	{
		Create(width, height);
	}

	Texture::Texture(const Vector2u &size) : Texture()
	{
		Create(size);
	}

	Texture::Texture(const Image &image) : Texture()
	{
		Create(image);
	}

	void Texture::SetWrapping(const Wrapping s)
	{
		m_Internals->Wrapping.S = s;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(t));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t, const Wrapping r)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;
		m_Internals->Wrapping.R = r;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(t));
		SetParameter(GL_TEXTURE_WRAP_R, static_cast<int>(r));
	}

	void Texture::SetWrappingS(Wrapping wrapping)
	{
		m_Internals->Wrapping.S = wrapping;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
	}

	void Texture::SetWrappingT(Wrapping wrapping)
	{
		m_Internals->Wrapping.T = wrapping;

		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(wrapping));
	}

	void Texture::SetWrappingR(Wrapping wrapping)
	{
		m_Internals->Wrapping.R = wrapping;

		SetParameter(GL_TEXTURE_WRAP_R, static_cast<int>(wrapping));
	}

	void Texture::SetFilters(Filter min, Filter mag)
	{
		m_Internals->Filter.Min = min;
		m_Internals->Filter.Mag = mag;

		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void Texture::SetMinFilter(Filter min)
	{
		m_Internals->Filter.Min = min;

		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
	}

	void Texture::SetMagFilter(Filter mag)
	{
		m_Internals->Filter.Mag = mag;

		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void Texture::Bind() const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, *this));
	}

	void Texture::GenerateMipMaps() const
	{
		m_Internals->MipmapGenerated = true;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glGenerateTextureMipmap(m_Internals->Id));
	}

	void Texture::Create(const uint32_t width, const uint32_t height)
	{
		if(width == 0 || height == 0)
			return;

		const uint32_t maxSize = GetMaxSize();

		if(width > maxSize || height > maxSize)
		{
			GL_LOG_ERROR("Unable to create texture of size {}x{}, texture to big max texture size is {}x{}", width, height, maxSize, maxSize);
			return;
		}


		Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, width, height, InternalFormat::RGBA8);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture::Create(const Vector2u &size)
	{
		Create(size.Width, size.Height);
	}

	void Texture::Create(const Image &image)
	{
		const uint32_t width  = image.Width();
		const uint32_t height = image.Height();

		if(width == 0 || height == 0)
			return;

		const uint32_t maxSize = GetMaxSize();

		if(width > maxSize || height > maxSize)
		{
			GL_LOG_ERROR("Unable to create texture of size {}x{}, texture to big max texture size is {}x{}", width, height, maxSize, maxSize);
			return;
		}

		Image2D(image.GetPixels(), DataType::UnsignedByte, Format::Rgba, image.Width(), image.Height(), InternalFormat::RGBA8);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	Image Texture::ToImage() const
	{
		const size_t size = static_cast<size_t>(m_Internals->Size.Width) * m_Internals->Size.Height;

		std::vector<Color> pixels;
		pixels.resize(size);

		CHECK_IF_VALID_CONTEXT Image(0, 0);
		GL_CHECK(glGetTextureImage(m_Internals->Id, 0, GL_RGBA, GL_UNSIGNED_BYTE, size, &pixels[0]));

		return Image(m_Internals->Size, pixels.data());
	}

	void Texture::Update(const uint8_t *pixels)
	{
		Update(pixels, m_Internals->Size.Width, m_Internals->Size.Height, 0, 0);
	}

	void Texture::Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset)
	{
		Update(pixels, size.Width, size.Height, offset.X, offset.Y);
	}

	void Texture::Update(const Color *pixels)
	{
		Update(pixels, m_Internals->Size.Width, m_Internals->Size.Height, 0, 0);
	}

	void Texture::Update(const Color *pixels, const Vector2u &size, const Vector2i &offset)
	{
		Update(pixels, size.Width, size.Height, offset.X, offset.Y);
	}

	void Texture::Update(const Texture &texture)
	{
		Update(texture, 0, 0);
	}

	void Texture::Update(const Texture &texture, int32_t x, int32_t y)
	{
		Update(texture.ToImage(), x, y);
	}

	void Texture::Update(const Texture &texture, const Vector2i &offset)
	{
		return Update(texture.ToImage(), offset.X, offset.Y);
	}

	void Texture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		ASSERT(x + width < m_Internals->Size.X && y + height < m_Internals->Size.Y, "Out of range");
		if(!(x + width >= m_Internals->Size.X && y + height >= m_Internals->Size.Y))
			throw std::out_of_range("Out of range");

		if(pixels)
		{
			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, x, y, width, height);
		}
	}

	void Texture::Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		ASSERT(x + width < m_Internals->Size.X && y + height < m_Internals->Size.Y, "Out of range");
		if(!(x + width >= m_Internals->Size.X && y + height >= m_Internals->Size.Y))
			throw std::out_of_range("Out of range");

		if(pixels)
		{
			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, x, y, width, height);
		}
	}


	void Texture::Update(const CubeTexture &texture)
	{
		return Update(texture, 0, 0);
	}

	void Texture::Update(const CubeTexture &texture, int32_t x, int32_t y)
	{
		return Update(texture.ToImage(), x, y);
	}

	void Texture::Update(const CubeTexture &texture, const Vector2i &offset)
	{
		return Update(texture.ToImage(), offset.X, offset.Y);
	}

	void Texture::Resize(uint32_t width, uint32_t height)
	{
		ASSERT(width> 0 && height > 0, "Texture dimensions cant be 0");

		Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, width, height, InternalFormat::RGBA8);
	}

	void Texture::Swap(Texture &right) noexcept
	{
		std::swap(m_Internals, right.m_Internals);
	}

	uint32_t Texture::GetMaxSize()
	{
		static bool checked = false;
		static int32_t size = 0;

		if(!checked)
		{
			checked = true;

			CHECK_IF_VALID_CONTEXT size;
			GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size));
		}

		return static_cast<uint32_t>(size);
	}

	void Texture::Image2D(const void *pixels, DataType type, Format format, const Vector2<uint32_t> &size, InternalFormat internalFormat)
	{
		m_Internals->Size   = size;
		m_Internals->Format = internalFormat;

		Bind();
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(
		         glTexImage2D( GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), size.Width, size.Height, 0, static_cast<GLenum>(format), static_cast<GLenum
			         >(type), pixels )
		        );
	}

	void Texture::SubImage2D(const void *pixels, DataType type, Format format, const Vector2i &offset, const Vector2u &size)
	{
		if(!pixels)
			return;

		ASSERT(offset.X + size.Width < m_Internals->Size.Width && offset.Y + size.Height < m_Internals->Size.Height, "Texture size out of range");
		if(offset.X + size.Width > m_Internals->Size.Width || offset.Y + size.Height > m_Internals->Size.Height)
			throw std::out_of_range("Texture size out of range");

		Bind();
		GL_CHECK(
		         glTexSubImage2D(GL_TEXTURE_2D, 0, offset.X, offset.Y, size.Width, size.Height, static_cast<GLenum>(format), static_cast<GLenum>(type), pixels)
		        );
	}

	void Texture::Image2D(const void *pixels, DataType type, Format format, uint32_t width, uint32_t height, InternalFormat internalFormat)
	{
		m_Internals->Size   = Vector2u(width, height);
		m_Internals->Format = internalFormat;

		Bind();
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(
		         glTexImage2D( GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, static_cast<GLenum>(format), static_cast< GLenum>(type),
			         pixels )
		        );
	}

	void Texture::SubImage2D(const void *pixels, DataType type, Format format, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		if(!pixels)
			return;

		ASSERT(x + width < m_Internals->Size.Width && y + height < m_Internals->Size.Height, "Texture size out of range");
		if(x + width > m_Internals->Size.Width || y + height > m_Internals->Size.Height)
			throw std::out_of_range("Texture size out of range");

		Bind();

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, static_cast<GLenum>(format), static_cast<GLenum>(type), pixels));
	}

	void Texture::SetParameter(uint32_t name, int parameter)
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glTextureParameteri(m_Internals->Id, name, parameter));
	}
}
