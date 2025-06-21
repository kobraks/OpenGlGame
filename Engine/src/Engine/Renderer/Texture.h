#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine {
	class Image;

	struct TextureSpec {
		Vector2u Size;
		ImageFormat ImageFormat = ImageFormat::RGBA8;
		uint32_t Samples = 1;
		std::string Label;
		const void* InitialData = nullptr;
		std::optional<DataType> DataType = std::nullopt;
		std::optional<DataFormat> DataFormat = std::nullopt;

		TextureUsage Usage = TextureUsage::Default;
	};

	class Texture {
		friend class Framebuffer;

	public:
		using IDType = uint32_t;

		~Texture() = default;

		[[nodiscard]] explicit operator IDType() const { return m_Internals->ID; }
		[[nodiscard]] IDType RendererID() const { return m_Internals->ID; }

		void GenerateMipMaps() const;

		[[nodiscard]] bool HasMipMapsGenerated() const { return m_Internals->MipMapGenerated; }
		[[nodiscard]] bool IsMultisampled() const { return m_Internals->Multisampled; }

		void Bind() const;
		void BindUnit(uint32_t sampler = 0) const;

		void Unbind() const;
		void UnbindUnit(uint32_t sampler = 0) const;

		[[nodiscard]] static Ref<Texture> Create(const TextureSpec& spec);
		[[nodiscard]] static Ref<Texture> Create(const Ref<Image>& image, ImageFormat imageFormat = ImageFormat::RGBA8, uint32_t samples = 1, const std::string& label = {});
		[[nodiscard]] static Ref<Texture> Create(const Ref<Image>& image, TextureUsage usage, ImageFormat imageFormat = ImageFormat::RGBA8, uint32_t samples = 1, const std::string& label = {});

		void SetLabel(const std::string& label);

		void SetWrapping(WrapMode s);
		void SetWrapping(WrapMode s, WrapMode t);

		void SetWrappingS(WrapMode wrapping);
		void SetWrappingT(WrapMode wrapping);

		[[nodiscard]] WrapMode GetWrappingS() const { return m_Internals->Wrapping.S; }
		[[nodiscard]] WrapMode GetWrappingT() const { return m_Internals->Wrapping.T; }

		void SetFilters(FilterMode min, FilterMode mag);
		void SetMinFilter(FilterMode filter);
		void SetMagFilter(FilterMode filter);

		[[nodiscard]] FilterMode GetMagFilter() const { return m_Internals->Filter.Mag; }
		[[nodiscard]] FilterMode GetMinFilter() const { return m_Internals->Filter.Min; }

		[[nodiscard]] Vector2u Size() const { return m_Internals->Size; }
		[[nodiscard]] uint32_t Width() const { return m_Internals->Size.Width; }
		[[nodiscard]] uint32_t Height() const { return m_Internals->Size.Height; }

		[[nodiscard]] uint32_t Samples() const { return m_Internals->Samples; }

		Ref<Image> ToImage() const;
		Ref<Image> GetImage(const Vector2u& size, const Vector2i& offset) const;

		void Clear(const Color& color);
		void Clear(int value = 0);
		void Clear(const void* pixels = nullptr, DataFormat dataFormat = DataFormat::RGBA, DataType dataType = DataType::Int);

		void ClearRegion(const Color& color, const Vector2i& offset, const Vector2u& size);
		void ClearRegion(int value, const Vector2i& offset, const Vector2u& size);
		void ClearRegion(const void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

		void GetPixels(void* pixels, uint32_t size) const;

		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture& texture);
		void Update(const Texture& texture, const Vector2i& offset);

		void Update(Ref<Image> image);
		void Update(Ref<Image> image, const Vector2i& offset);

		void Swap(Texture& to);

		TextureUsage GetUsage() const { return m_Internals->Usage; }
		void SetUsage(TextureUsage usage) { m_Internals->Usage = usage; }

		[[nodiscard]] bool IsSRGB() const { return m_Internals->ImageFormat == ImageFormat::SRGB8 || m_Internals->ImageFormat == ImageFormat::SRGB8A8 || m_Internals->ImageFormat == ImageFormat::SRGBA; }
		[[nodiscard]] bool IsValid() const { return m_Internals && m_Internals->ID != 0; }

		[[nodiscard]] std::string_view Label() const { return m_Internals->Label; }

		static Vector2u GetMaxDim() { return {GetMaxSize(), GetMaxSize()}; }
		static uint32_t GetMaxSize();

		bool operator==(const Texture& texture) const {
			return m_Internals->ID == texture.m_Internals->ID;
		}

		void Resize(const Vector2u& size);
		void ResizeNoCopy(const Vector2u& size);

		[[nodiscard]] ImageFormat ImageFormat() const { return m_Internals->ImageFormat; }

		void Invalidate();
		void Recreate(const Vector2u& size);

		std::string DebugInfo() const;
	protected:
		Texture(bool multisampled = false);

		void Initialize(uint32_t samples, const Vector2u& size, enum ImageFormat ImageFormat, TextureUsage usage = TextureUsage::Default, const void* pixels = nullptr, DataType dataType = DataType::UnsignedByte, DataFormat dataFormat = DataFormat::RGBA);
		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat, DataType dataType);

		void ReAlloc(const Vector2u& size);
		void SetupStorage(uint32_t samples = 1, const Vector2u& size = {1, 1}, TextureUsage usage = TextureUsage::Default, enum ImageFormat imageFormat = ImageFormat::RGBA8);

		void UploadPixels(const void* pixels, const Vector2u& size = {}, const Vector2i& offset = { 0, 0 }, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);

		void GetImage(void* pixels, uint32_t size) const;
		void GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset = { 0, 0 }) const;

		void SetParameter(uint32_t name, int parameter);
		void GetParameter(uint32_t name, int* parameter) const;

		void SetupDefaultParameters(TextureUsage usage);

		static void ValidateSize(Vector2u size);

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
			enum ImageFormat ImageFormat = ImageFormat::RGBA8;
			uint32_t ImageFormatGL = 0;
			TextureUsage Usage = TextureUsage::Default;

			std::string Label = {};

			Internals(bool multisampled = false);
			~Internals();

			void Invalidate();
		};

		Ref<Internals> m_Internals;
	};
}
