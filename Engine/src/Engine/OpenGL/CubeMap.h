#pragma once
#include "Engine/Core/Base.h"
#include "Engine/OpenGl/TextureObject.h"

namespace Game
{
	class Texture;
	class CubeTexture;

	class CubeMap : public TextureObject
	{
		friend CubeTexture;

	public:
		enum class Orientation : uint32_t
		{
			Right = 0,
			Left,
			Top,
			Bottom,
			Back,
			Front,
			Count
		};

	private:
		std::array<CubeTexture*, static_cast<size_t>(Orientation::Count)> m_Textures = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

		CubeMap();
	public:
		using TextureObject::GetWrappingS;
		using TextureObject::GetWrappingT;
		using TextureObject::GetWrappingR;

		using TextureObject::SetWrappingS;
		using TextureObject::SetWrappingT;
		using TextureObject::SetWrappingR;
		using TextureObject::SetWrapping;

		CubeMap(uint32_t width, uint32_t height);
		explicit CubeMap(const Vector2u& size);

		CubeMap(const CubeMap& map);
		CubeMap(CubeMap&& map) noexcept;

		~CubeMap();

		CubeMap& operator=(const CubeMap& map);
		CubeMap& operator=(CubeMap&& map) noexcept;

		CubeTexture& GetTexture(Orientation orientation);
		const CubeTexture& GetTexture(Orientation orientation) const;

		static uint64_t GetMaxSize();
	protected:
		using TextureObject::Update;
		using TextureObject::ToImage;
		using TextureObject::SetParameter;

		void Create(const Vector2u& size);
	};

	class CubeTexture
	{
		friend CubeMap;

		const CubeMap::Orientation m_Face;
		mutable const CubeMap *m_Owner;

		Vector2u m_Size;

	protected:
		CubeTexture(CubeMap::Orientation orientation);

		void SetSize(const Vector2u &size);
		CubeTexture& SetOwner(const CubeMap &owner) const;

	public:
		CubeTexture() = delete;

		CubeMap& Owner() const { return *const_cast<CubeMap *>(m_Owner); }

		Vector2u Size() const { return m_Size; }
		uint32_t Width() const { return m_Size.Width; }
		uint32_t Height() const { return m_Size.Height; }

		Image ToImage() const;

		void Update(const uint8_t *pixels);
		void Update(const uint8_t *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const uint8_t *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const Color *pixels);
		void Update(const Color *pixels, uint32_t width, uint32_t height, int32_t x, int32_t y);
		void Update(const Color *pixels, const Vector2u &size, const Vector2i &offset);

		void Update(const TextureObject& texture);
		void Update(const TextureObject& texture, int32_t x, int32_t y);
		void Update(const TextureObject& texture, const Vector2i &offset);

		void Update(const Image& image);
		void Update(const Image& image, int32_t x, int32_t y);
		void Update(const Image& image, const Vector2i &offset);
	};
}