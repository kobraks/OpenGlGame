#include "pch.h"
#include "TextureBuilder.h"

#include "imgui_internal.h"
#include "Engine/Core/Image.h"

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

	TextureBuilder& TextureBuilder::Usage(TextureUsage usage) {
		m_Usage = usage;

		return *this;
	}

	TextureBuilder& TextureBuilder::FromImage(const Ref<Image>& image) {
		m_Image = image;
		m_UseRawData = false;

		return *this;
	}

	TextureBuilder& TextureBuilder::FromImage(const Ref<Image>& image, TextureUsage usage) {
		m_Image = image;
		m_UseRawData = false;
		m_Usage = usage;

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
		auto texture = Texture::Create(CreateSpec());

		if (!texture->IsMultisampled()) {
			texture->SetFilters(m_FilterMin, m_FilterMag);
			texture->SetWrapping(m_WrapS, m_WrapT);
		}

		if (m_GenerateMipMaps)
			texture->GenerateMipMaps();

		return texture;
	}

	TextureSpec TextureBuilder::CreateSpec() const {
		TextureSpec spec;

		if (!m_UseRawData && m_Image && m_Size != m_Image->Size()) {
			LOG_ENGINE_WARN("TextureBuilder: Ignoring builder-set size {} and using Image::Size() = {}", m_Size, m_Image->Size());
		}

		spec.ImageFormat = m_ImageFormat;
		spec.Samples = m_Samples;
		spec.Label = m_Label;
		spec.Usage = m_Usage;

		if (m_UseRawData) {
			spec.Size = m_Size;
			spec.InitialData = static_cast<const uint8_t*>(m_Buffer.Data);
			spec.DataFormat = m_DataFormat;
			spec.DataType = m_DataType;
		} else if (m_Image) {
			spec.Size = m_Image->Size();
			spec.InitialData = m_Image->GetPixels().data();
			spec.DataFormat = DataFormat::RGBA;
			spec.DataType = DataType::UnsignedByte;
		} else {
			spec.Size = m_Size;
		}

		return spec;
	}
}
