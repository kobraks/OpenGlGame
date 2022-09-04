#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Image.h"

#include "Engine/OpenGL/GLEnums.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

#include <glad/glad.h>

namespace Game
{
	class FrameBufferObject;
	class CubeTexture;

	class Texture
	{
		friend FrameBufferObject;

	public:
		using IDType = uint32_t;

	private:
		struct InternalWrapping
		{
			Game::Wrapping S = Game::Wrapping::Repeat;
			Game::Wrapping T = Game::Wrapping::Repeat;
			Game::Wrapping R = Game::Wrapping::Repeat;
		};

		struct InternalFilter
		{
			Game::Filter Mag = Game::Filter::Linear;
			Game::Filter Min = Game::Filter::NearestMipmapLinear;
		};

		class Internals
		{
		public:
			IDType ID;

			Vector2u Size {0, 0};

			InternalFormat Format = InternalFormat::RGBA8;

			InternalWrapping Wrapping;
			InternalFilter Filter;

			bool MipmapGenerated = false;

			OpenGlFunctions Functions;

			Internals();
			~Internals();
		};

		Pointer<Internals> m_Internals;

	public:
		Texture();
		Texture(uint32_t width, uint32_t height);
		explicit Texture(const Vector2u &size);
		explicit Texture(const Image &image);

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);
		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }
		Wrapping GetWrappingR() const { return m_Internals->Wrapping.R; }

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);
		void SetWrappingR(Wrapping wrapping);

		Filter GetMagFilter() const {return m_Internals->Filter.Mag; }
		Filter GetMinFilter() const {return m_Internals->Filter.Min; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter min);
		void SetMagFilter(Filter mag);

		void Bind() const;

		void GenerateMipMaps() const;
		bool IsMipMapsGenerated() const { return m_Internals->MipmapGenerated; }

		void Create(uint32_t width, uint32_t height);
		void Create(const Vector2u &size);
		void Create(const Image &image);

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		InternalFormat GetInternalFormat() const { return m_Internals->Format; }

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

		void Update(const CubeTexture &texture);
		void Update(const CubeTexture &texture, int32_t x, int32_t y);
		void Update(const CubeTexture &texture, const Vector2i &offset);

		void Update(const Image &image) { Update(image.GetPixels(), image.Size(), {0, 0}); }
		void Update(const Image &image, int32_t x, int32_t y) { Update(image.GetPixels(), image.Size(), {x, y}); }
		void Update(const Image &image, const Vector2i offset) { Update(image.GetPixels(), image.Size(), offset); }

		void Resize(uint32_t width, uint32_t height);
		void Resize(const Vector2u &newSize);

		void Swap(Texture &right) noexcept;

		static uint32_t GetMaximumSize() { return GetMaxSize(); }
		static uint32_t GetMaxSize();

		bool operator==(const Texture& texture) const
		{
			return m_Internals->ID == texture.m_Internals->ID;
		}

	private:
		void Image2D(const void *pixels, DataType type, Format format, const Vector2u &size, InternalFormat internalFormat = InternalFormat::RGBA8);
		void SubImage2D(const void *pixels, DataType type, Format format, const Vector2i &offset, const Vector2u &size);

		void Image2D(const void *pixels, DataType type, Format format, uint32_t width, uint32_t height, InternalFormat internalFormat = InternalFormat::RGBA8);
		void SubImage2D(const void *pixels, DataType type, Format format, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		void SetParameter(TextureParamName name, int parameter);
	};
}