#include "pch.h"
#include "Texture.h"

#include <vector>
#include <glad/glad.h>

#include "Assert.h"
#include "GLCheck.h"

namespace Game
{
	void DeleteTexture(Texture::IdType *id)
	{
		GL_CHECK(glDeleteTextures(1, id));

		delete id;
	}

	Pointer<Texture::IdType> CreateTexture()
	{
		auto texture = Pointer<Texture::IdType>(new Texture::IdType(), DeleteTexture);
		GL_CHECK(glGenTextures(1, &*texture));

		return texture;
	}

	Texture::Texture()
	{
		m_Texture = CreateTexture();
	}

	void Texture::SetWrapping(const Wrapping s)
	{
		m_S = s;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t)
	{
		m_T = t;

		SetWrapping(s);
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(t));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t, const Wrapping r)
	{
		m_R = r;

		SetWrapping(s, t);
		SetParameter(GL_TEXTURE_WRAP_R, static_cast<int>(r));
	}

	void Texture::SetWrappingS(Wrapping wrapping)
	{
		m_S = wrapping;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
	}

	void Texture::SetWrappingT(Wrapping wrapping)
	{
		m_T = wrapping;
		
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(wrapping));
	}

	void Texture::SetWrappingR(Wrapping wrapping)
	{
		m_R = wrapping;
		
		SetParameter(GL_TEXTURE_WRAP_R, static_cast<int>(wrapping));
	}

	void Texture::SetFilters(Filter min, Filter mag)
	{
		m_Min = min;
		m_Mag = mag;

		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void Texture::SetMinFilter(Filter min)
	{
		m_Min = min;
		
		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
	}

	void Texture::SetMagFilter(Filter mag)
	{
		m_Mag = mag;
		
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void Texture::SetBorderColor(const Color &color)
	{
		m_BorderColor = color;

		SetParameter(GL_TEXTURE_BORDER_COLOR, color);
	}

	void Texture::Bind() const
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, *this));
	}

	void Texture::GenerateMipMaps() const
	{
		m_MipmapGenerated = true;

		GL_CHECK(glGenerateTextureMipmap(*m_Texture));
	}

	void Texture::Create(const uint32_t width, const uint32_t height)
	{
		if(width == 0 || height == 0)
			return;

		Vector2u actualSize(width, height);
		uint32_t maxSize = GetMaxSize();

		if(actualSize.X > maxSize || actualSize.Y > maxSize)
		{
			GL_LOG_ERROR("Unable to create texture of size {}x{}, texture to big max texture size is {}x{}", actualSize.X, actualSize.Y, maxSize, maxSize);
			return;
		}

		m_Size.Width  = width;
		m_Size.Height = height;

		Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, actualSize.Width, actualSize.Height, InternalFormat::RGBA);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture::Create(const Image &image)
	{
		Image2D(image.GetPixels(), DataType::UnsignedByte, Format::Rgba, image.Width(), image.Height(), InternalFormat::RGBA);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	Image Texture::ToImage() const
	{
		const size_t size = static_cast<size_t>(m_Size.Width) * m_Size.Height;
		
		std::vector<Color> pixels;
		pixels.resize(size);

		GL_CHECK(glGetTextureImage(*m_Texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, size, &pixels[0]));

		return Image(m_Size, pixels.data());
	}

	void Texture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
	{
		ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
		if(!(x + width >= m_Size.X && y + height >= m_Size.Y))
			throw std::out_of_range("Out of range");

		if(pixels)
		{
			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, 0, x, y, width, height);
		}
	}

	void Texture::Update(const Color *pixels, const uint32_t width, const uint32_t height, const uint32_t x, const uint32_t y)
	{
		ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
		if(!(x + width >= m_Size.X && y + height >= m_Size.Y))
			throw std::out_of_range("Out of range");

		if(pixels)
		{
			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, 0, x, y, width, height);
		}
	}

	void Texture::Swap(Texture &right) noexcept
	{
		std::swap(m_Size, right.m_Size);
		std::swap(m_Texture, right.m_Texture);

		std::swap(m_InternalFormat, right.m_InternalFormat);

		std::swap(m_S, right.m_S);
		std::swap(m_T, right.m_T);
		std::swap(m_R, right.m_R);

		std::swap(m_Mag, right.m_Mag);
		std::swap(m_Min, right.m_Min);

		std::swap(m_BorderColor, right.m_BorderColor);
		std::swap(m_MipmapGenerated, right.m_MipmapGenerated);
	}

	uint32_t Texture::GetMaxSize()
	{
		static bool checked = false;
		static int32_t size = 0;

		if(!checked)
		{
			checked = true;

			GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size));
		}

		return static_cast<uint32_t>(size);
	}

	void Texture::Image2D(const void *data, DataType type, Format format, uint32_t width, uint32_t height, InternalFormat internalFormat)
	{
		m_Size           = Vector2u(width, height);
		m_InternalFormat = internalFormat;

		Bind();
		GL_CHECK(
		         glTexImage2D( GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), m_Size.Width, m_Size.Height, 0, static_cast<GLenum>(format), static_cast<
			         GLenum>(type), data )
		        );
		GL_CHECK(glFlush());
	}

	void Texture::SubImage2D(const void *data, DataType type, Format format, int level, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		Bind();

		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, level, x, y, width, height, static_cast<GLenum>(format), static_cast<GLenum>(type), data));
		GL_CHECK(glFlush());
	}

	void Texture::SetParameter(uint32_t name, int parameter)
	{
		GL_CHECK(glTextureParameteri(*m_Texture, name, parameter));
		GL_CHECK(glFlush());
	}

	void Texture::SetParameter(uint32_t name, Color color)
	{
		int table[4] = {color.R, color.G, color.B, color.A};
		GL_CHECK(glTextureParameteriv(*m_Texture, name, table));
	}
}
