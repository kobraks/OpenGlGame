#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

#include <vector>

namespace Game
{
	class Image;
	class FrameBufferObject;

	struct TextureWrapping
	{
		Wrapping S = Wrapping::Repeat;
		Wrapping T = Wrapping::Repeat;
		Wrapping R = Wrapping::Repeat;
	};

	struct TextureFilter
	{
		Filter Min = Filter::Nearest;
		Filter Mag = Filter::Nearest;
	};

	class TextureObject
	{
	public:
		friend FrameBufferObject;

		using IDType = uint32_t;

	private:
		class Internals
		{
			bool m_Created = false;
		public:
			IDType Texture{};

			TextureTarget Target = TextureTarget::Texture2D;

			Vector2u Size;
			InternalFormat Format = InternalFormat::RGB8;

			float Lod    = 0.0;
			float MinLod = -1000;
			float MaxLod = 1000;

			uint32_t Levels = 1;

			TextureFilter Filter;
			TextureWrapping Wrapping;

			OpenGlFunctions Functions;

			bool MipMapGenerated = false;

			explicit Internals(TextureTarget target);
			~Internals();

			void Bind() const;

			bool Created() const { return m_Created; };
			void Create(const Vector2u &size, uint32_t levels = 1, InternalFormat format = InternalFormat::RGBA8);

			void Update(
				const void *pixels,
				DataType type,
				Game::Format format,
				int32_t level,
				const Vector2u &size,
				const Vector2i &offset
				);

			void GenerateMipMaps();

			[[nodiscard]] std::vector<Color> Get(uint32_t level = 0) const;

			void SetFilter(const TextureFilter &filter);

			void SetFilterMag(Game::Filter mag);
			void SetFilterMin(Game::Filter min);

			void SetWrapping(const TextureWrapping &wrapping);

			void SetWrapping(Game::Wrapping s, Game::Wrapping t);

			void SetWrappingS(Game::Wrapping s);
			void SetWrappingT(Game::Wrapping t);
			void SetWrappingR(Game::Wrapping r);

			void SetLod(float lod);
			void SetMaxLod(float lod);
			void SetMinLod(float lod);

		private:
			void Generate();
			void Destroy();
		};

		Ref<Internals> m_Internals;
	protected:
		explicit TextureObject(TextureTarget target);
	public:
		virtual ~TextureObject() = 0;

		operator IDType() const { return m_Internals->Texture; }
		[[nodiscard]] IDType ID() const { return m_Internals->Texture; }

		void Bind() const;

		uint32_t Levels() const { return m_Internals->Levels; }

		[[nodiscard]] Vector2u Size() const { return m_Internals->Size; }
		[[nodiscard]] uint32_t Width() const { return m_Internals->Size.Width; }
		[[nodiscard]] uint32_t Height() const { return m_Internals->Size.Height; }

		[[nodiscard]] InternalFormat GetInternalFormat() const { return m_Internals->Format; }

		[[nodiscard]] bool Created() const;

		void SetFilter(const TextureFilter &filter);

		void SetFilters(Filter min, Filter mag);

		void SetFilterMag(Filter mag);
		void SetFilterMin(Filter min);

		[[nodiscard]] std::vector<Color> Get(uint32_t level = 0) const;
		[[nodiscard]] Image ToImage(uint32_t level = 0) const;

		[[nodiscard]] Filter GetFilterMin() const { return m_Internals->Filter.Min; }
		[[nodiscard]] Filter GetFilterMag() const { return m_Internals->Filter.Mag; }

		[[nodiscard]] TextureFilter GetFilter() const { return m_Internals->Filter; }

		void SetWrapping(const TextureWrapping &wrapping);

		[[nodiscard]] TextureWrapping GetWrapping() const { return m_Internals->Wrapping; }

		void SetLod(float lod);
		void SetMaxLod(float lod);
		void SetMinLod(float lod);

		float GetLog() const { return m_Internals->Lod; }
		float GetMaxLod() const { return m_Internals->MaxLod; }
		float GetMinLod() const { return m_Internals->MinLod; }
	protected:
		void Create(const Vector2u &size, uint32_t levels = 1, InternalFormat format = InternalFormat::RGBA8);
		void Create(const Image &image, uint32_t levels = 1, InternalFormat format = InternalFormat::RGBA8);

		void GenerateMipMaps();

		bool HasGeneratedMipMaps() const { return m_Internals->MipMapGenerated; }

		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);
		void SetWrapping(Wrapping s, Wrapping t);

		void SetWrappingS(Wrapping s);
		void SetWrappingT(Wrapping t);
		void SetWrappingR(Wrapping r);

		[[nodiscard]] Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		[[nodiscard]] Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }
		[[nodiscard]] Wrapping GetWrappingR() const { return m_Internals->Wrapping.R; }

		virtual void SetParameter(TextureParameterName name, int parameter);

		void Update(const uint8_t *pixels, uint32_t level);
		void Update(const uint8_t *pixels, uint32_t level, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const uint8_t *pixels, uint32_t level, const Vector2u &size, const Vector2i &offset);

		void Update(const Color *pixels, uint32_t level);
		void Update(const Color *pixels, uint32_t level, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const Color *pixels, uint32_t level, const Vector2u &size, const Vector2i &offset);

		void Update(const TextureObject &texture, uint32_t level);
		void Update(const TextureObject &texture, uint32_t level, int32_t x, int32_t y);
		void Update(const TextureObject &texture, uint32_t level, const Vector2i &offset);

		void Update(const Image &image, uint32_t level);
		void Update(const Image &image, uint32_t level, int32_t x, int32_t y);
		void Update(const Image &image, uint32_t level, const Vector2i &offset);

	protected:
		static uint32_t CalculateLevels(const Vector2u &size);

	public:
		static uint64_t MaxBufferSize();
		static uint64_t MaxImageUnits();
	};
}
