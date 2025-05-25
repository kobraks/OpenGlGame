#include "pch.h"
#include "Engine/Core/Image.h"

#include "Engine/Core/Assert.h"

#include <algorithm>
#include <FreeImage.h>

namespace Engine {
	namespace Utils {
		static constexpr FREE_IMAGE_FORMAT ConvertType(const ImageType type) {
			switch (type) {
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

		static constexpr Color GetColorFromFreeImage(BYTE* pixels, size_t i) {
			Color color;

			color.R = pixels[i * 4 + FI_RGBA_RED];
			color.G = pixels[i * 4 + FI_RGBA_GREEN];
			color.B = pixels[i * 4 + FI_RGBA_BLUE];
			color.A = pixels[i * 4 + FI_RGBA_ALPHA];

			return color;
		}

		const char* ToString(ImageType type) {
			switch (type) {
			case ImageType::BMP:     return "BMP";
			case ImageType::EXR:     return "EXR";
			case ImageType::J2K:     return "J2K";
			case ImageType::JP2:     return "JP2";
			case ImageType::JPEG:    return "JPEG";
			case ImageType::JXR:     return "JXR";
			case ImageType::PNG:     return "PNG";
			case ImageType::PBM:     return "PBM";
			case ImageType::PGM:     return "PGM";
			case ImageType::PPM:     return "PPM";
			case ImageType::TIFF:    return "TIFF";
			default:                 return "Unknown";
			}
		}
	}

	Image::Image(const Image& img) noexcept {
		LOG_ENGINE_DEBUG("Image copied: {}x{}", img.m_Width, img.m_Height);
		m_Pixels = img.m_Pixels;
		m_Width = img.m_Width;
		m_Height = img.m_Height;
	}

	Image::Image(Image&& img) noexcept {
		LOG_ENGINE_DEBUG("Image moved: {}x{}", img.m_Width, img.m_Height);
		m_Pixels = std::move(img.m_Pixels);
		m_Width = std::exchange(img.m_Width, 0);
		m_Height = std::exchange(img.m_Height, 0);
	}

	Ref<Image> Image::Create(uint32_t width, uint32_t height, const Color& background) {
		Ref<Image> image = Ref<Image>(new Image());

		image->Prepare(width, height);
		image->Fill(background);

		LOG_ENGINE_DEBUG("Creating Image - size: {}x{}, filled with color: {}", width, height, background);

		return image;
	}

	Ref<Image> Image::Create(uint32_t width, uint32_t height, const Color* pixels) {
		ENGINE_ASSERT(pixels);
		Ref<Image> image = Ref<Image>(new Image());

		image->Prepare(width, height);
		image->CopyFromRawBuffer(pixels);

		LOG_ENGINE_DEBUG("Creating Image - size: {}x{}, from Color buffer", width, height);

		return image;
	}

	Ref<Image> Image::Create(uint32_t width, uint32_t height, const uint8_t* pixels) {
		ENGINE_ASSERT(pixels);
		Ref<Image> image = Ref<Image>(new Image());

		image->Prepare(width, height);
		image->CopyFromRawBuffer(pixels);

		LOG_ENGINE_DEBUG("Creating Image - size: {}x{}, from uint8_t buffer", width, height);

		return image;
	}

	Ref<Image> Image::Create(uint32_t width, uint32_t height, const glm::vec4* pixels) {
		ENGINE_ASSERT(pixels);
		Ref<Image> image = Ref<Image>(new Image());

		image->Prepare(width, height);
		image->CopyFromRawBuffer(pixels);

		LOG_ENGINE_DEBUG("Creating Image - size: {}x{}, from glm::vec4 buffer", width, height);

		return image;
	}

	Ref<Image> Image::Create(uint32_t width, uint32_t height, const float* pixels) {
		ENGINE_ASSERT(pixels);
		Ref<Image> image = Ref<Image>(new Image());

		image->Prepare(width, height);
		image->CopyFromRawBuffer(pixels);

		LOG_ENGINE_DEBUG("Creating Image - size: {}x{}, from float buffer", width, height);

		return image;
	}

	Ref<Image> Image::Create(const Vector2u& size, const Color& background) {
		return Create(size.Width, size.Height, background);
	}

	Ref<Image> Image::Create(const Vector2u& size, const Color* pixels) {
		return Create(size.Width, size.Height, pixels);
	}

	Ref<Image> Image::Create(const Vector2u& size, const uint8_t* pixels) {
		return Create(size.Width, size.Height, pixels);
	}

	Ref<Image> Image::Create(const Vector2u& size, const glm::vec4* pixels) {
		return Create(size.Width, size.Height, pixels);
	}

	Ref<Image> Image::Create(const Vector2u& size, const float* pixels) {
		return Create(size.Width, size.Height, pixels);
	}

	void Image::Clear() {
		m_Width = m_Height = 0;
		m_Pixels.clear();
	}

	void Image::Copy(const Ref<Image>& image) {
		Reset(image->m_Width, image->m_Height);

		LOG_ENGINE_DEBUG("Image copied: {}x{}", image->m_Width, image->m_Height);
		Reset(image->m_Width, image->m_Height);

		std::ranges::copy(image->m_Pixels, std::begin(m_Pixels));
	}

	Ref<Image> Image::Load(const Buffer& buffer) {
		Ref<Image> result = Ref<Image>(new Image());

		ENGINE_ASSERT(buffer);
		if (!buffer)
			throw std::runtime_error("Uninitialized buffer");

		auto stream = FreeImage_OpenMemory(const_cast<BYTE*>(buffer.As<BYTE>()), static_cast<DWORD>(buffer.Size()));
		const auto format = FreeImage_GetFileTypeFromMemory(stream, 0);

		ENGINE_ASSERT(format != FIF_UNKNOWN);
		if (format == FIF_UNKNOWN) {
			FreeImage_CloseMemory(stream);
			throw std::runtime_error("Unknown image format");
		}

		const auto image = FreeImage_LoadFromMemory(format, stream, 0);

		ENGINE_ASSERT(image);
		if (!image) {
			FreeImage_CloseMemory(stream);
			throw std::runtime_error("Unable to load image from memory");
		}

		result->LoadToMemory(image);
		FreeImage_Unload(image);
		FreeImage_CloseMemory(stream);

		LOG_ENGINE_INFO("Loaded image from buffer: {}", result->DebugInfo());

		return result;
	}

	Ref<Image> Image::Load(const std::filesystem::path &path) {
		Ref<Image> result = Ref<Image>(new Image());

		const std::string sPath = path.string();

		ENGINE_ASSERT(std::filesystem::exists(path));
		ENGINE_ASSERT(std::filesystem::is_regular_file(path));

		if (!std::filesystem::is_regular_file(path))
			throw std::runtime_error(fmt::format("'{}' is not regular file", sPath));

		if (!std::filesystem::exists(path))
			throw std::runtime_error(fmt::format("'{}' does not exists", sPath));

		const auto format = FreeImage_GetFileType(sPath.c_str());

		ENGINE_ASSERT(format != FIF_UNKNOWN);
		if(format == FIF_UNKNOWN)
			throw std::runtime_error(fmt::format("Unknown image format in '{}' file", sPath));

		const auto image = FreeImage_Load(format, sPath.c_str());

		ENGINE_ASSERT(image);
		if(!image)
			throw std::runtime_error(fmt::format("Unable to load '{}' file", sPath));

		result->LoadToMemory(image);
		FreeImage_Unload(image);

		LOG_ENGINE_INFO("Loaded image from file '{}': {}", sPath, result->DebugInfo());

		return result;
	}


	bool Image::Save(const Ref<Image>& image, const std::filesystem::path &path, ImageType type) {
		ENGINE_ASSERT(image);
		if (!image)
			throw std::runtime_error("Uninitialized memory");

		const auto sPath = path.string();

		const auto width = image->m_Width;
		const auto height = image->m_Height;
		const auto& pixels = image->m_Pixels;

		auto handler = FreeImage_Allocate(
		                                  static_cast<int>(width),
		                                  static_cast<int>(height),
		                                  32,
		                                  FI_RGBA_RED_MASK,
		                                  FI_RGBA_GREEN_MASK,
		                                  FI_RGBA_BLUE_MASK
		                                 );

		FillFreeImagePixels(handler, pixels, width, height);

		bool result = FreeImage_Save(Utils::ConvertType(type), handler, sPath.c_str(), 0);
		ENGINE_ASSERT(result);

		FreeImage_Unload(handler);

		LOG_ENGINE_INFO("Saved image to '{}' as format: {}", sPath, type);
		return result;
	}

	void Image::FlipVertical() {
		for (uint32_t y = 0; y < m_Height / 2; ++y) {
			for (uint32_t x = 0; x < m_Width; ++x) {
				std::swap(GetPixel(x, y), GetPixel(x, m_Height - 1 - y));
			}
		}

		LOG_ENGINE_DEBUG("Flipped image vertically: {}x{}", m_Width, m_Height);
	}

	void Image::Resize(const Vector2u& size) {
		Resize(size.Width, size.Height);
	}

	void Image::Resize(uint32_t newWidth, uint32_t newHeight) {
		if (newWidth == m_Width && newHeight == m_Height)
			return;

		LOG_ENGINE_DEBUG("Resized image from {}x{} to {}x{}", m_Width, m_Height, newWidth, newHeight);

		std::vector<Color> newPixels(newWidth * newHeight);

		for (uint32_t y = 0; y < newHeight; ++y) {
			for (uint32_t x = 0; x < newWidth; ++x) {
				const uint32_t srcX = x * m_Width / newWidth;
				const uint32_t srcY = y * m_Height / newHeight;
				newPixels[x + y * newWidth] = GetPixel(srcX, srcY);
			}
		}

		m_Width = newWidth;
		m_Height = newHeight;
		m_Pixels = std::move(newPixels);
	}

	void Image::Crop(const Vector2u& start, const Vector2u& size) {
		Crop(start.X, start.Y, size.Width, size.Height);
	}

	void Image::Crop(uint32_t startX, uint32_t startY, uint32_t width, uint32_t height) {
		ENGINE_ASSERT(startX + width < m_Width && startY + height < m_Height);
		if (startX + width >= m_Width || startY + height > m_Height)
			throw std::out_of_range("Crop parameters are out of range.");

		std::vector<Color> newPixels(width * height);

		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				newPixels[x + y * width] = GetPixel(startX + x, startY + y);
			}
		}

		m_Width = width;
		m_Height = height;
		m_Pixels = std::move(newPixels);

		LOG_ENGINE_DEBUG("Cropped image to {}x{} at ({}, {})", width, height, startX, startY);
	}

	Color Image::GetAverageColor() const {
		glm::vec4 total(0.f);
		for (const auto& pixel : m_Pixels) {
			total += pixel.ToFloat();
		}

		total /= static_cast<float>(m_Pixels.size());
		return Color(total);
	}

	void Image::Fill(const Color& color) {
		std::ranges::fill(m_Pixels, color);
	}

	Color& Image::GetPixel(uint32_t x, uint32_t y) {
		ENGINE_ASSERT(x < m_Width && y < m_Height);

		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	const Color& Image::GetPixel(uint32_t x, uint32_t y) const {
		ENGINE_ASSERT(x < m_Width && y < m_Height);

		if(x >= m_Width || y >= m_Height)
			throw std::out_of_range("Out of range");

		return m_Pixels[x + y * m_Width];
	}

	void Image::SetPixel(uint32_t x, uint32_t y, const Color &color) {
		GetPixel(x, y) = color;
	}

	std::unique_ptr<uint8_t[]> Image::CopyAsRGBA8() const {
		auto data = std::make_unique<uint8_t[]>(m_Pixels.size() * 4);

		for (size_t i = 0; i < m_Pixels.size(); ++i) {
			const Color& c = m_Pixels[i];

			data[i * 4 + 0] = c.R;
			data[i * 4 + 1] = c.G;
			data[i * 4 + 2] = c.B;
			data[i * 4 + 3] = c.A;
		}

		return data;
	}

	std::string Image::DebugInfo() const {
		return fmt::format("Image: {}x{}, pixels: {}", m_Width, m_Height, m_Pixels.size());
	}

	Image& Image::operator=(Image &&img) noexcept {
		LOG_ENGINE_DEBUG("Image moved: {}x{}", img.m_Width, img.m_Height);
		Clear();

		m_Width  = img.m_Width;
		m_Height = img.m_Height;

		m_Pixels = std::move(img.m_Pixels);

		img.m_Width = img.m_Height = 0;

		return *this;
	}

	Image& Image::operator=(const Image &img) noexcept {
		LOG_ENGINE_DEBUG("Image copied: {}x{}", img.m_Width, img.m_Height);
		Reset(img.m_Width, img.m_Height);


		std::ranges::copy(img.m_Pixels, std::begin(m_Pixels));
		return *this;
	}

	void Image::Reset(uint32_t width, uint32_t height) {
		Clear();
		Prepare(width, height);
	}

	void Image::LoadToMemory(void *buffer) {
		auto image = static_cast<FIBITMAP*>(buffer);
		auto converted = FreeImage_ConvertTo32Bits(image);

		if (!converted)
			throw std::runtime_error("Failed to convert image to 32 bit");

		const auto width = FreeImage_GetWidth(converted);
		const auto height = FreeImage_GetHeight(converted);

		Prepare(width, height);

		const auto pixels = FreeImage_GetBits(converted);
		for(size_t i = 0; i < m_Pixels.size(); ++i) {
			m_Pixels[i] = Utils::GetColorFromFreeImage(pixels, i);
		}

		FreeImage_Unload(converted);
	}

	void Image::Prepare(uint32_t width, uint32_t height) {
		const auto size = static_cast<size_t>(width) * static_cast<size_t>(height);
		m_Width  = width;
		m_Height = height;

		m_Pixels.resize(size);
	}

	void Image::FillFreeImagePixels(void* handler, const std::vector<Color>& pixels, uint32_t width, uint32_t height) {
		for (uint32_t i = 0; i < width; ++i)
			for (uint32_t j = 0; j < height; ++j) {
				RGBQUAD c;

				const auto pixel = pixels[i + j * width];

				c.rgbRed = static_cast<BYTE>(pixel.R);
				c.rgbGreen = static_cast<BYTE>(pixel.G);
				c.rgbBlue = static_cast<BYTE>(pixel.B);
				c.rgbReserved = static_cast<BYTE>(pixel.A);

				FreeImage_SetPixelColor(static_cast<FIBITMAP*>(handler), i, j, &c);
			}
	}

	void Image::CopyFromRawBuffer(const Color* src) {
		ENGINE_ASSERT(src);

		std::copy_n(src, m_Pixels.size(), std::begin(m_Pixels));
	}

	void Image::CopyFromRawBuffer(const uint8_t* src) {
		ENGINE_ASSERT(src);

		for (size_t i = 0; i < m_Pixels.size(); ++i) {
			const auto& r = src[i * 4 + 0];
			const auto& g = src[i * 4 + 1];
			const auto& b = src[i * 4 + 2];
			const auto& a = src[i * 4 + 3];

			m_Pixels[i] = Color(r, g, b, a);
		}
	}

	void Image::CopyFromRawBuffer(const float* src) {
		ENGINE_ASSERT(src);

		for (size_t i = 0; i < m_Pixels.size(); ++i) {
			const auto& r = src[i * 4 + 0];
			const auto& g = src[i * 4 + 1];
			const auto& b = src[i * 4 + 2];
			const auto& a = src[i * 4 + 3];

			m_Pixels[i] = Color(r, g, b, a);
		}
	}

	void Image::CopyFromRawBuffer(const glm::vec4* src) {
		ENGINE_ASSERT(src);

		std::copy_n(src, m_Pixels.size(), std::begin(m_Pixels));
	}
}
