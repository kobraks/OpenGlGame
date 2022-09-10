#include "pch.h"
#include "Engine/OpenGL/TextureObject.h"

#include "Engine/Core/Image.h"

#include "Engine/Renderer/Context.h"

namespace Game
{
	TextureObject::Internals::Internals(TextureTarget target) : Functions(Context::GetContext()->GetFunctions())
	{
		Target = target;
		Generate();
	}

	TextureObject::Internals::~Internals()
	{
		Destroy();
		MipMapGenerated = false;
	}

	void TextureObject::Internals::Bind() const
	{
		Functions.BindTexture(Target, Texture);
	}

	void TextureObject::Internals::Create(const Vector2u &size, uint32_t levels, InternalFormat format)
	{
		if (m_Created)
		{
			Destroy();
			Generate();
		}

		Size   = size;
		Levels = levels;
		Format = format;

		m_Created = true;

		Functions.TexStorage2D(Texture, levels, format, size);
	}

	void TextureObject::Internals::Update(
		const void *pixels,
		DataType type,
		Game::Format format,
		int32_t level,
		const Vector2u &size,
		const Vector2i &offset
		)
	{
		ASSERT(size.Width + offset.X < Size.Width && size.Height + offset.Y < Size.Height);

		if(size.Width + offset.X >= Size.Width || size.Height + offset.Y >= Size.Height)
			throw std::out_of_range("Out of range");

		if(pixels)
			Functions.SubImage2D(Texture, level, offset, size, format, type, pixels);
	}

	void TextureObject::Internals::GenerateMipMaps()
	{
		MipMapGenerated = true;
		Functions.GenerateMipMap(Texture);
	}

	std::vector<Color> TextureObject::Internals::Get(uint32_t level) const
	{
		std::vector<Color> pixels;

		pixels.resize(static_cast<std::vector<Color>::size_type>(Size.Width) * static_cast<std::vector<Color>::size_type>(Size.Height));
		Functions.GetTextureImage(
		                          Texture,
		                          static_cast<int32_t>(level),
		                          Format::Rgba,
		                          DataType::UnsignedByte,
		                          static_cast<uint32_t>(pixels.size()),
		                          pixels.data()
		                         );

		return pixels;
	}

	void TextureObject::Internals::SetFilter(const TextureFilter &filter)
	{
		Filter = filter;

		Functions.TextureParameter(Texture, TextureParamName::MagFilter, static_cast<int32_t>(filter.Mag));
		Functions.TextureParameter(Texture, TextureParamName::MinFilter, static_cast<int32_t>(filter.Min));
	}

	void TextureObject::Internals::SetFilterMag(Game::Filter mag)
	{
		Filter.Mag = mag;
		Functions.TextureParameter(Texture, TextureParamName::MagFilter, static_cast<int32_t>(mag));
	}

	void TextureObject::Internals::SetFilterMin(Game::Filter min)
	{
		Filter.Min = min;
		Functions.TextureParameter(Texture, TextureParamName::MinFilter, static_cast<int32_t>(min));
	}

	void TextureObject::Internals::SetWrapping(const TextureWrapping &wrapping)
	{
		Wrapping = wrapping;

		Functions.TextureParameter(Texture, TextureParamName::WrapS, static_cast<int32_t>(wrapping.S));
		Functions.TextureParameter(Texture, TextureParamName::WrapT, static_cast<int32_t>(wrapping.T));
		Functions.TextureParameter(Texture, TextureParamName::WrapR, static_cast<int32_t>(wrapping.R));
	}

	void TextureObject::Internals::SetWrapping(Game::Wrapping s, Game::Wrapping t)
	{
		Wrapping.S = s;
		Wrapping.T = t;

		Functions.TextureParameter(Texture, TextureParamName::WrapS, static_cast<int32_t>(s));
		Functions.TextureParameter(Texture, TextureParamName::WrapT, static_cast<int32_t>(t));
	}

	void TextureObject::Internals::SetWrappingS(Game::Wrapping s)
	{
		Wrapping.S = s;

		Functions.TextureParameter(Texture, TextureParamName::WrapS, static_cast<int32_t>(s));
	}

	void TextureObject::Internals::SetWrappingT(Game::Wrapping t)
	{
		Wrapping.T = t;

		Functions.TextureParameter(Texture, TextureParamName::WrapT, static_cast<int32_t>(t));
	}

	void TextureObject::Internals::SetWrappingR(Game::Wrapping r)
	{
		Wrapping.R = r;

		Functions.TextureParameter(Texture, TextureParamName::WrapR, static_cast<int32_t>(r));
	}

	void TextureObject::Internals::Generate()
	{
		Texture = Functions.GenTexture();
	}

	void TextureObject::Internals::Destroy()
	{
		Functions.DeleteTexture(Texture);
	}

	TextureObject::TextureObject(TextureTarget target) : m_Internals(MakePointer<Internals>(target))
	{
	}

	TextureObject::~TextureObject() {}

	void TextureObject::Bind() const
	{
		m_Internals->Bind();
	}

	bool TextureObject::Created() const
	{
		return m_Internals->Created();
	}

	void TextureObject::Create(const Vector2u &size, uint32_t levels, InternalFormat format)
	{
		m_Internals->Create(size, levels, format);
	}

	void TextureObject::Create(const Image &image, uint32_t levels, InternalFormat format)
	{
		Create(image.Size(), levels, format);
		Update(image, 0);
	}

	std::vector<Color> TextureObject::Get(uint32_t level) const
	{
		return m_Internals->Get(level);
	}

	Image TextureObject::ToImage(uint32_t level) const
	{
		return Image(Size(), Get(level).data());
	}

	void TextureObject::GenerateMipMaps()
	{
		m_Internals->GenerateMipMaps();
	}

	void TextureObject::SetFilter(const TextureFilter &filter)
	{
		m_Internals->SetFilter(filter);
	}

	void TextureObject::SetFilterMag(Filter mag)
	{
		m_Internals->SetFilterMag(mag);
	}

	void TextureObject::SetFilterMin(Filter min)
	{
		m_Internals->SetFilterMin(min);
	}

	void TextureObject::SetFilters(Filter min, Filter mag)
	{
		SetFilter({min, mag});
	}

	void TextureObject::SetWrapping(Wrapping s, Wrapping t, Wrapping r)
	{
		SetWrapping({s, t, r});
	}

	void TextureObject::SetWrapping(Wrapping s, Wrapping t)
	{
		m_Internals->SetWrapping(s, t);
	}

	void TextureObject::SetWrapping(const TextureWrapping &wrapping)
	{
		m_Internals->SetWrapping(wrapping);
	}

	void TextureObject::SetWrappingS(Wrapping s)
	{
		m_Internals->SetWrappingS(s);
	}
	void TextureObject::SetWrappingT(Wrapping t)
	{
		m_Internals->SetWrappingT(t);
	}

	void TextureObject::SetWrappingR(Wrapping r)
	{
		m_Internals->SetWrappingR(r);
	}

	void TextureObject::SetParameter(TextureParamName name, int parameter)
	{
		m_Internals->Functions.TextureParameter(*this, name, parameter);
	}

	void TextureObject::Update(const uint8_t *pixels, uint32_t level)
	{
		Update(pixels, level, Size(), {0, 0});
	}

	void TextureObject::Update(
		const uint8_t *pixels,
		uint32_t level,
		uint32_t width,
		uint32_t height,
		int32_t x,
		int32_t y
		)
	{
		Update(pixels, level, {width, height}, {x, y});
	}

	void TextureObject::Update(const uint8_t *pixels, uint32_t level, const Vector2u &size, const Vector2i &offset)
	{
		m_Internals->Update(pixels, DataType::UnsignedByte, Format::Rgba, static_cast<int32_t>(level), size, offset);
	}

	void TextureObject::Update(const Color *pixels, uint32_t level)
	{
		Update(pixels, level, Size(), {0, 0});
	}

	void TextureObject::Update(
		const Color *pixels,
		uint32_t level,
		uint32_t width,
		uint32_t height,
		int32_t x,
		int32_t y
		)
	{
		Update(pixels, level, {width, height}, {x, y});
	}

	void TextureObject::Update(const Color *pixels, uint32_t level, const Vector2u &size, const Vector2i &offset)
	{
		m_Internals->Update(pixels, DataType::UnsignedByte, Format::Rgba, static_cast<int32_t>(level), size, offset);
	}

	void TextureObject::Update(const TextureObject &texture, uint32_t level)
	{
		Update(texture.ToImage(), level, {0, 0});
	}
	void TextureObject::Update(const TextureObject &texture, uint32_t level, int32_t x, int32_t y)
	{
		Update(texture.ToImage(), level, {x, y});
	}

	void TextureObject::Update(const TextureObject &texture, uint32_t level, const Vector2i &offset)
	{
		Update(texture.ToImage(), level, offset);
	}

	void TextureObject::Update(const Image &image, uint32_t level)
	{
		Update(image.GetPixels(), level, image.Size(), {0, 0});
	}

	void TextureObject::Update(const Image &image, uint32_t level, int32_t x, int32_t y)
	{
		Update(image.GetPixels(), level, image.Size(), {x, y});
	}

	void TextureObject::Update(const Image &image, uint32_t level, const Vector2i &offset)
	{
		Update(image.GetPixels(), level, image.Size(), offset);
	}

	uint32_t TextureObject::CalculateLevels(const Vector2u &size)
	{
		return static_cast<uint32_t>(std::floor(
		                                        std::log2(static_cast<long double>(std::max(size.Width, size.Height)))
		                                       ));
	}
}
