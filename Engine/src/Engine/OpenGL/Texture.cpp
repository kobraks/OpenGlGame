#include "pch.h"
#include "Engine/OpenGL/Texture.h"

#include "Engine/OpenGL/CubeMap.h"


namespace Game
{
	Texture::Texture() : TextureObject(TextureTarget::Texture2D) {}
	Texture::Texture(const Vector2u &size, uint32_t levels) : Texture()
	{
		Create(size, levels);
	}

	Texture::Texture(uint32_t width, uint32_t height, uint32_t levels) : Texture()
	{
		Create(Vector2u{width, height}, levels);
	}
	Texture::Texture(Image image, uint32_t levels) : Texture()
	{
		Create(image, levels);
	}

	void Texture::Create(uint32_t width, uint32_t height, uint32_t levels)
	{
		Create(Vector2u{width, height}, levels);
	}

	void Texture::Create(const Vector2u &size, uint32_t levels)
	{
		ASSERT(size.Width < GetMaxSize() && size.Height < GetMaxSize());

		TextureObject::Create(size, static_cast<int32_t>(levels) < 0 ? CalculateLevels(size) : levels);
	}

	void Texture::Create(Image image, uint32_t levels)
	{
		ASSERT(image.Width() < GetMaxSize() && image.Height() < GetMaxSize());
		TextureObject::Create(image, static_cast<int32_t>(levels) < 0 ? CalculateLevels(image.Size()) : levels);
	}

	void Texture::Resize(uint32_t width, uint32_t height)
	{
		Create(Vector2u{width, height}, Levels());
	}

	void Texture::Resize(const Vector2u &size)
	{
		Create(size, Levels());
	}

	void Texture::Update(const uint8_t *pixels)
	{
		TextureObject::Update(pixels, 0);
	}

	void Texture::Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		TextureObject::Update(pixels, 0, width, height, x, y);
	}

	void Texture::Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset)
	{
		TextureObject::Update(pixels, 0, size, offset);
	}

	void Texture::Update(const Color *pixels)
	{
		TextureObject::Update(pixels, 0);
	}

	void Texture::Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y)
	{
		TextureObject::Update(pixels, 0, width, height, x, y);
	}

	void Texture::Update(const Color *pixels, const Vector2u &size, const Vector2i &offset)
	{
		TextureObject::Update(pixels, 0, size, offset);
	}

	void Texture::Update(const TextureObject &texture)
	{
		TextureObject::Update(texture, 0);
	}

	void Texture::Update(const TextureObject &texture, int32_t x, int32_t y)
	{
		TextureObject::Update(texture, 0, x, y);
	}

	void Texture::Update(const TextureObject &texture, const Vector2i &offset)
	{
		TextureObject::Update(texture, 0, offset);
	}

	void Texture::Update(const Image &image)
	{
		TextureObject::Update(image, 0);
	}

	void Texture::Update(const Image &image, int32_t x, int32_t y)
	{
		TextureObject::Update(image, 0, x, y);
	}
	void Texture::Update(const Image &image, const Vector2i &offset)
	{
		TextureObject::Update(image, 0, offset);
	}

	uint32_t Texture::GetMaxSize()
	{
		static bool checked = false;
		static uint32_t size = 0;

		if (checked)
			return size;

		ASSERT(Context::GetContext(), "No active openGL context");

		if (!Context::GetContext())
			throw std::runtime_error("No active openGL context");

		size = Context::GetContext()->GetFunctions().GetInteger(GL_MAX_TEXTURE_SIZE);
		checked = true;

		return size;
	}
}
