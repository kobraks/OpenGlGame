#include "pch.h"
#include "Image.h"

#include <algorithm>
#include <FreeImage.h>

#include "Assert.h"

namespace Game
{
	static constexpr FREE_IMAGE_FORMAT ConvertType(const ImageType type)
	{
		switch(type)
		{
			case ImageType::BMP: return FIF_BMP;
			case ImageType::EXR: return FIF_EXR;
			case ImageType::J2K: return FIF_J2K;
			case ImageType::JP2: return FIF_JP2;
			case ImageType::JPEG: return FIF_JPEG;
			case ImageType::JXR: return FIF_JXR;
			case ImageType::PBM: return FIF_PBM;
			case ImageType::PGM: return FIF_PGM;
			case ImageType::PNG: return FIF_PNG;
			case ImageType::PPM: return FIF_PPM;
			case ImageType::TIFF: return FIF_TIFF;
			default: return FIF_UNKNOWN;
		}
	}

	Image::Image(uint32_t width, uint32_t height, const Color &background)
	{
		Create(width, height, background);
	}

	Image::Image(uint32_t width, uint32_t height, const Color *pixels) : m_Width(width), m_Height(height)
	{
		m_Pixels = new Color[static_cast<size_t>(width) * height];
		std::memcpy(m_Pixels, pixels, static_cast<size_t>(width) * height * sizeof(Color));
	}

	Image::Image(uint32_t width, uint32_t height, const uint8_t *pixels) : m_Width(width), m_Height(height)
	{
		m_Pixels = new Color[static_cast<size_t>(width) * height];

		std::memcpy(m_Pixels, pixels, static_cast<size_t>(width) * height * sizeof(Color));
	}

	Image::Image(uint32_t width, uint32_t height, const glm::vec4 *pixels) : m_Width(width), m_Height(height)
	{
		m_Pixels = new Color[static_cast<size_t>(width) * height];

		for(uint32_t i = 0; i < width; ++i)
			for(uint32_t j = 0; j < height; ++j)
			{
				const auto &pixel       = pixels[i + j * width];
				m_Pixels[i + j * width] = Color(pixel);
			}
	}

	Image::Image(uint32_t width, uint32_t height, const float *pixels) : m_Width(width), m_Height(height)
	{
		m_Pixels = new Color[static_cast<size_t>(width) * height];

		for(uint32_t i = 0; i < width; ++i)
			for(uint32_t j = 0; j < height; ++j)
			{
				const size_t colorIndex = (i + j * width) * 4;
				m_Pixels[i + j * width] = Color(pixels[colorIndex + 0], pixels[colorIndex + 1], pixels[colorIndex + 2], pixels[colorIndex + 3]);
			}
	}

	Image::Image(const Vector2u &size, const Color &background) : Image(size.Width, size.Height, background) {}
	Image::Image(const Vector2u &size, const Color *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const uint8_t *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const glm::vec4 *pixels) : Image(size.Width, size.Height, pixels) {}
	Image::Image(const Vector2u &size, const float *pixels) : Image(size.Width, size.Height, pixels) {}

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
		Clear();
	}

	void Image::Create(uint32_t width, uint32_t height, const Color &background)
	{
		Clear();

		m_Width  = width;
		m_Height = height;

		const size_t size = static_cast<size_t>(width) * height;

		m_Pixels = new Color[size];

		std::fill_n(m_Pixels, size, background);
	}

	void Image::Clear()
	{
		delete[] m_Pixels;

		m_Pixels = nullptr;
		m_Width  = m_Height = 0;
	}

	void Image::Load(const uint8_t *pixels, size_t size)
	{
		Clear();

		const auto format = FreeImage_GetFileTypeFromMemory(reinterpret_cast<FIMEMORY*>(const_cast<uint8_t*>(pixels)), static_cast<int>(size));
		if(format == FIF_UNKNOWN)
		{
			throw std::runtime_error("Unknown image format");
		}

		const auto image = FreeImage_LoadFromMemory(format, reinterpret_cast<FIMEMORY*>(const_cast<uint8_t*>(pixels)), static_cast<int>(size));
		if(!image)
		{
			throw std::runtime_error("Unable to open memory file");
		}

		LoadToMemory(image);
		FreeImage_Unload(image);
	}

	void Image::Load(const std::string &fileName)
	{
		Clear();

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
		auto handler = FreeImage_Allocate(m_Width, m_Height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

		for(size_t i = 0; i < m_Width; ++i)
			for(size_t j = 0; j < m_Height; ++j)
			{
				RGBQUAD color;

				const auto pixel  = m_Pixels[i + j * m_Width];
				color.rgbRed      = static_cast<BYTE>(pixel.R);
				color.rgbGreen    = static_cast<BYTE>(pixel.G);
				color.rgbBlue     = static_cast<BYTE>(pixel.B);
				color.rgbReserved = static_cast<BYTE>(pixel.A);

				FreeImage_SetPixelColor(handler, i, j, &color);
			}

		FreeImage_Save(ConvertType(type), handler, fileName.c_str(), 0);
		FreeImage_Unload(handler);
	}

	Color& Image::GetPixel(uint32_t x, uint32_t y)
	{
		ASSERT(x < m_Width && y < m_Height, "Out of range");
		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	const Color& Image::GetPixel(uint32_t x, uint32_t y) const
	{
		ASSERT(x < m_Width && y < m_Height, "Out of range");
		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	void Image::SetPixel(uint32_t x, uint32_t y, const Color &color)
	{
		GetPixel(x, y) = color;
	}

	Image& Image::operator=(Image &&image) noexcept
	{
		Clear();
		m_Width  = image.m_Width;
		m_Height = image.m_Height;

		m_Pixels       = image.m_Pixels;
		image.m_Pixels = nullptr;

		return *this;
	}

	Image& Image::operator=(const Image &image)
	{
		Clear();
		m_Width  = image.m_Width;
		m_Height = image.m_Height;

		const size_t size = static_cast<size_t>(m_Width) * m_Height;

		m_Pixels = new Color[size];

		std::memcpy(m_Pixels, image.m_Pixels, size * sizeof(Color));

		return *this;
	}

	void Image::LoadToMemory(void *imageFile)
	{
		auto image = static_cast<FIBITMAP*>(imageFile);
		image      = FreeImage_ConvertTo32Bits(image);

		m_Width  = FreeImage_GetWidth(image);
		m_Height = FreeImage_GetHeight(image);

		const size_t size = static_cast<size_t>(m_Width) * m_Height;

		auto pixels = FreeImage_GetBits(image);

		m_Pixels = new Color[size];

		for(size_t i = 0; i < size; ++i)
		{
			m_Pixels[i] = Color(pixels[i * 4 + FI_RGBA_RED], pixels[i * 4 + FI_RGBA_GREEN], pixels[i * 4 + FI_RGBA_BLUE], pixels[i * 4 + FI_RGBA_ALPHA]);
		}

		FreeImage_Unload(image);
	}
}
