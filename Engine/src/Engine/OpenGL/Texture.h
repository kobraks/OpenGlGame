#pragma once

#include "Engine/Core/Base.h"

#include "Engine/OpenGL/TextureObject.h"

namespace Game
{
	class Texture: public TextureObject
	{
		friend FrameBufferObject;

	public:
		using TextureObject::SetWrapping;
		using TextureObject::SetWrappingS;
		using TextureObject::SetWrappingT;
		using TextureObject::GetWrappingS;
		using TextureObject::GetWrappingT;
		using TextureObject::GenerateMipMaps;
		using TextureObject::HasGeneratedMipMaps;

		Texture();
		Texture(const Vector2u &size, uint32_t levels = -1);
		Texture(uint32_t width, uint32_t height, uint32_t levels = -1);
		explicit Texture(Image image, uint32_t levels = -1);

		void Create(uint32_t width, uint32_t height, uint32_t levels = -1);
		void Create(const Vector2u &size, uint32_t levels = -1);
		void Create(Image image, uint32_t levels = -1);

		void Resize(uint32_t width, uint32_t height);
		void Resize(const Vector2u &size);

		void Update(const uint8_t *pixels);
		void Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const Color *pixels);
		void Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const Color *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const TextureObject &texture);
		void Update(const TextureObject &texture, int32_t x, int32_t y);
		void Update(const TextureObject &texture, const Vector2i &offset);

		void Update(const Image &image);
		void Update(const Image &image, int32_t x, int32_t y);
		void Update(const Image &image, const Vector2i &offset);

		static uint64_t GetMaxSize();
		static float GetMaxLod();
	};
}
