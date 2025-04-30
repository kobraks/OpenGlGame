#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"
#include "Engine/Core/Buffer.h"

#include <filesystem>
#include <vector>

namespace Engine {
	enum class ImageType {
		BMP,
		EXR,
		J2K,
		JP2,
		JPEG,
		JXR,
		PNG,
		PBM,
		PGM,
		PPM,
		TIFF
	};

	class Image {
	public:
		Image() = default;

		Image(uint32_t width, uint32_t height, const Color &background = Color::White);
		Image(uint32_t width, uint32_t height, const Color *pixels);
		Image(uint32_t width, uint32_t height, const uint8_t *pixels);
		Image(uint32_t width, uint32_t height, const glm::vec4 *pixels);
		Image(uint32_t width, uint32_t height, const float *pixels);

		Image(const Vector2u &size, const Color &background = Color::White);
		Image(const Vector2u &size, const Color *pixels);
		Image(const Vector2u &size, const uint8_t *pixels);
		Image(const Vector2u &size, const glm::vec4 *pixels);
		Image(const Vector2u &size, const float *pixels);

		Image(const Image &img) noexcept;
		Image(Image &&img) noexcept;

		~Image() = default;

		void Create(uint32_t width, uint32_t height, const Color &background = Color::White);
		void Create(const Vector2u &size, const Color &background = Color::White);

		void Clear();

		static Ref<Image> Load(const Buffer& buffer);
		static Ref<Image> Load(const std::filesystem::path &path);

		static bool Save(Ref<Image> image, const std::filesystem::path &path, ImageType type);

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }
		Vector2u Size() const { return {m_Width, m_Height }; }

		void FlipVertical();

		void Resize(const Vector2u& size);
		void Resize(uint32_t newWidth, uint32_t newHeight);

		void Crop(const Vector2u& start, const Vector2u& size);
		void Crop(uint32_t startX, uint32_t startY, uint32_t width, uint32_t height);

		Color GetAverageColor() const;

		void Fill(const Color& color);

		const auto &GetPixels() const { return m_Pixels; }

		Color& GetPixel(const Vector2u &pos) { return GetPixel(pos.X, pos.Y); }
		const Color &GetPixel(const Vector2u &pos) const { return GetPixel(pos.X, pos.Y); }

		Color& GetPixel(uint32_t x, uint32_t y);
		const Color &GetPixel(uint32_t x, uint32_t y) const;

		void SetPixel(const Vector2u &pos, const Color &color) { SetPixel(pos.X, pos.Y, color); }
		void SetPixel(uint32_t x, uint32_t y, const Color &color);

		std::unique_ptr<uint8_t[]> CopyAsRGBA8() const;

		Image& operator=(Image&& img) noexcept;
		Image& operator=(const Image &img) noexcept;

	private:
		void LoadToMemory(void *buffer);
		void Prepare(uint32_t width, uint32_t height);

	private:
		std::vector<Color> m_Pixels;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}