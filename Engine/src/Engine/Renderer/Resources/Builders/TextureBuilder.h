#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Renderer/Resources/Texture.h"
#include "Engine/Core/Buffer.h"

namespace Engine {
	class TextureBuilder {
	public:
		TextureBuilder& Size(const Vector2u& size);
		TextureBuilder& ForceCustomSize(bool forceCustomSize = true);
		TextureBuilder& Format(TextureInternalFormat imageFormat);
		TextureBuilder& Filter(FilterMode min, FilterMode mag);
		TextureBuilder& Wrap(WrapMode s, WrapMode t);
		TextureBuilder& Mipmaps(bool generate = true);
		TextureBuilder& Label(const std::string& label);
		TextureBuilder& Samples(uint32_t samples);
		TextureBuilder& Levels(uint32_t levels);
		TextureBuilder& LevelsAuto();
		TextureBuilder& Usage(TextureUsage usage);

		TextureBuilder& FromImage(const Ref<Image>& image);
		TextureBuilder& FromImage(const Ref<Image>& image, TextureUsage usage);
		TextureBuilder& FromRawData(const Buffer& buffer, PixelFormat format, PixelDataType type);

		TextureBuilder& RowStrideBytes(uint32_t bytes);
		TextureBuilder& FlipY(bool flip = true);
		TextureBuilder& PictureMip(uint32_t level);

		TextureBuilder& Clear();

		TextureSpec BuildSpecification() const;
		Ref<Texture> Build() const;
	private:
		Vector2u m_Size{ 1, 1 };
		TextureInternalFormat m_ImageFormat = TextureInternalFormat::RGBA8;

		uint32_t m_Samples = 1;
		uint32_t m_Levels = 1; // Only valid for samples == 1, 0 means auto

		TextureUsage m_Usage = TextureUsage::Default;
		
		FilterMode m_FilterMin = FilterMode::Linear;
		FilterMode m_FilterMag = FilterMode::Linear;

		WrapMode m_WrapS = WrapMode::ClampEdge;
		WrapMode m_WrapT = WrapMode::ClampEdge;

		bool m_GenerateMipMaps = false;
		std::string m_Label = {};

		Ref<Image> m_Image = nullptr;

		Buffer m_Buffer{};
		PixelFormat m_DataFormat = PixelFormat::RGBA;
		PixelDataType m_DataType = PixelDataType::UnsignedByte;
		bool m_UseRawData = false;

		uint32_t m_RowStrideBytes = 0;
		uint32_t m_PictureMipLevel = 0;
		bool m_FlipY = false;
		bool m_ForceCustomSize = false;
	};
}