#include "pch.h"
#include "Engine/OpenGL/Texture.h"

#include "Engine/Core/Assert.h"
#include "Engine/Renderer/Context.h"
#include "Engine/OpenGL/CubeMap.h"

#include <vector>
#include <glad/glad.h>


namespace Game
{
	Texture::Internals::Internals()
	{
		Functions = Context::GetContext()->GetFunctions();
		ID = Functions.GenTexture();
	}

	Texture::Internals::~Internals()
	{
		Functions.DeleteTexture(ID);
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

	void Texture::SetWrapping(Wrapping s)
	{
		m_Internals->Wrapping.S = s;

		SetParameter(TextureParamName::WrapS, static_cast<int>(s));
	}

	void Texture::SetWrapping(Wrapping s, Wrapping t)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;

		SetParameter(TextureParamName::WrapS, static_cast<int>(s));
		SetParameter(TextureParamName::WrapT, static_cast<int>(t));
	}

	void Texture::SetWrappingS(Wrapping wrapping)
	{
		m_Internals->Wrapping.S = wrapping;

		SetParameter(TextureParamName::WrapS, static_cast<int>(wrapping));
	}

	void Texture::SetWrappingT(Wrapping wrapping)
	{
		m_Internals->Wrapping.T = wrapping;

		SetParameter(TextureParamName::WrapT, static_cast<int>(wrapping));
	}

	void Texture::SetFilters(Filter min, Filter mag)
	{
		m_Internals->Filter.Min = min;
		m_Internals->Filter.Mag = mag;

		SetParameter(TextureParamName::MinFilter, static_cast<int>(min));
		SetParameter(TextureParamName::MagFilter, static_cast<int>(mag));
	}

	void Texture::SetMinFilter(Filter min)
	{
		m_Internals->Filter.Min = min;

		SetParameter(TextureParamName::MinFilter, static_cast<int>(min));
	}

	void Texture::SetMagFilter(Filter mag)
	{
		m_Internals->Filter.Mag = mag;

		SetParameter(TextureParamName::MagFilter, static_cast<int>(mag));
	}

	void Texture::Bind() const
	{
		m_Internals->Functions.BindTexture(TextureTarget::Texture2D, m_Internals->ID);
	}

	void Texture::GenerateMipMaps() const
	{
		m_Internals->MipmapGenerated = true;

		m_Internals->Functions.GenerateMipMap(m_Internals->ID);
	}

	void Texture::Create(uint32_t width, uint32_t height)
	{
		if(width == 0 || height == 0)
		{
			GL_LOG_WARN("Trying to create texture of size 0");
			return;
		}

		const uint32_t maxSize = GetMaxSize();

		if(width > maxSize || height > maxSize)
		{
			GL_LOG_ERROR(
			             "unable to create texture of size {0:}x{1:}, texture to big. Max size is {2:}x{2:}",
			             width,
			             height,
			             maxSize
			            );
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
		{
			GL_LOG_WARN("Trying to create texture of size 0");
			return;
		}

		const uint32_t maxSize = GetMaxSize();

		if(width > maxSize || height > maxSize)
		{
			GL_LOG_ERROR(
			             "unable to create texture of size {0:}x{1:}, texture to big. Max size is {2:}x{2:}",
			             width,
			             height,
			             maxSize
			            );
			return;
		}

		Image2D(image.GetPixels(), DataType::UnsignedByte, Format::Rgba, width, height, InternalFormat::RGBA8);
		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	Image Texture::ToImage() const
	{
		const size_t size = static_cast<size_t>(m_Internals->Size.Width) * m_Internals->Size.Height;

		std::vector<Color> pixels;
		pixels.resize(size);

		m_Internals->Functions.GetTextureImage(m_Internals->ID, 0, Format::Rgba, DataType::UnsignedByte, static_cast<uint32_t>(size), &pixels[0]);

		return Image(size, pixels.data());
	}

	void Texture::Update(const uint8_t *pixels)
	{
		Update(pixels, m_Internals->Size.Width, m_Internals->Size.Height, 0, 0);
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

	void Texture::Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset)
	{
		Update(pixels, size.Width, size.Height, offset.X, offset.Y);
	}

	void Texture::Update(const Color *pixels)
	{
		Update(pixels, m_Internals->Size.Width, m_Internals->Size.Width, 0, 0);
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
		Update(texture.ToImage(), offset.X, offset.Y);
	}

	void Texture::Update(const CubeTexture &texture)
	{
		Update(texture, 0, 0);
	}

	void Texture::Update(const CubeTexture &texture, int32_t x, int32_t y)
	{
		Update(texture.ToImage(), x, y);
	}

	void Texture::Update(const CubeTexture &texture, const Vector2i &offset)
	{
		Update(texture.ToImage(), offset.X, offset.Y);
	}

	void Texture::Resize(uint32_t width, uint32_t height)
	{
		ASSERT(width > 0 && height > 0, "Texture dimensions cant be 0");

		Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, width, height, InternalFormat::RGBA8);
	}

	void Texture::Resize(const Vector2u &newSize)
	{
		Resize(newSize.Width, newSize.Height);
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

			size = Context::GetContext()->GetFunctions().GetInteger(GL_MAX_TEXTURE_SIZE);
		}

		return static_cast<uint32_t>(size);
	}

	void Texture::Image2D(
		const void *pixels,
		DataType type,
		Format format,
		const Vector2u &size,
		InternalFormat internalFormat
		)
	{
		m_Internals->Size   = size;
		m_Internals->Format = internalFormat;


		m_Internals->Functions.Image2DBind(m_Internals->ID, TextureTarget::Texture2D, 0, internalFormat, size, 0, format, type, pixels);
	}

	void Texture::SubImage2D(
		const void *pixels,
		DataType type,
		Format format,
		const Vector2i &offset,
		const Vector2u &size
		)
	{
		if(!pixels)
			return;

		ASSERT(
		       offset.X + size.Width < m_Internals->Size.Width && offset.Y + size.Height < m_Internals->Size.Height,
		       "Texture size out of range"
		      );

		if(offset.X + size.Width >= m_Internals->Size.Width || offset.Y + size.Height >= m_Internals->Size.Height)
			throw std::out_of_range("Texture size out of range");

		m_Internals->Functions.SubImage2D(m_Internals->ID, 0, offset, size, format, type, pixels);
	}

	void Texture::Image2D(
		const void *pixels,
		DataType type,
		Format format,
		uint32_t width,
		uint32_t height,
		InternalFormat internalFormat
		)
	{
		m_Internals->Size   = {width, height};
		m_Internals->Format = internalFormat;

		m_Internals->Functions.Image2DBind(m_Internals->ID, TextureTarget::Texture2D, 0, internalFormat, width, height, 0, format, type, pixels);
	}

	void Texture::SubImage2D(
		const void *pixels,
		DataType type,
		Format format,
		uint32_t x,
		uint32_t y,
		uint32_t width,
		uint32_t height
		)
	{
		if(!pixels)
			return;

		ASSERT(
		       x + width < m_Internals->Size.Width && y + height < m_Internals->Size.Height,
		       "Texture size out of range"
		      );
		if(x + width >= m_Internals->Size.Width || y + height >= m_Internals->Size.Height)
			throw std::out_of_range("Texture size out of range");

		m_Internals->Functions.SubImage2D(m_Internals->ID, 0, x, y, width, height, format, type, pixels);
	}

	void Texture::SetParameter(TextureParamName name, int parameter)
	{
		m_Internals->Functions.TextureParameter(m_Internals->ID, name, parameter);
	}
}
