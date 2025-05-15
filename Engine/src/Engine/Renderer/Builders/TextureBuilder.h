#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Core/Buffer.h"

namespace Engine {
	class TextureBuilder {
	public:
		TextureBuilder& Size(const Vector2u& size);
		TextureBuilder& Format(ImageFormat imageFormat);
		TextureBuilder& Filter(FilterMode min, FilterMode mag);
		TextureBuilder& Wrap(WrapMode s, WrapMode t);
		TextureBuilder& Mipmaps(bool generate = true);
		TextureBuilder& Label(const std::string& label);
		TextureBuilder& Samples(uint32_t samples);
		TextureBuilder& Usage(TextureUsage usage);

		TextureBuilder& FromImage(const Ref<Image>& image);
		TextureBuilder& FromImage(const Ref<Image>& image, TextureUsage usage);
		TextureBuilder& FromRawData(const Buffer& buffer, DataFormat format, DataType type);

		Ref<Texture> Build();
	protected:
		TextureSpec CreateSpec() const;

	private:
		Vector2u m_Size{ 1, 1 };
		ImageFormat m_ImageFormat = ImageFormat::RGBA8;
		uint32_t m_Samples = 1;

		TextureUsage m_Usage = TextureUsage::Default;
		
		FilterMode m_FilterMin = FilterMode::Linear;
		FilterMode m_FilterMag = FilterMode::Linear;

		WrapMode m_WrapS = WrapMode::ClampEdge;
		WrapMode m_WrapT = WrapMode::ClampEdge;

		bool m_GenerateMipMaps = false;
		std::string m_Label = {};

		Ref<Image> m_Image = nullptr;

		Buffer m_Buffer{};
		DataFormat m_DataFormat = DataFormat::RGBA;
		DataType m_DataType = DataType::UnsignedByte;
		bool m_UseRawData = false;
	};
}