#pragma once
#include <string>

#include "Types.h"
#include "Color.h"
#include "Vector2.h"

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
		Color *m_Pixels   = nullptr;
		uint32_t m_Width  = 0;
		uint32_t m_Height = 0;

	
	public:
		Image() = default;

		Image(uint32_t width, uint32_t height, const Color& background = Color::White);
		Image(uint32_t width, uint32_t height, const Color *pixels);
		Image(uint32_t width, uint32_t height, const uint8_t *pixels);
		Image(uint32_t width, uint32_t height, const glm::vec4 *pixels);
		Image(uint32_t width, uint32_t height, const float *pixels);

		Image(const Vector2u& size, const Color& background = Color::White);
		Image(const Vector2u& size, const Color *pixels);
		Image(const Vector2u& size, const uint8_t *pixels);
		Image(const Vector2u& size, const glm::vec4 *pixels);
		Image(const Vector2u& size, const float *pixels);

		Image(const Image& image);
		Image(Image&& image) noexcept;

		~Image();

		void Create(uint32_t width, uint32_t height, const Color& background = Color::White);
		
		void Create(const Vector2u& size, const Color& background = Color::White)
		{
			return Create(size.Width, size.Height, background);
		}

		void Clear();

		void Load(const uint8_t *pixels, size_t size);
		void Load(const std::string& fileName);
		
		void Save(const std::string& fileName, ImageType type);

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }
		Vector2u Size() const { return Vector2u(m_Width, m_Height); }

		const Color* GetPixels() const { return m_Pixels; }

		Color& GetPixel(const Vector2u& position) { return GetPixel(position.X, position.Y); } 
		const Color& GetPixel(const Vector2u& position) const { return GetPixel(position.X, position.Y); } 

		Color& GetPixel(uint32_t x, uint32_t y);
		const Color& GetPixel(uint32_t x, uint32_t y) const;

		void SetPixel(const Vector2u &position, const Color& color)
		{
			return SetPixel(position.X, position.Y, color);
		}

		void SetPixel(uint32_t x, uint32_t y, const Color& color);

		Image& operator=(Image&& image) noexcept;
		Image& operator=(const Image& image);

		Color& operator()(uint32_t x, uint32_t y) { return GetPixel(x, y); }
		const Color& operator()(uint32_t x, uint32_t y) const { return GetPixel(x, y); }

		Color* begin() { return m_Pixels; }
		Color* end() { return m_Pixels + static_cast<size_t>(m_Width) * m_Height; }

		const Color* begin() const { return m_Pixels; }
		const Color* end() const { return m_Pixels + static_cast<size_t>(m_Width) * m_Height; }

	private:
		void LoadToMemory(void *imageFile);
	};
}
