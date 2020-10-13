#include "pch.h"
#include "Texture.h"

#include <vector>

#include "Assert.h"

namespace Game
{
	void DeleteTexture(Texture::IdType *id)
	{
		glDeleteTextures(1, id);

		delete id;
	}

	Pointer<Texture::IdType> CreateTexture()
	{
		auto texture = Pointer<Texture::IdType>(new Texture::IdType(), DeleteTexture);
		glGenTextures(1, &*texture);

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

		m_Size       = Vector2u(width, height);
		m_ActualSize = Vector2u(GetValidSize(width), GetValidSize(height));

		glTexImage2D(
		             GL_TEXTURE_2D,
		             0,
		             static_cast<GLint>(internalFormat),
		             m_ActualSize.Width,
		             m_ActualSize.Height,
		             0,
		             static_cast<GLenum>(format),
		             static_cast<GLenum>(type),
		             data
		            );
	}

	void Texture::SetWrapping(const Wrapping s)
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t)
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(s));
	}

	void Texture::SetWrapping(const Wrapping s, const Wrapping t, const Wrapping r)
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLint>(s));
	}

	void Texture::SetFilters(Filter min, Filter mag)
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag));
	}

	void Texture::SetBorderColor(const glm::vec4 &color)
	{
		Bind();
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color.x);
	}

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, *this);
	}

	void Texture::GenerateMipMaps() const
	{
		Bind();
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture::Create(const uint32_t width, const uint32_t height)
	{
		if(width == 0 || height == 0)
			return;

		Vector2u actualSize(GetValidSize(width), GetValidSize(height));
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
		std::vector<glm::vec4> pixels(m_Size.Width * m_Size.Height);

		if(m_Size == m_ActualSize)
		{
			Bind();
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
		}
		else
		{
			std::vector<glm::vec4> allPixels(m_ActualSize.Width * m_ActualSize.Height);
			Bind();
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &allPixels[0]);
			
			const glm::vec4 *src = &allPixels[0];
			glm::vec4 *dst       = &pixels[0];

			int srcPitch = m_ActualSize.X * 4;
			int dstPitch = m_ActualSize.Y * 4;

			for(size_t i = 0; i < m_Size.Height; ++i)
			{
				std::memcpy(dst, src, dstPitch);
				src += srcPitch;
				dst += dstPitch;
			}
		}

		return Image(m_Size, pixels.data());
	}

	void Texture::Update(
		const glm::vec4 *pixels,
		const uint32_t width,
		const uint32_t height,
		const uint32_t x,
		const uint32_t y
		)
	{
		ASSERT(x + width <= m_Size.X && y + height <= m_Size.Y, "Out of range");
		if(!(x + width <= m_Size.X && y + height <= m_Size.Y))
			throw std::out_of_range("Out of range");

		if(pixels)
		{
			Bind();
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_FLOAT, pixels);
			glFlush();
		}
	}

	void Texture::Swap(Texture &right) noexcept
	{
		std::swap(m_Size, right.m_Size);
		std::swap(m_ActualSize, right.m_ActualSize);
		std::swap(m_Texture, right.m_Texture);
	}

	uint32_t Texture::GetMaxSize()
	{
		static bool checked = false;
		static int32_t size = 0;

		if(!checked)
		{
			checked = true;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		}

		return static_cast<uint32_t>(size);
	}

	uint32_t Texture::GetValidSize(const uint32_t size)
	{
		if constexpr(true)
			return size;
		else
		{
			uint32_t pow = 1;
			while(pow < size)
				pow = pow << 1;

			return pow;
		}
	}
}
