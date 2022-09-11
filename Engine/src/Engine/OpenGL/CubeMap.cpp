#include "pch.h"

#include "Engine/OpenGL/CubeMap.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/Image.h"
#include "Engine/OpenGL/Texture.h"
#include "Engine/Renderer/Context.h"

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

	CubeMap::CubeMap() : TextureObject(TextureTarget::CubeMap)
	{
		for(size_t i      = 0; i < static_cast<size_t>(Orientation::Count); ++i)
			m_Textures[i] = new CubeTexture(static_cast<Orientation>(i));
	}

	CubeMap::CubeMap(uint32_t width, uint32_t height) : CubeMap()
	{
		Create({width, height});
	}

	CubeMap::CubeMap(const Vector2u &size) : CubeMap()
	{
		Create(size);
	}

	CubeMap::CubeMap(const CubeMap &map) : TextureObject(map)
	{
		for(size_t i = 0; i < static_cast<size_t>(Orientation::Count); ++i)
		{
			delete m_Textures[i];
			m_Textures[i] = new CubeTexture(*map.m_Textures[i]);
		}
	}

	CubeMap::CubeMap(CubeMap &&map) noexcept : TextureObject(map)
	{
		std::swap(map.m_Textures, m_Textures);
	}

	CubeMap::~CubeMap()
	{
		for(auto &tex : m_Textures)
			delete tex;
	}

	CubeMap& CubeMap::operator=(const CubeMap &map)
	{
		TextureObject::operator=(map);

		for(size_t i = 0; i < static_cast<size_t>(Orientation::Count); ++i)
		{
			delete m_Textures[i];
			m_Textures[i] = new CubeTexture(*map.m_Textures[i]);
		}

		return *this;
	}

	CubeMap& CubeMap::operator=(CubeMap &&map) noexcept
	{
		TextureObject::operator=(map);

		for(size_t i = 0; i < static_cast<size_t>(Orientation::Count); ++i)
		{
			std::swap(map.m_Textures[i], m_Textures[i]);
		}

		return *this;
	}

	CubeTexture& CubeMap::GetTexture(Orientation orientation)
	{
		return m_Textures[static_cast<size_t>(orientation)]->SetOwner(*this);
	}

	const CubeTexture& CubeMap::GetTexture(Orientation orientation) const
	{
		return m_Textures[static_cast<size_t>(orientation)]->SetOwner(*this);
	}

	uint64_t CubeMap::GetMaxSize()
	{
		static bool s_Checked  = false;
		static uint64_t s_Size = 0;

		if(s_Checked)
			return s_Size;

		ASSERT(Context::GetContext(), "No active openGL context");

		if(!Context::GetContext())
			throw std::runtime_error("No active openGL context");

		s_Checked = true;
		return s_Size = static_cast<uint64_t>(Context::GetContext()->GetFunctions().GetInteger(
			 GL_MAX_CUBE_MAP_TEXTURE_SIZE
			));
	}

	void CubeMap::Create(const Vector2u &size)
	{
		ASSERT(size.Width == size.Height);
		ASSERT(size.Width < GetMaxSize() && size.Height < GetMaxSize());

		if(size.Width != size.Height)
			throw std::runtime_error("Unable to create texture that is not squar");

		if(size.Width < GetMaxSize() || size.Height < GetMaxSize())
			throw std::runtime_error("Cube map excede max supported size");

		for(auto &tex : m_Textures)
			tex->SetSize(size);

		TextureObject::Create(size, 6, InternalFormat::RGBA8);
	}

	CubeTexture::CubeTexture(CubeMap::Orientation orientation) : m_Face(orientation),
	                                                             m_Owner(nullptr) {}

	void CubeTexture::SetSize(const Vector2u &size)
	{
		m_Size = size;
	}

	CubeTexture& CubeTexture::SetOwner(const CubeMap &owner) const
	{
		m_Owner = &owner;

		return const_cast<CubeTexture&>(*this);
	}

	Image CubeTexture::ToImage() const
	{
		return m_Owner->ToImage(static_cast<uint32_t>(m_Face));
	}

	void CubeTexture::Update(const uint8_t *pixels)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face));
	}

	void CubeTexture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face), width, height, x, y);
	}

	void CubeTexture::Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face), size, offset);
	}

	void CubeTexture::Update(const Color *pixels)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face));
	}

	void CubeTexture::Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face), width, height, x, y);
	}

	void CubeTexture::Update(const Color *pixels, const Vector2u &size, const Vector2i &offset)
	{
		const_cast<CubeMap*>(m_Owner)->Update(pixels, static_cast<uint32_t>(m_Face), size, offset);
	}

	void CubeTexture::Update(const TextureObject &texture)
	{
		const_cast<CubeMap*>(m_Owner)->Update(texture, static_cast<uint32_t>(m_Face));
	}

	void CubeTexture::Update(const TextureObject &texture, int32_t x, int32_t y)
	{
		const_cast<CubeMap*>(m_Owner)->Update(texture, static_cast<uint32_t>(m_Face), x, y);
	}

	void CubeTexture::Update(const TextureObject &texture, const Vector2i &offset)
	{
		const_cast<CubeMap*>(m_Owner)->Update(texture, static_cast<uint32_t>(m_Face), offset);
	}

	void CubeTexture::Update(const Image &image)
	{
		const_cast<CubeMap*>(m_Owner)->Update(image, static_cast<uint32_t>(m_Face));
	}

	void CubeTexture::Update(const Image &image, int32_t x, int32_t y)
	{
		const_cast<CubeMap*>(m_Owner)->Update(image, static_cast<uint32_t>(m_Face), x, y);
	}

	void CubeTexture::Update(const Image &image, const Vector2i &offset)
	{
		const_cast<CubeMap*>(m_Owner)->Update(image, static_cast<uint32_t>(m_Face), offset);
	}
}
