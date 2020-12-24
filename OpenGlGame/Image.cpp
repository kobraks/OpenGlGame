#include "pch.h"
#include "Image.h"

#include <algorithm>

#include <FreeImage/FreeImage.h>

#include "Assert.h"

namespace Game
{
	static constexpr FREE_IMAGE_FORMAT ConvertType(const ImageType type)
	{
		switch(type)
		{
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

	Image::Image(const uint32_t width, const uint32_t height, const glm::vec4 &background)
	{
		Create(width, height, background);
	}

	Image::Image(const uint32_t width, const uint32_t height, const uint8_t *pixels)
	{
		m_Pixels = new glm::vec4[width * height];
		for(uint32_t i = 0; i < width; ++i)
			for(uint32_t j = 0; j < height; ++j)
			{
				size_t colorIndex = (i + j * width) * sizeof(glm::vec4);

				m_Pixels[i + j * width] = TranslateColor(
				                                         pixels[colorIndex + 0],
				                                         pixels[colorIndex + 1],
				                                         pixels[colorIndex + 2],
				                                         pixels[colorIndex + 3]
				                                        );
			}
	}

	Image::Image(const uint32_t width, const uint32_t height, const glm::vec4 *pixels) : m_Width(width),
		m_Height(height)
	{
		m_Pixels = new glm::vec4[width * height];
		std::memcpy(m_Pixels, pixels, width * height);
	}

	Image::Image(const Vector2u &size, const glm::vec4 &background) : Image(size.X, size.Y, background) {}
	Image::Image(const Vector2u &size, const uint8_t *pixels) : Image(size.X, size.Y, pixels) {}
	Image::Image(const Vector2u &size, const glm::vec4 *pixels) : Image(size.X, size.Y, pixels) {}

	Image::Image(const uint8_t *pixels, const uint32_t size)
	{
		Load(pixels, size);
	}

	Image::Image(const std::string &fileName)
	{
		Load(fileName);
	}

	Image::Image(const Image &image)
	{
		*this = image;
	}

	Image::Image(Image &&image) noexcept
	{
		*this = std::move(image);
	}

	Image::~Image()
	{
		delete[] m_Pixels;
	}

	void Image::Create(uint32_t width, uint32_t height, const glm::vec4 &background)
	{
		delete[] m_Pixels;

		m_Width  = width;
		m_Height = height;

		m_Pixels = new glm::vec4(width * height);

		std::fill_n(m_Pixels, width * height, background);
	}

	void Image::Load(const uint8_t *pixels, uint32_t size)
	{
		delete[] m_Pixels;

		m_Pixels = nullptr;
		m_Width  = m_Height = 0;

		auto format = FreeImage_GetFileTypeFromMemory(reinterpret_cast<FIMEMORY*>(const_cast<uint8_t*>(pixels)), size);
		if(format == FIF_UNKNOWN)
		{
			throw std::runtime_error("Unknown image format");
		}

		auto image = FreeImage_LoadFromMemory(format, reinterpret_cast<FIMEMORY*>(const_cast<uint8_t*>(pixels)), size);

		if(!image)
		{
			throw std::runtime_error("Unable to open memory file");
		}

		LoadToMemory(image);
		FreeImage_Unload(image);
	}

	void Image::Load(const std::string &fileName)
	{
		delete[] m_Pixels;
		m_Width = m_Height = 0;

		std::ifstream file;
		file.exceptions(std::ios::failbit | std::ios::badbit);
		file.open(fileName);
		file.close();

		const auto format = FreeImage_GetFileType(fileName.c_str(), 0);
		if(format == FIF_UNKNOWN)
		{
			throw std::runtime_error("Unknown image format");
		}

		const auto image = FreeImage_Load(format, fileName.c_str());
		if(!image)
		{
			throw std::runtime_error("Unable to open file");
		}

		LoadToMemory(image);
		FreeImage_Unload(image);
	}

	void Image::Save(const std::string &fileName, ImageType type)
	{
		auto handler = FreeImage_Allocate(
		                                  m_Width,
		                                  m_Height,
		                                  32,
		                                  FI_RGBA_RED_MASK,
		                                  FI_RGBA_GREEN_MASK,
		                                  FI_RGBA_BLUE_MASK
		                                 );

		for(size_t i = 0; i < m_Width; ++i)
			for(size_t j = 0; j < m_Height; ++j)
			{
				RGBQUAD color;

				const auto pixel  = m_Pixels[i + j * m_Width];
				color.rgbRed      = static_cast<BYTE>(pixel.r * 255);
				color.rgbGreen    = static_cast<BYTE>(pixel.g * 255);
				color.rgbBlue     = static_cast<BYTE>(pixel.b * 255);
				color.rgbReserved = static_cast<BYTE>(pixel.a * 255);

				FreeImage_SetPixelColor(handler, i, j, &color);
			}

		FreeImage_Save(ConvertType(type), handler, fileName.c_str(), 0);
	}

	glm::vec4& Image::GetPixel(uint32_t x, uint32_t y)
	{
		ASSERT(x > m_Width || y > m_Height, "Out of range");
		if(x > m_Width || y > m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	glm::vec4 Image::GetPixel(uint32_t x, uint32_t y) const
	{
		ASSERT(x > m_Width || y > m_Height, "Out of range");
		if(x > m_Width || y > m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	void Image::SetPixel(uint32_t x, uint32_t y, const glm::vec4 &color)
	{
		ASSERT(x > m_Width || y > m_Height, "Out of range");
		if(x > m_Width || y > m_Height)
			throw std::out_of_range("Out of range");

		m_Pixels[x + y * m_Width] = color;
	}

	Image& Image::operator=(Image &&image) noexcept
	{
		m_Width  = image.m_Width;
		m_Height = image.m_Height;

		delete[] m_Pixels;
		m_Pixels       = image.m_Pixels;
		image.m_Pixels = nullptr;

		return *this;
	}

	Image& Image::operator=(const Image &image)
	{
		delete[] m_Pixels;

		m_Width  = image.m_Width;
		m_Height = image.m_Height;

		const size_t size = m_Width * m_Height;

		m_Pixels = new glm::vec4[size];
		std::memcpy(m_Pixels, image.m_Pixels, size);

		return *this;
	}

	void Image::LoadToMemory(void *imageFile)
	{
		auto image = reinterpret_cast<FIBITMAP*>(imageFile);
		image      = FreeImage_ConvertTo32Bits(image);

		m_Width  = FreeImage_GetWidth(image);
		m_Height = FreeImage_GetHeight(image);

		auto pixels = reinterpret_cast<uint8_t*>(FreeImage_GetBits(image));

		m_Pixels = new glm::vec4[m_Width * m_Height];

		for(size_t i    = 0; i < m_Width * m_Height; ++i)
			m_Pixels[i] = TranslateColor(
			                             pixels[i * 4 + FI_RGBA_RED],
			                             pixels[i * 4 + FI_RGBA_GREEN],
			                             pixels[i * 4 + FI_RGBA_BLUE],
			                             pixels[i * 4 + FI_RGBA_ALPHA]
			                            );

		FreeImage_Unload(image);
	}
}
