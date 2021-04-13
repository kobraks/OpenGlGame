#include "pch.h"
#include "CubeMap.h"
#include "GLCheck.h"

#include "Texture.h"

namespace Game
{
	static constexpr GLenum GetTarget(CubeMap::Orientation orientation)
	{
		switch(orientation)
		{
			case CubeMap::Orientation::Right: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			case CubeMap::Orientation::Left: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			case CubeMap::Orientation::Top: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			case CubeMap::Orientation::Bottom: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			case CubeMap::Orientation::Back: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			case CubeMap::Orientation::Front: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			default: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		}
	}

	CubeMap::Internals::Internals()
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glGenTextures(1, &Id));
	}

	CubeMap::Internals::~Internals()
	{
		for(auto &texture : Textures)
			delete texture;

		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glDeleteTextures(1, &Id));
	}

	CubeMap::CubeMap()
	{
		m_Internals = MakePointer<Internals>();

		auto &textures = m_Internals->Textures;

		textures[0] = new CubeTexture(this, Orientation::Right);
		textures[1] = new CubeTexture(this, Orientation::Left);
		textures[2] = new CubeTexture(this, Orientation::Top);
		textures[3] = new CubeTexture(this, Orientation::Bottom);
		textures[4] = new CubeTexture(this, Orientation::Front);
		textures[5] = new CubeTexture(this, Orientation::Back);
	}

	CubeMap::CubeMap(const Vector2u &size)
	{
		const uint32_t maxSize = Texture::GetMaxSize();

		ASSERT(size.Width == size.Height, "Requested Cube size is not squerish");
		ASSERT(size.Width < maxSize && size.Height < maxSize, "Requested cubeMap size is to big");

		if(size.Width != size.Height)
		{
			throw std::runtime_error("Requested Cube size is not squerish");
		}

		if(size.Width >= maxSize || size.Height >= maxSize)
		{
			throw std::runtime_error("Requested cubeMap size is to big");
		}

		m_Internals->Size = size;

		for(auto &texture : m_Internals->Textures)
		{
			texture->Create(size);
		}
	}

	CubeMap::CubeMap(uint32_t width, uint32_t height) : CubeMap(Vector2u{width, height}) { }


	void CubeMap::SetWrapping(Wrapping s)
	{
		m_Internals->Wrapping.S = s;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
	}

	void CubeMap::SetWrapping(Wrapping s, Wrapping t)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(t));
	}

	void CubeMap::SetWrapping(Wrapping s, Wrapping t, Wrapping r)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;
		m_Internals->Wrapping.R = r;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(s));
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(t));
		SetParameter(GL_TEXTURE_WRAP_R, static_cast<int>(r));
	}

	void CubeMap::SetWrappingS(Wrapping wrapping)
	{
		m_Internals->Wrapping.S = wrapping;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
	}

	void CubeMap::SetWrappingT(Wrapping wrapping)
	{
		m_Internals->Wrapping.T = wrapping;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
	}

	void CubeMap::SetWrappingR(Wrapping wrapping)
	{
		m_Internals->Wrapping.R = wrapping;

		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
	}

	void CubeMap::SetFilters(Filter min, Filter mag)
	{
		m_Internals->Filter.Min = min;
		m_Internals->Filter.Mag = mag;

		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void CubeMap::SetMinFilter(Filter min)
	{
		m_Internals->Filter.Min = min;
		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(min));
	}

	void CubeMap::SetMagFilter(Filter mag)
	{
		m_Internals->Filter.Mag = mag;
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(mag));
	}

	void CubeMap::Bind() const
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Internals->Id));
	}

	void CubeMap::Swap(CubeMap &cubeMap)
	{
		std::swap(m_Internals, cubeMap.m_Internals);
	}

	void CubeMap::SetParameter(uint32_t name, int parameter)
	{
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(glTextureParameteri(m_Internals->Id, name, parameter));
	}

	CubeTexture::CubeTexture(CubeMap *owner, CubeMap::Orientation orientation) : m_Face(orientation),
	                                                                             m_Owner(owner) {}

	void CubeTexture::Image2D(const void *data, DataType type, Format format, const Vector2u &size, InternalFormat internalFormat)
	{
		m_Owner->Bind();
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(
		         glTexImage2D(GetTarget(m_Face), 0, static_cast<GLint>(internalFormat), size.Width, size.Height, 0, static_cast<GLenum>(format), static_cast<
			         GLenum>(type), data)
		        );
	}

	void CubeTexture::SubImage2D(const void *data, DataType type, Format format, const Vector2i &offset, const Vector2u &size)
	{
		if(!data)
			return;

		ASSERT(offset.X + size.Width < m_Size.Width && offset.Y + size.Height < m_Size.Height, "Texture size out of range");
		if(offset.X + size.Width > m_Size.Width || offset.Y + size.Height > m_Size.Height)
			throw std::runtime_error("Texture size out of range");

		m_Owner->Bind();
		CHECK_IF_VALID_CONTEXT;
		GL_CHECK(
		         glTexSubImage2D(GetTarget(m_Face), 0, offset.X, offset.Y, size.Width, size.Height, static_cast<GLenum>(format), static_cast<GLenum>(type), data
		         )
		        );
	}

	void CubeTexture::Create(const Vector2u &size)
	{
		Image2D(nullptr, DataType::UnsignedByte, Format::Rgba, size, InternalFormat::RGBA8);
	}

	Image CubeTexture::ToImage() const
	{
		const size_t size = static_cast<size_t>(m_Size.Width) * m_Size.Height;

		std::vector<Color> pixels;
		pixels.resize(size);
		
		CHECK_IF_VALID_CONTEXT Image(0, 0);
		GL_CHECK(glGetTextureImage(*m_Owner, static_cast<GLint>(m_Face), GL_RGBA, GL_UNSIGNED_BYTE, size, &pixels[0]));

		return Image(m_Size, pixels.data());
	}

	void CubeTexture::Update(const uint8_t *pixels)
	{
		Update(pixels, m_Size.Width, m_Size.Height, 0, 0);
	}

	void CubeTexture::Update(const Color *pixels)
	{
		Update(pixels, m_Size.Width, m_Size.Height, 0, 0);
	}

	void CubeTexture::Update(const Texture &texture)
	{
		Update(texture, 0, 0);
	}

	void CubeTexture::Update(const CubeTexture &texture)
	{
		Update(texture, 0, 0);
	}

	void CubeTexture::Update(const CubeTexture &texture, uint32_t x, uint32_t y)
	{
		Update(texture.ToImage(), x, y);
	}

	void CubeTexture::Update(const Image &image)
	{
		Update(image.GetPixels(), image.Width(), image.Height(), 0, 0);
	}

	void CubeTexture::Update(const Image &image, const uint32_t x, const uint32_t y)
	{
		Update(image.GetPixels(), image.Width(), image.Height(), x, y);
	}

	void CubeTexture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
	{
		if(pixels)
		{
			ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
			if(!(x + width >= m_Size.X && y + height >= m_Size.Y))
				throw std::out_of_range("Out of range");
			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, Vector2i(x, y), Vector2u{width, height});
		}
	}

	void CubeTexture::Update(const Color *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
	{
		if(pixels)
		{
			ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
			if(!(x + width >= m_Size.X && y + height >= m_Size.Y))
				throw std::out_of_range("Out of range");

			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, Vector2i(x, y), Vector2u{width, height});
		}
	}

	void CubeTexture::Update(const Texture &texture, uint32_t x, uint32_t y)
	{
		return Update(texture.ToImage(), x, y);
	}
}
