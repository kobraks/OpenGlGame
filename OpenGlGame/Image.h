#pragma once

#include "Types.h"

namespace Game
{
	enum class ImageType
	{
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

	class Image
	{
		glm::vec4 *m_Pixels = nullptr;
		uint32_t m_Width    = 0;
		uint32_t m_Height   = 0;

	public:
		Image() = default;

		Image(uint32_t width, uint32_t height, const glm::vec4 &background = glm::vec4(1));
		Image(uint32_t width, uint32_t height, const uint8_t *pixels);
		Image(uint32_t width, uint32_t height, const glm::vec4 *pixels);

		Image(const Vector2u &size, const glm::vec4 &background = glm::vec4(1));
		Image(const Vector2u &size, const uint8_t *pixels);
		Image(const Vector2u &size, const glm::vec4 *pixels);

		Image(const uint8_t *pixels, uint32_t size);

		explicit Image(const std::string &fileName);

		Image(const Image &image);
		Image(Image &&image) noexcept;

		~Image();

		void Create(const Vector2u &size, const glm::vec4 &background = glm::vec4(1))
		{
			Create(size.X, size.Y, background);
		};

		void Create(uint32_t width, uint32_t height, const glm::vec4 &background = glm::vec4(1));

		void Load(const uint8_t *pixels, uint32_t size);
		void Load(const std::string &fileName);

		void Save(const std::string &fileName, ImageType type);

		unsigned int Width() const { return m_Width; }
		unsigned int Height() const { return m_Height; }

		Vector2u Size() const { return Vector2u(m_Width, m_Height); }

		const glm::vec4* GetPixels() const { return m_Pixels; }

		glm::vec4& GetPixel(const Vector2u &position) { return GetPixel(position.X, position.Y); }
		glm::vec4 GetPixel(const Vector2u &position) const { return GetPixel(position.X, position.Y); }

		glm::vec4& GetPixel(uint32_t x, uint32_t y);
		glm::vec4 GetPixel(uint32_t x, uint32_t y) const;

		void SetPixel(const Vector2u &position, const glm::vec4 &color)
		{
			return SetPixel(position.X, position.Y, color);
		}

		void SetPixel(uint32_t x, uint32_t y, const glm::vec4 &color);

		static glm::vec4 TranslateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		{
			return glm::vec4(r / 255, g / 255, b / 255, a / 255);
		};

		static glm::vec4 TranslateColor(const uint8_t color[4])
		{
			return TranslateColor(color[0], color[1], color[2], color[3]);
		};

		Image& operator=(Image &&image) noexcept;
		Image& operator=(const Image &image);

		glm::vec4 operator()(uint32_t x, uint32_t y) const { return GetPixel(x, y); }
		glm::vec4& operator()(uint32_t x, uint32_t y) { return GetPixel(x, y); }
	private:
		void LoadToMemory(void *imageFile);
	};
}
