#include "pch.h"
#include "Engine/Core/Image.h"

#include "Engine/Core/Assert.h"

#include <algorithm>
#include <FreeImage.h>

namespace Engine {
	static constexpr FREE_IMAGE_FORMAT ConvertType(const ImageType type) {
		switch(type) {
			case ImageType::BMP:
				return FIF_BMP;
			case ImageType::EXR:
				return FIF_EXR;
			case ImageType::J2K:
				return FIF_J2K;
			case ImageType::JP2:
				return FIF_JP2;
			case ImageType::JPEG:
				return FIF_JPEG;
			case ImageType::JXR:
				return FIF_JXR;
			case ImageType::PBM:
				return FIF_PBM;
			case ImageType::PGM:
				return FIF_PGM;
			case ImageType::PNG:
				return FIF_PNG;
			case ImageType::PPM:
				return FIF_PPM;
			case ImageType::TIFF:
				return FIF_TIFF;
			default:
				return FIF_UNKNOWN;
		}
	}

	Image::Image(uint32_t width, uint32_t height, const Color &background) {
		Create(width, height, background);
	}

	Image::Image(uint32_t width, uint32_t height, const Color *pixels) {
		ENGINE_ASSERT(pixels);
		Prepare(width, height);

		std::copy_n(pixels, m_Pixels.size(), std::begin(m_Pixels));
	}

	Image::Image(uint32_t width, uint32_t height, const uint8_t *pixels) {
		ENGINE_ASSERT(pixels);
		Prepare(width, height);

		for (size_t i = 0; i < m_Pixels.size(); i++)
			m_Pixels[i] = Color(pixels[i * 4 + 0], pixels[i * 4 + 1], pixels[i * 4 + 2], pixels[i * 4 + 3]);
	}

	Image::Image(uint32_t width, uint32_t height, const glm::vec4 *pixels) {
		ENGINE_ASSERT(pixels);
		Prepare(width, height);

		std::copy_n(pixels, static_cast<size_t>(width) * height, std::begin(m_Pixels));
	}

	Image::Image(uint32_t width, uint32_t height, const float *pixels) {
		ENGINE_ASSERT(pixels);
		Prepare(width, height);

		for (size_t i = 0; i < m_Pixels.size(); i++)
			m_Pixels[i] = Color(pixels[i * 4 + 0], pixels[i * 4 + 1], pixels[i * 4 + 2], pixels[i * 4 + 3]);
	}

	Image::Image(const Vector2u &size, const Color &background) : Image(size.Width, size.Height, background) {}
	Image::Image(const Vector2u &size, const Color *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const uint8_t *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const glm::vec4 *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const float *pixels) : Image(size.Width, size.Height, pixels) {}

	Image::Image(const Image &img) noexcept{
		*this = img;
	}

	Image::Image(Image &&img) noexcept{
		*this = std::move(img);
	}

	void Image::Create(uint32_t width, uint32_t height, const Color &background) {
		Clear();

		Prepare(width, height);

		std::ranges::fill(m_Pixels, background);
	}

	void Image::Create(const Vector2u &size, const Color &background) {
		Create(size.Width, size.Height, background);
	}

	void Image::Clear() {
		m_Width = m_Height = 0;
		m_Pixels.clear();
	}

	void Image::Load(uint8_t *buffer, size_t size) {
		Clear();

		auto stream       = FreeImage_OpenMemory(buffer, size);
		const auto format = FreeImage_GetFileTypeFromMemory(stream, 0);

		ENGINE_ASSERT(format != FIF_UNKNOWN);
		if(format == FIF_UNKNOWN)
			throw std::runtime_error("Unknown image format");

		const auto image = FreeImage_LoadFromMemory(format, stream, 0);

		ENGINE_ASSERT(image);
		if(!image)
			throw std::runtime_error("Unable to load image from memory");

		LoadToMemory(image);
		FreeImage_Unload(image);
		FreeImage_CloseMemory(stream);
	}

	void Image::Load(const std::filesystem::path &path) {
		Clear();

		const std::string sPath = path.string();

		std::ifstream file;
		file.exceptions(std::ios::failbit | std::ios::badbit);
		file.open(sPath);
		file.close();

		const auto format = FreeImage_GetFileType(sPath.c_str());

		ENGINE_ASSERT(format != FIF_UNKNOWN);
		if(format == FIF_UNKNOWN)
			throw std::runtime_error(fmt::format("Unknown image format in '{}' file", sPath));

		const auto image = FreeImage_Load(format, sPath.c_str());

		ENGINE_ASSERT(image);
		if(!image)
			throw std::runtime_error(fmt::format("Unable to load '{}' file", sPath));

		LoadToMemory(image);
		FreeImage_Unload(image);
	}


	void Image::Save(const std::filesystem::path &path, ImageType type) {
		const auto sPath = path.string();

		auto handler = FreeImage_Allocate(
		                                  static_cast<int>(m_Width),
		                                  static_cast<int>(m_Height),
		                                  32,
		                                  FI_RGBA_RED_MASK,
		                                  FI_RGBA_GREEN_MASK,
		                                  FI_RGBA_BLUE_MASK
		                                 );

		for(size_t i = 0; i < m_Width; ++i)
			for(size_t j = 0; j < m_Height; ++i) {
				RGBQUAD c;

				const auto pixel = m_Pixels[i + j * m_Width];

				c.rgbRed      = static_cast<BYTE>(pixel.R);
				c.rgbGreen    = static_cast<BYTE>(pixel.G);
				c.rgbBlue     = static_cast<BYTE>(pixel.B);
				c.rgbReserved = static_cast<BYTE>(pixel.A);

				FreeImage_SetPixelColor(handler, static_cast<uint32_t>(i), static_cast<uint32_t>(j), &c);
			}

		FreeImage_Save(ConvertType(type), handler, sPath.c_str(), 0);
		FreeImage_Unload(handler);
	}

	Color& Image::GetPixel(uint32_t x, uint32_t y) {
		ENGINE_ASSERT(x < m_Width && y < m_Height);

		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Put of range");

		return m_Pixels[x + y * m_Width];
	}

	const Color& Image::GetPixel(uint32_t x, uint32_t y) const {
		ENGINE_ASSERT(x < m_Width && y < m_Height);

		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Put of range");

		return m_Pixels[x + y * m_Width];
	}

	void Image::SetPixel(uint32_t x, uint32_t y, const Color &color) {
		GetPixel(x, y) = color;
	}

	Image& Image::operator=(Image &&img) noexcept {
		Clear();

		m_Width  = img.m_Width;
		m_Height = img.m_Height;

		m_Pixels = std::move(img.m_Pixels);

		return *this;
	}

	Image& Image::operator=(const Image &img) noexcept {
		Clear();
		Prepare(img.m_Width, m_Height);

		std::ranges::copy(img.m_Pixels, std::begin(m_Pixels));

		return *this;
	}

	void Image::LoadToMemory(void *buffer) {
		auto image = static_cast<FIBITMAP*>(buffer);
		image      = FreeImage_ConvertTo32Bits(image);

		Prepare(FreeImage_GetWidth(image), FreeImage_GetHeight(image));

		auto pixels = FreeImage_GetBits(image);

		for(size_t i = 0; i < m_Pixels.size(); ++i) {
			m_Pixels[i] = Color(
			                    pixels[i * 4 + FI_RGBA_RED],
			                    pixels[i * 4 + FI_RGBA_GREEN],
			                    pixels[i * 4 + FI_RGBA_BLUE],
			                    pixels[i * 4 + FI_RGBA_ALPHA]
			                   );
		}

		FreeImage_Unload(image);
	}

	void Image::Prepare(uint32_t width, uint32_t height) {
		const auto size = static_cast<size_t>(width) * height;
		m_Width  = width;
		m_Height = height;

		m_Pixels.resize(size);
	}
}
