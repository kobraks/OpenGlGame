#include "pch.h"
#include "Engine/Renderer/Texture2D.h"

#include "glad/glad.h"

namespace Engine {
	int GetOpenWrapping(Wrapping wrapping) {
		switch (wrapping) {
		case Wrapping::Repeat:
			return GL_REPEAT;
		case Wrapping::MirroredRepeat:
			return GL_MIRRORED_REPEAT;
		case Wrapping::ClampEdge:
			return GL_CLAMP_TO_EDGE;
		case Wrapping::ClampBorder:
			return GL_CLAMP_TO_BORDER;
		}
	}

	int GetOpenFilter(Filter filter) {
		switch (filter) {
		case Filter::Nearest:
			return GL_NEAREST;
		case Filter::Linear:
			return GL_LINEAR;
		case Filter::NearestMipmapNearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case Filter::LinearMipmapNearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case Filter::NearestMipmapLinear:
			return GL_NEAREST_MIPMAP_LINEAR;
		case Filter::LinearMipmapLinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		}
	}

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
		m_Internals->MipMapGenerated = true;

		glGenerateTextureMipmap(m_Internals->ID);
	}

	void Texture2D::Bind() const {
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

	void Texture2D::SetWrapping(Wrapping s) {
		SetWrappingS(s);
	}

	void Texture2D::SetWrapping(Wrapping s, Wrapping t) {
		SetWrappingS(s);
		SetWrappingS(t);
	}

	void Texture2D::SetWrapping(Wrapping s, Wrapping t, Wrapping r) {
		SetWrappingS(s);
		SetWrappingT(t);
		SetWrappingR(r);
	}

	void Texture2D::SetWrappingS(Wrapping wrapping) {
		m_Internals->Wrapping.S = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_S, GetOpenWrapping(wrapping));
	}

	void Texture2D::SetWrappingT(Wrapping wrapping) {
		m_Internals->Wrapping.T = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_T, GetOpenWrapping(wrapping));
	}

	void Texture2D::SetWrappingR(Wrapping wrapping) {
		m_Internals->Wrapping.R = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_R, GetOpenWrapping(wrapping));
	}

	void Texture2D::SetFilters(Filter min, Filter mag) {
		SetMinFilter(min);
		SetMagFilter(mag);
	}

	void Texture2D::SetMinFilter(Filter filter) {
		m_Internals->Filter.Min = filter;
		m_Internals->SetParameter(GL_TEXTURE_MIN_FILTER, GetOpenFilter(filter));
	}

	void Texture2D::SetMagFilter(Filter filter) {
		if (filter > Filter::Linear) {
			LOG_ENGINE_ERROR("Only possible values for mag filter is Nearest or Linear");
			return;
		}

		m_Internals->Filter.Mag = filter;
		m_Internals->SetParameter(GL_TEXTURE_MAG_FILTER, GetOpenFilter(filter));
	}

	Image Texture2D::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		m_Internals->GetImage(pixels.data(), static_cast<uint32_t>(size));
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
		if (CheckTextureSize(size)) {
			const auto image = ToImage();
			CreateTexture(size);

			Update(image);
			if (IsMipMapsGenerated())
				GenerateMipMaps();
		}
	}

	void Texture2D::Swap(Texture2D& to) {
		std::swap(m_Internals, to.m_Internals);
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
		ENGINE_ASSERT(size.Width != 0 && size.Height != 0);

		if (size.Width == 0 || size.Height == 0)
			throw std::runtime_error("Texture dimensions cannot be 0");

		const uint32_t maxSize = GetMaxDim();

		if (size.Width > maxSize || size.Height > maxSize) {
			ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);
			throw std::runtime_error(fmt::format("Unable to create texture width {}, texture is too big. Maximum texture size is {}", size, GetMaxSize()));
		}

		return true;
	}

	void Texture2D::CreateTexture(const Vector2u& size, const void* pixels) {
		m_Internals->Image2D(pixels, size);

		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture2D::Update(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(((offset.X + size.Width) < m_Internals->Size.Width) && ((offset.Y + size.Height) < m_Internals->Size.Height));
		if (((offset.X + size.Width) < m_Internals->Size.Width) && ((offset.Y + size.Height) < m_Internals->Size.Height)) {
			m_Internals->SubImage2D(pixels, size, offset);
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

	void Texture2D::Internals::Image2D(const void* pixels, const Vector2u& size) {
		Size = size;

		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::Internals::SubImage2D(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		ENGINE_ASSERT(((offset.X + size.Width) < Size.Width) && ((offset.Y + size.Height) < Size.Height));
		if (((offset.X + size.Width) < Size.Width) && ((offset.Y + size.Height) < Size.Height))
			glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		else
			throw std::out_of_range("SubImage2D out of range");
	}

	void Texture2D::Internals::GetImage(void* pixels, uint32_t size) {
		glGetTextureImage(ID, 0, GL_RGBA8, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size), pixels);
	}

	void Texture2D::Internals::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(ID, name, parameter);
	}
}
