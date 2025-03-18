#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Image.h"
#include "Engine/Core/Color.h"

namespace Engine {
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

		Texture2D();
		Texture2D(uint32_t width, uint32_t height);
		explicit Texture2D(const Vector2u& size);
		explicit Texture2D(const Image& image);

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID;  }

		void GenerateMipMaps() const;
		bool IsMipMapsGenerated() const { return m_Internals->MipMapGenerated;  }

		void Bind() const;

		void Create(const Vector2u& size);
		void Create(const Image& image);

		void SetWrapping(Wrapping s);
		void SetWrapping(Wrapping s, Wrapping t);
		void SetWrapping(Wrapping s, Wrapping t, Wrapping r);

		void SetWrappingS(Wrapping wrapping);
		void SetWrappingT(Wrapping wrapping);
		void SetWrappingR(Wrapping wrapping);

		Wrapping GetWrappingS() const { return m_Internals->Wrapping.S; }
		Wrapping GetWrappingT() const { return m_Internals->Wrapping.T; }
		Wrapping GetWrappingR() const { return m_Internals->Wrapping.R; }

		void SetFilters(Filter min, Filter mag);
		void SetMinFilter(Filter filter);
		void SetMagFilter(Filter filter);

		Filter GetMagFilter() const { return m_Internals->Filter.Mag; }
		Filter GetMinFilter() const { return m_Internals->Filter.Min; }

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		Image ToImage() const;

		void Update(const uint8_t* pixels);
		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Color* pixels);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture2D& texture);
		void Update(const Texture2D& texture, const Vector2i& offset);

		void Update(const Image& image);
		void Update(const Image& image, const Vector2i &offset);

		void Resize(const Vector2u& size);
		void Swap(Texture2D& to);

		static Vector2u GetMaxSize();
		static uint32_t GetMaxDim();

		bool operator==(const Texture2D& texture) const {
			return m_Internals->ID == texture.m_Internals->ID;
		}
	protected:
		bool CheckTextureSize(const Vector2u& size);
		void CreateTexture(const Vector2u& size, const void* pixels = nullptr);
		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset);

	private:
		struct InternalWrapping {
			Wrapping S = Wrapping::Repeat;
			Wrapping T = Wrapping::Repeat;
			Wrapping R = Wrapping::Repeat;
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

			void Bind();

			void Image2D(const void* pixels, const Vector2u& size);
			void SubImage2D(const void* pixels, const Vector2u& size, const Vector2i& offset);
			void GetImage(void* pixels, uint32_t size);

			void SetParameter(uint32_t name, int parameter);
		};

		Ref<Internals> m_Internals;
	};
}
