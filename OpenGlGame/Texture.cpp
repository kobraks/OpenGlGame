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

	void Texture::Image2D(
		const void *data,
		const DataType type,
		const Format format,
		const uint32_t width,
		const uint32_t height,
		const InternalFormat internalFormat
		)
	{
		Bind();

		m_Size           = Vector2u(width, height);
		m_InternalFormat = internalFormat;

		GL_CHECK(glTexImage2D(
		             GL_TEXTURE_2D,
		             0,
		             static_cast<GLint>(internalFormat),
		             m_Size.Width,
		             m_Size.Height,
		             0,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            ));
	}

	void Texture::SetWrapping(const Wrapping s)
	{
		m_S = s;
		
		Bind();
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(s)));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t)
	{
		m_T = t;

		SetWrapping(s);
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(t)));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t, const Wrapping r)
	{
		m_R = r;
		
		SetWrapping(s, t);
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLint>(r)));
	}

	void Texture::SetWrappingS(Wrapping wrapping)
	{
		Bind();
		m_S = wrapping;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping)));
	}

	void Texture::SetWrappingT(Wrapping wrapping)
	{
		Bind();
		m_T = wrapping;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping)));
	}

	void Texture::SetWrappingR(Wrapping wrapping)
	{
		Bind();
		m_R = wrapping;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapping)));
	}

	void Texture::SetFilters(Filter min, Filter mag)
	{
		Bind();
		m_Min = min;
		m_Mag = mag;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min)));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag)));
	}

	void Texture::SetMinFilter(Filter min)
	{
		Bind();
		m_Min = min;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min)));
	}
	void Texture::SetMagFilter(Filter mag)
	{
		Bind();
		m_Mag = mag;
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag)));
	}

	void Texture::SetBorderColor(const Color &color)
	{
		Bind();
		m_BorderColor = color;
		GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color.R));
	}

	void Texture::Bind() const
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, *this));
	}

	void Texture::GenerateMipMaps() const
	{
		Bind();
		m_MipmapGenerated = true;
		GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	}

	void Texture::Create(const uint32_t width, const uint32_t height)
	{
		if(width == 0 || height == 0) return;

		Vector2u actualSize(width, height);
		uint32_t maxSize = GetMaxSize();

		if(actualSize.X > maxSize || actualSize.Y > maxSize)
		{
			GL_LOG_ERROR(
			             "Unable to create texture of size {}x{}, texture to big max texture size is {}x{}",
			             actualSize.X,
			             actualSize.Y,
			             maxSize,
			             maxSize
			            );
			return;
		}

		m_Size.Width  = width;
		m_Size.Height = height;

		Bind();
		Image2D(nullptr, DataType::Float, Format::RGBA, actualSize.Width, actualSize.Height, InternalFormat::RGBA);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture::Create(const Image &image)
	{
		Bind();
		Image2D(image.GetPixels(), DataType::Float, Format::RGBA, image.Width(), image.Height(), InternalFormat::RGBA);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	Image Texture::ToImage() const
	{
		std::vector<glm::vec4> pixels;
		pixels.resize(static_cast<size_t>(m_Size.Width) * m_Size.Height);
		
		Bind();
		GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels[0]));

		return Image(m_Size, pixels.data());
	}

	void Texture::Update(const float *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
	{
		ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
		if(!(x + width >= m_Size.X && y + height >= m_Size.Y)) throw std::out_of_range("Out of range");

		if (pixels)
		{
			Bind();
			GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_FLOAT, pixels));
			GL_CHECK(glFlush());
		}
	}

	void Texture::Update(
		const glm::vec4 *pixels,
		const uint32_t width,
		const uint32_t height,
		const uint32_t x,
		const uint32_t y
		)
	{
		ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
		if(!(x + width >= m_Size.X && y + height >= m_Size.Y)) throw std::out_of_range("Out of range");

		if(pixels)
		{
			Bind();
			GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_FLOAT, pixels));
			GL_CHECK(glFlush());
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
}
