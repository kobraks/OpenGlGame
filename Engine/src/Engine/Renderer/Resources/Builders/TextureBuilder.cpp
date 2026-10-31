#include "pch.h"
#include "TextureBuilder.h"

#include "Engine/Utils/Renderer/FilterModeUtils.h"

#include "imgui_internal.h"
#include "Engine/Core/Image.h"

#include <cmath>
#include <algorithm>

namespace Engine {
	TextureBuilder& TextureBuilder::Size(const Vector2u& size) {
		m_Size = size;

		return *this;
	}

	TextureBuilder& TextureBuilder::ForceCustomSize(bool forceCustomSize) {
		m_ForceCustomSize = forceCustomSize;

		return *this;
	}

	TextureBuilder& TextureBuilder::Format(ImageFormat imageFormat) {
		m_ImageFormat = imageFormat;

		return *this;
	}

	TextureBuilder& TextureBuilder::Filter(FilterMode min, FilterMode mag) {
		m_FilterMin = min;
		m_FilterMag = Utils::SanitizeMag(mag);

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

	TextureBuilder& TextureBuilder::Levels(uint32_t levels) {
		m_Levels = levels;
		return *this;
	}

	TextureBuilder& TextureBuilder::LevelsAuto() {
		m_Levels = 0;
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

	TextureBuilder& TextureBuilder::RowStrideBytes(uint32_t bytes) {
		m_RowStrideBytes = bytes;
		return *this;
	}

	TextureBuilder& TextureBuilder::FlipY(bool flip) {
		m_FlipY = flip;
		return *this;
	}

	TextureBuilder& TextureBuilder::PictureMip(uint32_t level) {
		m_PictureMipLevel = level;
		return *this;
	}

	TextureBuilder& TextureBuilder::Clear() {
		m_Size = { 1, 1 };
		m_ImageFormat = ImageFormat::RGBA8;

		m_Samples = 1;
		m_Levels = 1;

		m_Usage = TextureUsage::Default;

		m_FilterMin = FilterMode::Linear;
		m_FilterMag = FilterMode::Linear;

		m_WrapS = WrapMode::ClampEdge;
		m_WrapT = WrapMode::ClampEdge;

		m_GenerateMipMaps = false;
		m_Label = {};

		m_Image = nullptr;

		m_Buffer.Release();
		m_DataFormat = DataFormat::RGBA;
		m_DataType = DataType::UnsignedByte;
		m_UseRawData = false;

		m_RowStrideBytes = 0;
		m_PictureMipLevel = 0;
		m_FlipY = false;

		m_ForceCustomSize = false;

		return *this;
	}

	TextureSpec TextureBuilder::BuildSpecification() const {
		TextureSpec spec;

		if (!m_UseRawData && m_Image && (m_Size.Width < m_Image->Width() || m_Size.Height < m_Image->Height()) && !m_ForceCustomSize) {
			LOG_ENGINE_WARN("Image provided (Image::Size() = {}) is smaller than builder-set size {}. Using Image::Size() instead.", m_Image->Size(), m_Size);
		}

		spec.ImageFormat = m_ImageFormat;
		spec.Samples = m_Samples;
		spec.Label = m_Label;
		spec.Usage = m_Usage;

		if (m_UseRawData) spec.Size = m_Size;
		else if (m_Image && !m_ForceCustomSize) {
			Vector2u size;

			size.Width = std::max(m_Size.Width, m_Image->Width());
			size.Height = std::max(m_Size.Height, m_Image->Height());

			spec.Size = size;
		}
		else spec.Size = m_Size;

		if (spec.Samples > 1) {
			if (m_UseRawData || m_Image) {
				LOG_ENGINE_WARN("TextureBuilder: Ignoring provided image or raw data for multisampled texture (samples = {})", spec.Samples);
			}
			spec.Picture = std::nullopt;
		} else if (m_UseRawData) {
			TextureSpec::InitialPixels pic{};

			pic.Pixels = m_Buffer.Data();
			pic.Size = m_Size;
			pic.RowStrideBytes = m_RowStrideBytes;
			pic.Format = m_DataFormat;
			pic.DataType = m_DataType;
			pic.FlipY = m_FlipY;
			pic.MipLevel = m_PictureMipLevel;

			spec.Picture = pic;
		} else if (m_Image) {
			TextureSpec::InitialPixels pic{};

			pic.Pixels = m_Image->GetPixels().data();
			pic.Size = m_Image->Size();
			pic.RowStrideBytes = 0;
			pic.Format = DataFormat::RGBA;
			pic.DataType = DataType::UnsignedByte;
			pic.FlipY = m_FlipY;
			pic.MipLevel = m_PictureMipLevel;

			spec.Picture = pic;
		} else {
			spec.Picture = std::nullopt;
		}

		if (spec.Samples > 1) {
			if (m_Levels > 1) {
				LOG_ENGINE_WARN("TextureBuilder: Ignoring builder-set levels {} for multisampled texture (samples = {})", m_Levels, spec.Samples);
			}
			spec.Levels = 1;
		} else {
			if (m_Levels == 0) {
				// Auto levels
				const uint32_t longer = std::max(spec.Size.Width, spec.Size.Height);
				spec.Levels = static_cast<uint32_t>(std::floor(std::log2(longer))) + 1;
			} else {
				spec.Levels = m_Levels;
			}

			if (spec.Levels > 1)
				spec.GenerateMips = m_GenerateMipMaps;
		}

		return spec;
	}

	Ref<Texture> TextureBuilder::Build() const {
		auto texture = Texture::Create(BuildSpecification());

		if (!texture->IsMultisampled()) {
			texture->SetFilters(m_FilterMin, Utils::SanitizeMag(m_FilterMag));
			texture->SetWrapping(m_WrapS, m_WrapT);
		}

		return texture;
	}
}
