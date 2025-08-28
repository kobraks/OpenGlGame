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

	namespace Utils {
		const char* ToString(ImageType type);
	}

	class Image {
	public:
		Image(const Image& img) noexcept;
		Image(Image&& img) noexcept;

		~Image() = default;

		[[nodiscard]] static Ref<Image> Create(uint32_t width, uint32_t height, const Color& background = Color::White);
		[[nodiscard]] static Ref<Image> Create(uint32_t width, uint32_t height, const Color* pixels);
		[[nodiscard]] static Ref<Image> Create(uint32_t width, uint32_t height, const uint8_t* pixels);
		[[nodiscard]] static Ref<Image> Create(uint32_t width, uint32_t height, const glm::vec4* pixels);
		[[nodiscard]] static Ref<Image> Create(uint32_t width, uint32_t height, const float* pixels);

		[[nodiscard]] static Ref<Image> Create(const Vector2u& size, const Color& background = Color::White);
		[[nodiscard]] static Ref<Image> Create(const Vector2u& size, const Color* pixels);
		[[nodiscard]] static Ref<Image> Create(const Vector2u& size, const uint8_t* pixels);
		[[nodiscard]] static Ref<Image> Create(const Vector2u& size, const glm::vec4* pixels);
		[[nodiscard]] static Ref<Image> Create(const Vector2u& size, const float* pixels);

		[[nodiscard]] static Ref<Image> Load(const Buffer& buffer);
		[[nodiscard]] static Ref<Image> Load(const std::filesystem::path& path);

		static bool Save(const Ref<Image>& image, const std::filesystem::path& path, ImageType type);

		void Clear();

		void Copy(const Ref<Image>& image);

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }
		Vector2u Size() const { return {m_Width, m_Height}; }

		void FlipVertical();

		void Resize(const Vector2u& size);
		void Resize(uint32_t newWidth, uint32_t newHeight);

		void Crop(const Vector2u& start, const Vector2u& size);
		void Crop(uint32_t startX, uint32_t startY, uint32_t width, uint32_t height);

		Color GetAverageColor() const;

		void Fill(const Color& color);

		const auto& GetPixels() const { return m_Pixels; }

		Color& GetPixel(const Vector2u& pos) { return GetPixel(pos.X, pos.Y); }
		const Color& GetPixel(const Vector2u& pos) const { return GetPixel(pos.X, pos.Y); }

		Color& GetPixel(uint32_t x, uint32_t y);
		const Color& GetPixel(uint32_t x, uint32_t y) const;

		void SetPixel(const Vector2u& pos, const Color& color) { SetPixel(pos.X, pos.Y, color); }
		void SetPixel(uint32_t x, uint32_t y, const Color& color);

		[[nodiscard]] std::unique_ptr<uint8_t[]> CopyAsRGBA8() const;

		std::string DebugInfo() const;

		Image& operator=(Image&& img) noexcept;
		Image& operator=(const Image& img) noexcept;

	protected:
		Image() = default;

		void Reset(uint32_t width, uint32_t height);
	private:
		void LoadToMemory(void* buffer);
		void Allocate(uint32_t width, uint32_t height);

		static void FillFreeImagePixels(void* handler, const std::vector<Color>& pixels, uint32_t width, uint32_t height);

		void CopyFromRawBuffer(const Color* src);
		void CopyFromRawBuffer(const uint8_t* src);
		void CopyFromRawBuffer(const float* src);
		void CopyFromRawBuffer(const glm::vec4* src);

		std::vector<Color> m_Pixels;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}


template<>
struct fmt::formatter<Engine::ImageType> : fmt::formatter<const char*> {
	auto format(Engine::ImageType type, format_context& ctx) const {
		return fmt::formatter<const char*>::format(Engine::Utils::ToString(type), ctx);
	}
};