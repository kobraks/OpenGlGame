#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"

namespace Engine {
	class Image;

	enum class ImageFormat {
		
	};

	enum class Wrapping {
		Repeat = 0,
		ClampEdge,
		ClampBorder,
		MirroredRepeat,
	};

	enum class Filter {
		Nearest = 0,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	class Texture2D {
		friend class FrameBufferObject;
	public:
		using IDType = uint32_t;

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID;  }

		void GenerateMipMaps() const;
		bool IsMipMapsGenerated() const { return m_Internals->MipMapGenerated;  }

		void Bind() const;
		void BindUnit(uint32_t sampler = 0) const;

		static Ref<Texture2D> Create(const Vector2u& size, const uint8_t *pixels = nullptr);
		static Ref<Texture2D> Create(Ref<Image> image);

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter filter);
		void SetMagFilter(Filter filter);

		Filter GetMagFilter() const { return m_Internals->Filter.Mag; }
		Filter GetMinFilter() const { return m_Internals->Filter.Min; }

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		Ref<Image> ToImage() const;
		Ref<Image> GetImage(const Vector2u& size, const Vector2i& offset) const;

		void GetPixels(void* pixels, uint32_t size) const;

		void Update(const uint8_t* pixels);
		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Color* pixels);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture2D& texture);
		void Update(const Texture2D& texture, const Vector2i& offset);

		void Update(Ref<Image> image);
		void Update(Ref<Image> image, const Vector2i &offset);

		void Swap(Texture2D& to);

		static Vector2u GetMaxDim() { return { GetMaxSize(), GetMaxSize() }; }
		static uint32_t GetMaxSize();

		bool operator==(const Texture2D& texture) const {
			return m_Internals->ID == texture.m_Internals->ID;
		}
	protected:
		Texture2D();

		static bool CheckSize(const Vector2u& size);
		void CreateTexture(const Vector2u& size, const void* pixels = nullptr);
		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset);

	private:
		struct InternalWrapping {
			Wrapping S = Wrapping::Repeat;
			Wrapping T = Wrapping::Repeat;
		};

		struct InternalFilter {
			Filter Mag = Filter::Linear;
			Filter Min = Filter::NearestMipmapLinear;
		};

		struct Internals {
			IDType ID;
			Vector2u Size;

			bool MipMapGenerated = false;

			InternalWrapping Wrapping;
			InternalFilter Filter;

			Internals();
			~Internals();

			void Bind() const;
			void BindUnit(uint32_t sampler) const;

			void Storage(const Vector2u& size);

			void Image(const void* pixels, const Vector2u& size);
			void SubImage(const void* pixels, const Vector2u& size, const Vector2i& offset = {0, 0});

			void GetImage(void* pixels, uint32_t size) const;
			void GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset = { 0, 0 });

			void SetParameter(uint32_t name, int parameter);
			void GetParameter(uint32_t name, int* parameter);
		};

		Ref<Internals> m_Internals;
	};
}
