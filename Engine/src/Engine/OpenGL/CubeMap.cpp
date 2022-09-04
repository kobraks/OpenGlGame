#include "pch.h"

#include "Engine/OpenGL/CubeMap.h"
#include "Engine/Core/Assert.h"
#include "Engine/OpenGL/Texture.h"

namespace Game
{
	static constexpr TextureTarget GetTarget(CubeMap::Orientation orientation)
	{
		switch(orientation)
		{
			case CubeMap::Orientation::Right:
				return TextureTarget::CubeMapPositiveX;
			case CubeMap::Orientation::Left:
				return TextureTarget::CubeMapNegativeX;
			case CubeMap::Orientation::Top:
				return TextureTarget::CubeMapPositiveY;
			case CubeMap::Orientation::Bottom:
				return TextureTarget::CubeMapNegativeY;
			case CubeMap::Orientation::Back:
				return TextureTarget::CubeMapPositiveZ;
			case CubeMap::Orientation::Front:
				return TextureTarget::CubeMapNegativeZ;
			default:
				return TextureTarget::CubeMapPositiveZ;
		}
	}

	CubeMap::Internals::Internals(): Format(InternalFormat::RGBA8)
	{
		Functions = Context::GetContext()->GetFunctions();

		ID = Functions.GenTexture();
	}

	CubeMap::Internals::~Internals()
	{
		for(auto &texture : Textures)
			delete texture;

		Functions.DeleteTexture(ID);
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

	CubeMap::CubeMap(uint32_t width, uint32_t height) : CubeMap(Vector2u(width, height)) {}

	CubeMap::CubeMap(const Vector2u &size) : CubeMap()
	{
		const uint32_t maxSize = Texture::GetMaxSize();

		ASSERT(size.Width == size.Height, "Requested cube size is not a squer");
		ASSERT(size.Width < maxSize && size.Height < maxSize, "Requested cubeMap size is to big");

		if(size.Width != size.Height)
			throw std::runtime_error("Requested cube size is not a squer");

		if(size.Width >= maxSize || size.Height >= maxSize)
			throw std::runtime_error("Requested cube map size is to big");

		m_Internals->Size = size;

		for(auto &texture : m_Internals->Textures)
			texture->Create(size);
	}

	void CubeMap::SetWrapping(Wrapping s)
	{
		SetWrappingS(s);
	}

	void CubeMap::SetWrapping(Wrapping s, Wrapping t)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;

		SetParameter(TextureParamName::WrapS, static_cast<int32_t>(s));
		SetParameter(TextureParamName::WrapT, static_cast<int32_t>(t));
	}

	void CubeMap::SetWrapping(Wrapping s, Wrapping t, Wrapping r)
	{
		m_Internals->Wrapping.S = s;
		m_Internals->Wrapping.T = t;
		m_Internals->Wrapping.R = r;

		SetParameter(TextureParamName::WrapS, static_cast<int32_t>(s));
		SetParameter(TextureParamName::WrapT, static_cast<int32_t>(t));
		SetParameter(TextureParamName::WrapR, static_cast<int32_t>(r));
	}

	void CubeMap::SetWrappingS(Wrapping wrap)
	{
		m_Internals->Wrapping.S = wrap;

		SetParameter(TextureParamName::WrapS, static_cast<int32_t>(wrap));
	}

	void CubeMap::SetWrappingT(Wrapping wrap)
	{
		m_Internals->Wrapping.T = wrap;

		SetParameter(TextureParamName::WrapT, static_cast<int32_t>(wrap));
	}

	void CubeMap::SetWrappingR(Wrapping wrap)
	{
		m_Internals->Wrapping.R = wrap;

		SetParameter(TextureParamName::WrapR, static_cast<int32_t>(wrap));
	}

	void CubeMap::SetMagFiler(Filter mag)
	{
		m_Internals->Filter.Mag = mag;

		SetParameter(TextureParamName::MagFilter, static_cast<int32_t>(mag));
	}

	void CubeMap::SetMinFiler(Filter min)
	{
		m_Internals->Filter.Min = min;

		SetParameter(TextureParamName::MinFilter, static_cast<int32_t>(min));
	}

	void CubeMap::Bind() const
	{
		m_Internals->Functions.BindTexture(TextureTarget::CubeMap, m_Internals->ID);
	}

	CubeTexture& CubeMap::GetTexture(Orientation orientation)
	{
		return *m_Internals->Textures[static_cast<uint32_t>(orientation)];
	}

	const CubeTexture& CubeMap::GetTexture(Orientation orientation) const
	{
		return *m_Internals->Textures[static_cast<uint32_t>(orientation)];
	}

	void CubeMap::Swap(CubeMap &cubeMap)
	{
		std::swap(m_Internals, cubeMap.m_Internals);

		for(size_t i = 0; i < m_Internals->Textures.size(); ++i)
		{
			m_Internals->Textures[i]->m_Owner         = this;
			cubeMap.m_Internals->Textures[i]->m_Owner = &cubeMap;
		}
	}

	void CubeMap::SetParameter(TextureParamName name, int32_t param)
	{
		m_Internals->Functions.TextureParameter(m_Internals->ID, name, param);
	}

	CubeTexture::CubeTexture(CubeMap *owner, CubeMap::Orientation orientation) : m_Face(orientation),
		m_Owner(owner) { }

	void CubeTexture::Image2D(
		const void *pixels,
		DataType type,
		Format format,
		const Vector2u &size,
		InternalFormat internalFormat
		)
	{
		m_Size = size;

		m_Owner->m_Internals->Functions.Image2D(
		                                        m_Owner->ID(),
		                                        TextureTarget::CubeMap,
		                                        GetTarget(m_Face),
		                                        0,
		                                        internalFormat,
		                                        size,
		                                        0,
		                                        format,
		                                        type,
		                                        pixels
		                                       );
	}

	void CubeTexture::SubImage2D(
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
		       offset.X + size.Width < m_Size.Width && offset.Y + size.Height < m_Size.Height,
		       "Texture size out of range"
		      );

		if(offset.X + size.Width > m_Size.Width || offset.Y + size.Height > m_Size.Height)
			throw std::runtime_error("Texture size out of range");

		m_Owner->m_Internals->Functions.SubImage2D(
		                                           m_Owner->ID(),
		                                           static_cast<int32_t>(m_Face),
		                                           offset.X,
		                                           offset.Y,
		                                           size.Width,
		                                           size.Height,
		                                           format,
		                                           type,
		                                           pixels
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

		m_Owner->m_Internals->Functions.GetTextureImage(*m_Owner, static_cast<int32_t>(m_Face), Format::Rgba, DataType::UnsignedByte, static_cast<uint32_t>(size), &pixels[0]);

		return Image(m_Size, pixels.data());
	}

	void CubeTexture::Update(const uint8_t *pixels)
	{
		Update(pixels, m_Size.Width, m_Size.Height, 0, 0);
	}

	void CubeTexture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		if (pixels)
		{
			ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
			if (x + width >= m_Size.X && y + height >= m_Size.Y)
				throw std::out_of_range("Out of range");

			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, Vector2i(x, y), Vector2u{width, height});
		}
	}

	void CubeTexture::Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset)
	{
		Update(pixels, size.Width, size.Height, offset.X, offset.Y);
	}

	void CubeTexture::Update(const Color *pixels)
	{
		Update(pixels, m_Size.Width, m_Size.Height, 0, 0);
	}

	void CubeTexture::Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		if (pixels)
		{
			ASSERT(x + width < m_Size.X && y + height < m_Size.Y, "Out of range");
			if (x + width >= m_Size.X && y + height >= m_Size.Y)
				throw std::out_of_range("Out of range");

			SubImage2D(pixels, DataType::UnsignedByte, Format::Rgba, Vector2i(x, y), Vector2u{width, height});
		}
	}

	void CubeTexture::Update(const Color *pixels, const Vector2u &size, const Vector2i &offset)
	{
		Update(pixels, size.Width, size.Height, offset.X, offset.Y);
	}

	void CubeTexture::Update(const Texture &texture)
	{
		Update(texture.ToImage(), 0, 0);
	}

	void CubeTexture::Update(const Texture &texture, int32_t x, int32_t y)
	{
		Update(texture.ToImage(), x, y);
	}

	void CubeTexture::Update(const Texture &texture, const Vector2i &offset)
	{
		Update(texture.ToImage(), offset.X, offset.Y);
	}

	void CubeTexture::Update(const CubeTexture &texture)
	{
		Update(texture.ToImage(), 0, 0);
	}

	void CubeTexture::Update(const CubeTexture &texture, int32_t x, int32_t y)
	{
		Update(texture.ToImage(), x, y);
	}

	void CubeTexture::Update(const CubeTexture &texture, const Vector2i &offset)
	{
		Update(texture.ToImage(), offset.X, offset.Y);
	}

	void CubeTexture::Update(const Image &image)
	{
		Update(image.GetPixels(), image.Width(), image.Height(), 0, 0);
	}
	void CubeTexture::Update(const Image &image, int32_t x, int32_t y)
	{
		Update(image.GetPixels(), image.Width(), image.Height(), x, y);
	}
	void CubeTexture::Update(const Image &image, const Vector2i &offset)
	{
		Update(image.GetPixels(), image.Width(), image.Height(), offset.X, offset.Y);
		
	}
}
