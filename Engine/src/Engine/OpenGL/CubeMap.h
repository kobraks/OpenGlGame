#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Image.h"
#include "Engine/Core/Color.h"
#include "Engine/OpenGL/GLEnums.h"
#include "Engine/Renderer/Context.h"


namespace Game
{
	class Texture;
	class CubeTexture;

	class CubeMap
	{
		friend CubeTexture;

	public:
		using IDType = uint32_t;

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

			IDType ID;
			Vector2u Size;
			InternalFormat Format;

			Wrapping Wrapping;
			Filter Filter;

			OpenGlFunctions Functions;

			bool MipMapGenerated = false;

			std::array<CubeTexture*, 8> Textures = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

			Internals();
			~Internals();
		};

		Pointer<Internals> m_Internals;
		CubeMap();

	public:
		CubeMap(uint32_t width, uint32_t height);
		CubeMap(const Vector2u& size);

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);
		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);

		void SetWrappingS(Wrapping wrap);
		void SetWrappingT(Wrapping wrap);
		void SetWrappingR(Wrapping wrap);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }
		Wrapping GetWrappingR() const { return m_Internals->Wrapping.R; }

		Filter GetMagFiler() const { return m_Internals->Filter.Mag; }
		Filter GetMinFiler() const { return m_Internals->Filter.Min; }

		void SetMagFiler(Filter mag);
		void SetMinFiler(Filter min);

		void Bind() const;

		CubeTexture& GetTexture(Orientation orientation);
		const CubeTexture& GetTexture(Orientation orientation) const;

		void Swap(CubeMap& cubeMap);

	protected:
		void SetParameter(TextureParamName name, int32_t param);
	};

	class CubeTexture
	{
		friend CubeMap;

		const CubeMap::Orientation m_Face;
		CubeMap *m_Owner;

		InternalFormat m_Format = InternalFormat::RGBA8;

		Vector2u m_Size;

	protected:
		CubeTexture(CubeMap *owner, CubeMap::Orientation orientation);

		void Image2D(const void* pixels, DataType type, Format format, const Vector2u &size, InternalFormat internalFormat);
		void SubImage2D(const void* pixels, DataType type, Format format, const Vector2i &offset, const Vector2u &size);

		void Create(const Vector2u& size);
	public:
		CubeMap& GetOwner() const { return *m_Owner; }

		Vector2u Size() const { return m_Size; }
		uint32_t Width() const { return m_Size.Width; }
		uint32_t Height() const { return m_Size.Height; }

		InternalFormat GetInternalFormat() const { return m_Format; }

		Image ToImage() const;

		void Update(const uint8_t *pixels);
		void Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const Color *pixels);
		void Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const Color *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const Texture& texture);
		void Update(const Texture& texture, int32_t x, int32_t y);
		void Update(const Texture& texture, const Vector2i &offset);

		void Update(const CubeTexture& texture);
		void Update(const CubeTexture& texture, int32_t x, int32_t y);
		void Update(const CubeTexture& texture, const Vector2i &offset);

		void Update(const Image& image);
		void Update(const Image& image, int32_t x, int32_t y);
		void Update(const Image& image, const Vector2i &offset);
	};
}