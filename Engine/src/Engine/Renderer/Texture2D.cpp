#include "pch.h"
#include "Engine/Renderer/Texture2D.h"

#include "glad/glad.h"

namespace Engine {
	Texture2D::Texture2D() {
		m_Internals = MakeRef<Internals>();
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height) : Texture2D(){
		Create(Vector2u(width, height));
	}

	Texture2D::Texture2D(const Vector2u& size) : Texture2D() {
		Create(size);
	}

	Texture2D::Texture2D(const Image& image) : Texture2D() {
		Create(image);
	}

	void Texture2D::GenerateMipMaps() const {
		m_Internals->MimpmapGenerated = true;

		glGenerateMipmap(m_Internals->ID);
	}

	void Texture2D::Bind() {
		m_Internals->Bind();
	}

	void Texture2D::Create(const Vector2u& size) {
		if (CheckTextureSize(size))
			CreateTexture(size);
	}

	void Texture2D::Create(const Image& image) {
		if (CheckTextureSize(image.Size()))
			CreateTexture(image.Size(), image.GetPixels().data());
	}

	Image Texture2D::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		glGetTextureImage(m_Internals->ID, 0, GL_RGBA, GL_UNSIGNED_INT, static_cast<GLsizei>(size), &pixels[0]);

		return Image(m_Internals->Size, pixels.data());
	}

	void Texture2D::Update(const uint8_t* pixels) {
		Update(pixels, m_Internals->Size, {0, 0});
	}

	void Texture2D::Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset) {
		Update(static_cast<const void*>(pixels), size, offset);
	}

	void Texture2D::Update(const Color* pixels) {
		Update(pixels, m_Internals->Size, { 0, 0 });
	}

	void Texture2D::Update(const Color* pixels, const Vector2u& size, const Vector2i& offset) {
		Update(static_cast<const void*>(pixels), size, offset);
	}

	void Texture2D::Update(const Texture2D& texture) {
		Update(texture, { 0, 0 });
	}

	void Texture2D::Update(const Texture2D& texture, const Vector2i& offset) {
		Update(texture.ToImage(), offset);
	}

	void Texture2D::Update(const Image& image) {
		Update(image.GetPixels().data(), image.Size(), {0, 0});
	}

	void Texture2D::Update(const Image& image, const Vector2i& offset) {
		Update(image.GetPixels().data(), image.Size(), offset);
	}

	void Texture2D::Resize(const Vector2u& size) {
	}

	void Texture2D::Swap(Texture2D& to) {
	}

	Vector2u Texture2D::GetMaxSize() {
		static bool checked = false;
		static int32_t size = 0;

		if (!checked) {
			checked = true;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		}

		return { static_cast<uint32_t>(size), static_cast<uint32_t>(size) };
	}

	uint32_t Texture2D::GetMaxDim() {
		return GetMaxSize().Width;
	}

	bool Texture2D::CheckTextureSize(const Vector2u& size) {
		ENGINE_ASSERT(size.Width != 0 && size.Height != 0)

			if (size.Width == 0 || size.Height == 0)
				return false;

		const uint32_t maxSize = GetMaxDim();

		if (size.Width > maxSize || size.Height > maxSize) {
			ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);
			LOG_GL_ERROR("Unable to create texture with size of {}, texture is to big max texture size is {}", size, GetMaxSize());
			return false;
		}

		return true;
	}

	void Texture2D::CreateTexture(const Vector2u& size, const void* pixels) {
		m_Internals->Image2D(pixels, GL_RGBA8, size, GL_RGBA, GL_UNSIGNED_BYTE);
	}

	void Texture2D::Update(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(((offset.X + size.Width) < m_Internals->Size.Width) && ((offset.Y + size.Height) < m_Internals->Size.Height));
		if (((offset.X + size.Width) < m_Internals->Size.Width) && ((offset.Y + size.Height) < m_Internals->Size.Height)) {
			m_Internals->SubImage2D(pixels, offset, size, GL_RGBA, GL_UNSIGNED_BYTE);
		}
		else
			throw std::out_of_range("Out of texture bounds");
	}

	Texture2D::Internals::Internals() {
		glGenTextures(1, &ID);
	}

	Texture2D::Internals::~Internals() {
		glDeleteTextures(1, &ID);
	}

	void Texture2D::Internals::Bind() {
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Texture2D::Internals::Image2D(const void* pixels, int internalFormat, const Vector2u& size, uint32_t format, uint32_t type) {
		Size = size;

		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 0, format, type, pixels);
	}

	void Texture2D::Internals::SubImage2D(const void* pixels, const Vector2i& offset, const Vector2u& size,
		uint32_t format, uint32_t type) {

		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		ENGINE_ASSERT(((offset.X + size.Width) < Size.Width) && ((offset.Y + size.Height) < Size.Height));
		if (((offset.X + size.Width) < Size.Width) && ((offset.Y + size.Height) < Size.Height))
			throw std::out_of_range("SubImage2D out of range");

		glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y), format, type, pixels);
	}

	void Texture2D::Internals::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(ID, name, parameter);
	}
}
