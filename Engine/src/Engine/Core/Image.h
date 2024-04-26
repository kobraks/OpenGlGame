#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/Vector2.h"

#include <filesystem>

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

		Image(const Image &img);
		Image(Image &&img);

		~Image();

		void Create(uint32_t width, uint32_t height, const Color &background = Color::White);
		void Create(const Vector2u &size, const Color &background = Color::White);

		void Clear();

		void Load(uint8_t *buffer, size_t size);
		void Load(const std::filesystem::path &path);

		void Save(const std::filesystem::path &path, ImageType type);

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }
		Vector2u Size() const { return {m_Width, m_Height }; }

		const Color *GetPixels() const { return m_Pixels; }
		Color& GetPixel(const Vector2u &pos) { return GetPixel(pos.X, pos.Y); }
		const Color &GetPixel(const Vector2u &pos) const { return GetPixel(pos.X, pos.Y); }

		Color& GetPixel(uint32_t x, uint32_t y);
		const Color &GetPixel(uint32_t x, uint32_t y) const;

		void SetPixel(const Vector2u &pos, const Color &color) { SetPixel(pos.X, pos.Y, color); }
		void SetPixel(uint32_t x, uint32_t y, const Color &color);

		Image& operator=(Image&& img) noexcept;
		Image& operator=(const Image &img);

	private:
		void LoadToMemory(void *ImageFile);
		void Prepare(uint32_t width, uint32_t height);

	private:
		Color *m_Pixels = nullptr;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		size_t m_Size = 0;
	};
}