#include "pch.h"
#include "Engine/Renderer/Texture2D.h"
#include "Engine/Core/Image.h"

#include "glad/glad.h"

namespace Engine {
	constexpr static int GetOpenWrapping(Wrapping wrapping) {
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

	constexpr static int GetOpenFilter(Filter filter) {
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

	void Texture2D::GenerateMipMaps() const {
		m_Internals->MipMapGenerated = true;

		glGenerateTextureMipmap(m_Internals->ID);
	}

	void Texture2D::Bind() const {
		m_Internals->Bind();
	}

	Ref<Texture2D> Texture2D::Create(const Vector2u& size, const uint8_t* pixels) {
		auto texture = MakeRef<Texture2D>();

		if (CheckTextureSize(size))
			texture->CreateTexture(size, pixels);

		return texture;
	}

	Ref<Texture2D> Texture2D::Create(Ref<Image> image) {
		auto texture = MakeRef<Texture2D>();

		if (CheckTextureSize(image->Size()))
			texture->CreateTexture(image->Size(), image->GetPixels().data());

		return texture;
	}

	void Texture2D::SetWrapping(Wrapping s) {
		SetWrappingS(s);
	}

	void Texture2D::SetWrapping(Wrapping s, Wrapping t) {
		SetWrappingS(s);
		SetWrappingS(t);
	}

	void Texture2D::SetWrappingS(Wrapping wrapping) {
		m_Internals->Wrapping.S = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_S, GetOpenWrapping(wrapping));
	}

	void Texture2D::SetWrappingT(Wrapping wrapping) {
		m_Internals->Wrapping.T = wrapping;
		m_Internals->SetParameter(GL_TEXTURE_WRAP_T, GetOpenWrapping(wrapping));
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

	Ref<Image> Texture2D::ToImage() const {
		const uint64_t size = static_cast<uint64_t>(m_Internals->Size.Width) * static_cast<uint64_t>(m_Internals->Size.Height);

		std::vector<Color> pixels;
		pixels.resize(size);

		m_Internals->GetImage(pixels.data(), static_cast<uint32_t>(size));
		return MakeRef<Image>(m_Internals->Size, pixels.data());
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

	void Texture2D::Update(Ref<Image> image) {
		Update(image->GetPixels().data(), image->Size(), {0, 0});
	}

	void Texture2D::Update(Ref<Image> image, const Vector2i& offset) {
		Update(image->GetPixels().data(), image->Size(), offset);
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
		const uint32_t maxSize = GetMaxDim();

		ENGINE_ASSERT(size.Width != 0 && size.Height != 0);
		ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);

		if (size.Width == 0 || size.Height == 0)
			throw std::runtime_error("Texture dimensions cannot be 0");


		if (size.Width > maxSize || size.Height > maxSize) {
			throw std::runtime_error(fmt::format("Unable to create texture width {}, texture is too big. Maximum texture size is {}", size, GetMaxSize()));
		}

		return true;
	}

	void Texture2D::CreateTexture(const Vector2u& size, const void* pixels) {
		m_Internals->Image(pixels, size);

		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture2D::Update(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(((offset.X + size.Width) < m_Internals->Size.Width) && ((offset.Y + size.Height) < m_Internals->Size.Height));
		if ((m_Internals->Size.Width < (offset.X + size.Width)) || (m_Internals->Size.Height < (offset.Y + size.Height)))
			throw std::out_of_range("Out of texture bounds");

		m_Internals->SubImage(pixels, size, offset);

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

	void Texture2D::Internals::Storage(const Vector2u& size) {
		glTextureStorage2D(ID, 1, GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void Texture2D::Internals::Image(const void* pixels, const Vector2u& size) {
		Size = size;

		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::Internals::SubImage(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		ENGINE_ASSERT(((offset.X + size.Width) < Size.Width) && ((offset.Y + size.Height) < Size.Height));
		if (size.Width < (offset.X + size.Width) || size.Height < (offset.Y + size.Height))
			throw std::out_of_range("SubImage out of range");

		glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::Internals::GetImage(void* pixels, uint32_t size) {
		ENGINE_ASSERT(static_cast<uint64_t>(size) < static_cast<uint64_t>(Size.Width) * static_cast<uint64_t>(Size.Height));

		glGetTextureImage(ID, 0, GL_RGBA8, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size), pixels);
	}

	void Texture2D::Internals::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(ID, name, parameter);
	}

	void Texture2D::Internals::GetParameter(uint32_t name, int* parameter) {
		glGetTextureParameteriv(ID, name, parameter);
	}
}
