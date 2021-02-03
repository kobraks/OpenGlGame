#pragma once
#include <array>

#include "Attachment.h"
#include "Image.h"
#include "Types.h"
#include "GLEnums.h"
#include "Color.h"

namespace Game
{
	class Texture;
	class CubeTexture;

	class CubeMap: public Attachment
	{
	public:
		using IdType = uint32_t;

		enum class Orientation
		{
			Right = 0,
			Left,
			Top,
			Bottom,
			Back,
			Front
		};

	private:
		class Internals
		{
		public:
			struct Wrapping
			{
				Game::Wrapping S = Game::Wrapping::Repeat;
				Game::Wrapping T = Game::Wrapping::Repeat;
				Game::Wrapping R = Game::Wrapping::Repeat;
			};

			struct Filter
			{
				Game::Filter Mag = Game::Filter::Linear;
				Game::Filter Min = Game::Filter::NearestMipmapLinear;
			};

			IdType Id;
			Vector2u Size;
			InternalFormat Format;

			Wrapping Wrapping;
			Filter Filter;

			bool MipmapGenerated;

			std::array<CubeTexture*, 6> Textures = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

			Internals();
			~Internals();
		};

		Pointer<Internals> m_Internals;
		
		CubeMap();
	public:
		CubeMap(uint32_t width, uint32_t height);
		explicit CubeMap(const Vector2u& size);

		operator IdType() const { return m_Internals->Id; }
		IdType Id() const { return m_Internals->Id; }

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);
		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }
		Wrapping GetWrappingR() const { return m_Internals->Wrapping.R; }

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);
		void SetWrappingR(Wrapping wrapping);

		Filter GetMagFilter() const { return m_Internals->Filter.Mag; }
		Filter GetMinFilter() const { return m_Internals->Filter.Min; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter min);
		void SetMagFilter(Filter mag);

		void Bind() const;

		CubeTexture& GetTexture(Orientation orientation);
		const CubeTexture& GetTexture(Orientation orientation) const;

		void Swap(CubeMap &cubeMap);
	protected:
		void SetParameter(uint32_t name, int parameter);
	};

	class CubeTexture
	{
		friend class CubeMap;
		const CubeMap::Orientation m_Face;
		CubeMap *m_Owner;
		InternalFormat m_Format = InternalFormat::RGBA8;

		Vector2u m_Size;
	protected:
		CubeTexture(CubeMap *owner, CubeMap::Orientation orientation);

		void Image2D(const void *data, DataType type, Format format, const Vector2u &size, InternalFormat internalFormat);
		void SubImage2D(const void *data, DataType type, Format format, const Vector2i &offset, const Vector2u &size);

		void Create(const Vector2u& size);
	public:
		CubeMap& GetOwner() const { return *m_Owner; };

		Vector2u Size() const { return m_Size; }
		uint32_t Width() const { return m_Size.Width; }
		uint32_t Height() const { return m_Size.Height; }

		InternalFormat GetInternalFormat() const { return m_Format; }

		Image ToImage() const;

		void Update(const uint8_t *pixels);
		void Update(const uint8_t *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y);

		void Update(const Color *pixels);
		void Update(const Color *pixels, uint32_t width, uint32_t height, uint32_t x, uint32_t y);

		void Update(const Texture &texture);
		void Update(const Texture &texture, uint32_t x, uint32_t y);

		void Update(const CubeTexture &texture);
		void Update(const CubeTexture &texture, uint32_t x, uint32_t y);

		void Update(const Image &image);

		void Update(const Image &image, const uint32_t x, const uint32_t y);
	};
}
