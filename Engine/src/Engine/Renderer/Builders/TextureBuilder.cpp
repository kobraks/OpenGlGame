#include "pch.h"
#include "TextureBuilder.h"

namespace Engine {
	TextureBuilder& TextureBuilder::Size(const Vector2u& size) {
		m_Size = size;

		return *this;
	}

	TextureBuilder& TextureBuilder::Format(ImageFormat imageFormat) {
		m_ImageFormat = imageFormat;

		return *this;
	}

	TextureBuilder& TextureBuilder::Filter(FilterMode min, FilterMode mag) {
		m_FilterMin = min;
		m_FilterMag = mag;

		return *this;
	}

	TextureBuilder& TextureBuilder::Wrap(WrapMode s, WrapMode t) {
		m_WrapS = s;
		m_WrapT = t;

		return *this;
	}

	TextureBuilder& TextureBuilder::Mipmaps(bool generate) {
		m_GenerateMipMaps = generate;

		return *this;
	}

	TextureBuilder& TextureBuilder::Label(const std::string& label) {
		m_Label = label;

		return *this;
	}

	TextureBuilder& TextureBuilder::Samples(uint32_t samples) {
		m_Samples = samples;

		return *this;
	}

	TextureBuilder& TextureBuilder::FromImage(const Ref<Image>& image) {
		m_Image = image;
		m_UseRawData = false;

		return *this;
	}

	TextureBuilder& TextureBuilder::FromRawData(const Buffer& buffer, DataFormat format, DataType type) {
		m_UseRawData = true;
		m_Buffer = buffer;
		m_DataFormat = format;
		m_DataType = type;

		return *this;
	}

	Ref<Texture> TextureBuilder::Build() {
		Ref<Texture> texture = nullptr;

		if (m_UseRawData) {
			texture = Texture::Create(m_Size, m_ImageFormat, m_Samples, m_Label, static_cast<const uint8_t*>(m_Buffer.Data), m_DataType, m_DataFormat);
		} else {
			texture = Texture::Create(m_Image, m_ImageFormat, m_Samples, m_Label);
		}

		if (!texture->IsMultisampled()) {
			texture->SetFilters(m_FilterMin, m_FilterMag);
			texture->SetWrapping(m_WrapS, m_WrapT);
		}

		if (m_GenerateMipMaps)
			texture->GenerateMipMaps();

		return texture;
	}
}
