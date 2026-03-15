#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Renderer/GraphicEnums.h"

#include "Engine/Asset/Asset.h"

#include <optional>

namespace Engine {
	class Image;

	struct TextureSpec {
		struct InitialPixels {
			const void* Pixels = nullptr;
			Vector2u Size{}; // 0, 0 -> default to texture size
			uint32_t RowStrideBytes = 0; // 0 -> tightly packed (width * bpp)

			std::optional<DataFormat> Format = std::nullopt; // if empty -> derive from ImageFormat
			std::optional<DataType> DataType = std::nullopt; // if empty -> derive from ImageFormat

			bool FlipY = false; // flip the image data before upload
			uint32_t MipLevel = 0; // which mip level to upload to
		};

		Vector2u Size;
		ImageFormat ImageFormat = ImageFormat::RGBA8;

		uint32_t Samples = 1; // multisampled (no pixel upload)
		uint32_t Levels = 1; // Only valid for samples == 1; 0 means auto

		std::string Label;
		
		TextureUsage Usage = TextureUsage::Default;
		bool GenerateMips = false; // After upload; ignored when Levels <= 1 or Samples > 1

		std::optional<InitialPixels> Picture;
	};

	class Texture : public Asset {
		friend class Framebuffer;

	public:
		using IDType = uint32_t;

		~Texture() = default;

		AssetType GetType() const override { return AssetType::Texture; }

		[[nodiscard]] explicit operator IDType() const { return m_GLState->ID; }
		[[nodiscard]] IDType RendererID() const { return m_GLState->ID; }

		void GenerateMipMaps() const;

		[[nodiscard]] bool HasMipMapsGenerated() const { return m_GLState->MipMapGenerated; }
		[[nodiscard]] bool IsMultisampled() const { return m_GLState->Multisampled; }

		void Bind() const;
		void BindUnit(uint32_t sampler = 0) const;

		void Unbind() const;
		void UnbindUnit(uint32_t sampler = 0) const;

		[[nodiscard]] static Ref<Texture> Create(const TextureSpec& spec);
		[[nodiscard]] static Ref<Texture> Create(const Ref<Image>& image, ImageFormat imageFormat = ImageFormat::RGBA8, uint32_t levels = 1, uint32_t samples = 1, const std::string& label = {});
		[[nodiscard]] static Ref<Texture> Create(const Ref<Image>& image, TextureUsage usage, ImageFormat imageFormat = ImageFormat::RGBA8, uint32_t levels = 1, uint32_t samples = 1, const std::string& label = {});

		void SetLabel(const std::string& label);

		void SetWrapping(WrapMode s);
		void SetWrapping(WrapMode s, WrapMode t);

		void SetWrappingS(WrapMode wrapping);
		void SetWrappingT(WrapMode wrapping);

		[[nodiscard]] WrapMode GetWrappingS() const { return m_GLState->Wrapping.S; }
		[[nodiscard]] WrapMode GetWrappingT() const { return m_GLState->Wrapping.T; }

		void SetFilters(FilterMode min, FilterMode mag);
		void SetMinFilter(FilterMode filter);
		void SetMagFilter(FilterMode filter);

		[[nodiscard]] FilterMode GetMagFilter() const { return m_GLState->Filter.Mag; }
		[[nodiscard]] FilterMode GetMinFilter() const { return m_GLState->Filter.Min; }

		[[nodiscard]] Vector2u Size() const { return m_GLState->Size; }
		[[nodiscard]] uint32_t Width() const { return m_GLState->Size.Width; }
		[[nodiscard]] uint32_t Height() const { return m_GLState->Size.Height; }

		[[nodiscard]] uint32_t Samples() const { return m_GLState->Samples; }
		[[nodiscard]] uint32_t Levels() const { return m_GLState->Levels; }

		[[nodiscard]] Ref<Image> ToImage(uint32_t mipLevel = 0) const;
		[[nodiscard]] Ref<Image> GetImage(const Vector2u& size, const Vector2i& offset, uint32_t mipLevel = 0) const;

		void Clear(const Color& color);
		void Clear(int value = 0);
		void Clear(const void* pixels = nullptr, DataFormat dataFormat = DataFormat::RGBA, DataType dataType = DataType::Int);

		void ClearRegion(const Color& color, const Vector2i& offset, const Vector2u& size);
		void ClearRegion(int value, const Vector2i& offset, const Vector2u& size);
		void ClearRegion(const void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat, DataType dataType);

		void GetPixels(void* pixels, uint32_t bufSize, DataFormat format, DataType type, uint32_t mipLevel = 0, uint32_t rowStrideBytes = 0) const;

		void Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);
		void Update(const Color* pixels, const Vector2u& size, const Vector2i& offset);

		void Update(const Texture& texture);
		void Update(const Texture& texture, const Vector2i& offset);

		void Update(Ref<Image> image);
		void Update(Ref<Image> image, const Vector2i& offset);

		void Swap(Texture& to);

		TextureUsage GetUsage() const { return m_GLState->Usage; }
		void SetUsage(TextureUsage usage) { m_GLState->Usage = usage; }

		[[nodiscard]] bool IsSRGB() const { return m_GLState->ImageFormat == ImageFormat::SRGB8 || m_GLState->ImageFormat == ImageFormat::SRGB8A8 || m_GLState->ImageFormat == ImageFormat::SRGBA; }
		[[nodiscard]] bool IsValid() const { return m_GLState && m_GLState->ID != 0; }

		[[nodiscard]] std::string_view Label() const { return m_GLState->Label; }

		static Vector2u GetMaxDim() { return {GetMaxSize(), GetMaxSize()}; }
		static uint32_t GetMaxSize();
		static uint32_t QueryMaxColorSamples();
		static uint32_t QueryMaxIntegerSamples();
		static uint32_t QueryMaxDepthSamples();

		bool operator==(const Texture& texture) const {
			return m_GLState->ID == texture.m_GLState->ID;
		}

		void Resize(const Vector2u& size);
		void ResizeNoCopy(const Vector2u& size);

		[[nodiscard]] ImageFormat ImageFormat() const { return m_GLState->ImageFormat; }

		void Invalidate();
		void Recreate(const Vector2u& size);

		std::string DebugInfo() const;
	protected:
		Texture(bool multisampled = false);

		void Initialize(const TextureSpec& spec);
		void Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat, DataType dataType);

		void ReAlloc(const Vector2u& size);
		void SetupStorage(uint32_t levels = 1, uint32_t samples = 1, const Vector2u& size = {1, 1}, TextureUsage usage = TextureUsage::Default, enum ImageFormat imageFormat = ImageFormat::RGBA8);

		void UploadPixels(const void* pixels, const Vector2u& size = {}, const Vector2i& offset = { 0, 0 }, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte);
		void UploadPixels(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format, DataType dataType, uint32_t mipLevel, uint32_t rowStrideBytes = 0, bool flipY = false);

		void GetImage(void* pixels, uint32_t bufSize, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte, uint32_t mipLevel = 0) const;
		void GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset = { 0, 0 }, DataFormat format = DataFormat::RGBA, DataType dataType = DataType::UnsignedByte, uint32_t mipLevel = 0) const;

		void SetParameter(uint32_t name, int parameter);
		void GetParameter(uint32_t name, int* parameter) const;

		void SetupDefaultParameters(TextureUsage usage);

		static void ValidateSize(Vector2u size);

		void ReapplyParameters();

	private:
		struct InternalWrapping {
			WrapMode S = WrapMode::Repeat;
			WrapMode T = WrapMode::Repeat;
		};

		struct InternalFilter {
			FilterMode Mag = FilterMode::Linear;
			FilterMode Min = FilterMode::NearestMipmapLinear;
		};

		struct GLState {
			IDType ID;
			Vector2u Size;

			bool MipMapGenerated = false;
			bool Multisampled = false;

			uint32_t Samples = 1;
			uint32_t Levels = 1;

			InternalWrapping Wrapping;
			InternalFilter Filter;
			enum ImageFormat ImageFormat = ImageFormat::RGBA8;
			uint32_t ImageFormatGL = 0;
			TextureUsage Usage = TextureUsage::Default;

			std::string Label = {};

			GLState(bool multisampled = false);
			~GLState();

			void Invalidate();
		};

		Ref<GLState> m_GLState;
	};
}
