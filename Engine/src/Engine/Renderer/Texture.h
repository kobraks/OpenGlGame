#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine {
	class Image;

	class Texture {
		friend class Framebuffer;

	public:
		using IDType = uint32_t;

		~Texture() = default;

		operator IDType() const { return m_Internals->ID; }
		IDType ID() const { return m_Internals->ID; }

		void GenerateMipMaps() const;

		bool HasMipMapsGenerated() const { return m_Internals->MipMapGenerated; }
		bool IsMultisampled() const { return m_Internals->Multisampled; }

		void Bind() const;
		void BindUnit(uint32_t sampler = 0) const;

		void Unbind() const;
		void UnbindUnit(uint32_t sampler = 0) const;

		static Ref<Texture> Create(const Vector2u& size, ImageFormat imageFormat, uint32_t samples = 1, const std::string& label = {}, const uint8_t* pixels = nullptr, std::optional<DataType> dataType = std::nullopt, std::optional<DataFormat> dataFormat = std::nullopt);
		static Ref<Texture> Create(Ref<Image> image, ImageFormat imageFormat = ImageFormat::RGBA8, uint32_t samples = 1, const std::string& label = {});

		void SetLabel(const std::string& label);

		void SetWrapping(WrapMode s);
		void SetWrapping(WrapMode s, WrapMode t);

		void SetWrappingS(WrapMode wrapping);
		void SetWrappingT(WrapMode wrapping);

		WrapMode GetWrappingS() const { return m_Internals->Wrapping.S; }
		WrapMode GetWrappingT() const { return m_Internals->Wrapping.T; }

		void SetFilters(FilterMode min, FilterMode mag);
		void SetMinFilter(FilterMode filter);
		void SetMagFilter(FilterMode filter);

		FilterMode GetMagFilter() const { return m_Internals->Filter.Mag; }
		FilterMode GetMinFilter() const { return m_Internals->Filter.Min; }

		Vector2u Size() const { return m_Internals->Size; }
		uint32_t Width() const { return m_Internals->Size.Width; }
		uint32_t Height() const { return m_Internals->Size.Height; }

		uint32_t Samples() const { return m_Internals->Samples; }

		Ref<Image> ToImage() const;
		Ref<Image> GetImage(const Vector2u& size, const Vector2i& offset) const;

		void Clear(int value = 0);
		void Clear(void* pixels = nullptr, DataFormat dataFormat = DataFormat::RGBA, DataType dataType = DataType::Int);
		void Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

		void GetPixels(void* pixels, uint32_t size) const;

		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture& texture);
		void Update(const Texture& texture, const Vector2i& offset);

		void Update(Ref<Image> image);
		void Update(Ref<Image> image, const Vector2i& offset);

		void Swap(Texture& to);

		bool IsSRGB() const { return m_Internals->ImageFormat == ImageFormat::SRGB8 || m_Internals->ImageFormat == ImageFormat::SRGB8A8 || m_Internals->ImageFormat == ImageFormat::SRGBA; }

		std::string_view Label() const { return m_Internals->Label; }

		static Vector2u GetMaxDim() { return {GetMaxSize(), GetMaxSize()}; }
		static uint32_t GetMaxSize();

		bool operator==(const Texture& texture) const {
			return m_Internals->ID == texture.m_Internals->ID;
		}

		void Resize(const Vector2u& size);

		ImageFormat ImageFormat() const { return m_Internals->ImageFormat; }
	protected:
		Texture(bool multisampled = false);

		static bool CheckSize(const Vector2u& size);

		void CreateTexture(uint32_t samples, const Vector2u& size, enum ImageFormat ImageFormat, const void* pixels = nullptr, DataType dataType = DataType::UnsignedByte, DataFormat dataFormat = DataFormat::RGBA);

		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat, DataType dataType);

	private:
		struct InternalWrapping {
			WrapMode S = WrapMode::Repeat;
			WrapMode T = WrapMode::Repeat;
		};

		struct InternalFilter {
			FilterMode Mag = FilterMode::Linear;
			FilterMode Min = FilterMode::NearestMipmapLinear;
		};

		struct Internals {
			IDType ID;
			Vector2u Size;

			bool MipMapGenerated = false;
			bool Multisampled = false;

			uint32_t Samples = 1;

			InternalWrapping Wrapping;
			InternalFilter Filter;
			enum ImageFormat ImageFormat;

			std::string Label = {};

			Internals(bool multisampled = false);
			~Internals();

			void Allocate(const Vector2u& size, enum ImageFormat imageFormat);
			void Allocate(const Vector2u& size, uint32_t samples, enum ImageFormat imageFormat);

			void SendImage(const void* pixels, const Vector2u& size, const Vector2i& offset = {0, 0}, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);

			void GetImage(void* pixels, uint32_t size) const;
			void GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset = {0, 0});

			void Clear(void* pixels, DataFormat dataFormat, DataType dataType);
			void Clear(void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

			void CheckSubRegionSize(const Vector2i& offset, const Vector2u& size) const;

			void SetParameter(uint32_t name, int parameter);
			void GetParameter(uint32_t name, int* parameter) const;

			void Invalidate();
		};

		Ref<Internals> m_Internals;
	};
}
