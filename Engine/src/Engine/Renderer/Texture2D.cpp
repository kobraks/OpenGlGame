#include "pch.h"
#include "Engine/Renderer/Texture2D.h"
#include "Engine/Core/Image.h"

#include "glad/glad.h"

namespace Engine {
	static uint32_t GenTexture(bool multisampled) {
		uint32_t name;

		if (multisampled)
			glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &name);
		else
			glCreateTextures(GL_TEXTURE_2D, 1, &name);

		return name;
	}

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

	Texture2D::Texture2D(bool multisampled) : m_Internals(MakeRef<Internals>(multisampled)) {
	}

	void Texture2D::GenerateMipMaps() const {
		m_Internals->MipMapGenerated = true;

		glGenerateTextureMipmap(m_Internals->ID);
	}

	void Texture2D::Bind() const {
		m_Internals->Bind();
	}

	void Texture2D::BindUnit(uint32_t sampler) const {
		m_Internals->BindUnit(sampler);
	}

	Ref<Texture2D> Texture2D::Create(const Vector2u& size, const uint8_t* pixels) {
		auto texture = Ref<Texture2D>(new Texture2D());

		if (CheckSize(size))
			texture->CreateTexture(size, pixels);

		return texture;
	}

	Ref<Texture2D> Texture2D::Create(const Vector2u& size, uint32_t samples, const uint8_t* pixels) {
		auto texture = Ref<Texture2D>(new Texture2D(samples > 1));

		if (CheckSize(size))
			texture->CreateTexture(samples, size, pixels);

		return texture;
	}

	Ref<Texture2D> Texture2D::Create(Ref<Image> image) {
		auto texture = Ref<Texture2D>(new Texture2D());

		if (CheckSize(image->Size()))
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

	Ref<Image> Texture2D::GetImage(const Vector2u& size, const Vector2i& offset) const {
		const uint64_t pixelCount = static_cast<uint64_t>(size.Width - offset.X) * static_cast<uint64_t>(size.Height - offset.Y);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);

		m_Internals->GetImage(pixels.data(), static_cast<uint32_t>(pixelCount), size, offset);
		return MakeRef<Image>(size, pixels.data());
	}

	void Texture2D::Clear() {
		m_Internals->Clear(nullptr);
	}

	void Texture2D::Clear(const Vector2i& offset, const Vector2u& size) {
		m_Internals->Clear(nullptr, offset, size);
	}

	void Texture2D::GetPixels(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		ENGINE_ASSERT(size != 0);

		if (!pixels || size == 0)
			throw std::runtime_error("Uninitialized memory access");

		if (size >= m_Internals->Size.Width * m_Internals->Size.Height)
			m_Internals->GetImage(pixels, size);
		else
			throw std::out_of_range("Specified buffer is too small");
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

	void Texture2D::Swap(Texture2D& to) {
		std::swap(m_Internals, to.m_Internals);
	}

	uint32_t Texture2D::GetMaxSize() {
		static bool checked = false;
		static int32_t size = 0;

		if (!checked) {
			checked = true;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		}

		return static_cast<uint32_t>(size);
	}

	bool Texture2D::CheckSize(const Vector2u& size) {
		const uint32_t maxSize = GetMaxSize();
		const uint32_t texSize = size.Width * size.Height;

		ENGINE_ASSERT(texSize != 0)
		ENGINE_ASSERT(size.Width < maxSize && size.Height < maxSize);

		if (texSize == 0)
			throw std::runtime_error("Texture dimensions cannot be 0");


		if (size.Width > maxSize || size.Height > maxSize) {
			throw std::out_of_range(fmt::format("Unable to create texture with size of {}, texture is too big. Maximum texture size is {}", size, maxSize));
		}

		return true;
	}

	void Texture2D::CreateTexture(const Vector2u& size, const void* pixels) {
		//m_Internals->Image(pixels, size);
		m_Internals->Allocate(size);

		if (pixels)
			m_Internals->SendImage(pixels, size);

		SetFilters(Filter::Nearest, Filter::Nearest);
		SetWrapping(Wrapping::Repeat, Wrapping::Repeat);
	}

	void Texture2D::CreateTexture(uint32_t samples, const Vector2u& size, const void* pixels) {
		if (samples > 1) {
			m_Internals->Allocate(size, samples);

			if (pixels)
				m_Internals->SendImage(pixels, size);
		}
		else
			CreateTexture(size, pixels);
	}

	void Texture2D::Update(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		m_Internals->SendImage(pixels, size, offset);
	}

	Texture2D::Internals::Internals(bool multisampled) : ID(GenTexture(multisampled)), Multisampled(multisampled) {
	}

	Texture2D::Internals::~Internals() {
		glDeleteTextures(1, &ID);
	}

	void Texture2D::Internals::Bind() const {
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Texture2D::Internals::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, ID);
	}

	void Texture2D::Internals::Allocate(const Vector2u& size) {
		Size = size;

		glTextureStorage2D(ID, 1, GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height));
	}

	void Texture2D::Internals::Allocate(const Vector2u& size, uint32_t samples) {
		Size = size;

		glTextureStorage2DMultisample(ID, static_cast<GLsizei>(samples), GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), GL_FALSE);
	}

	// void Texture2D::Internals::Image(const void* pixels, const Vector2u& size) {
	// 	Size = size;
	//
	// 	Bind();
	// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	// }

	void Texture2D::Internals::SendImage(const void* pixels, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			return;

		CheckSubRegionSize(offset, size);

		glTextureSubImage2D(ID, 0, offset.X, offset.Y, static_cast<GLsizei>(size.X), static_cast<GLsizei>(size.Y), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::Internals::GetImage(void* pixels, uint32_t size) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(static_cast<uint64_t>(size) < static_cast<uint64_t>(Size.Width) * static_cast<uint64_t>(Size.Height));

		glGetTextureImage(ID, 0, GL_RGBA8, GL_UNSIGNED_BYTE, static_cast<GLsizei>(size), pixels);
	}

	void Texture2D::Internals::GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset) {
		ENGINE_ASSERT(pixels);

		const uint32_t gettingSize = (size.Width - offset.X) * (size.Height - offset.Y);

		if (!pixels)
			throw std::runtime_error("Recived uninitialized pointer to memory");

		ENGINE_ASSERT(bufSize >= gettingSize);
		if (bufSize < gettingSize)
			throw std::out_of_range("Buffer is too small");

		CheckSubRegionSize(offset, size);

		glGetTextureSubImage(ID, 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, bufSize, pixels);
	}

	void Texture2D::Internals::Clear(void* pixels) {
		glClearTexImage(ID, 0, GL_RGBA8, GL_INT, pixels);
	}

	void Texture2D::Internals::Clear(void* pixels, const Vector2i& offset, const Vector2u& size) {
		CheckSubRegionSize(offset, size);
		
		glClearTexSubImage(ID, 0, offset.X, offset.Y, 0, size.Width, size.Height, 0, GL_RGBA8, GL_INT, nullptr);
	}

	bool Texture2D::Internals::CheckSubRegionSize(const Vector2i& offset, const Vector2u& size) const {
		ENGINE_ASSERT(Size.Width >= size.Width + offset.X && Size.Height >= size.Height + offset.Y);
		if (Size.Width < size.Width + offset.X || Size.Height < size.Height + offset.Y)
			throw std::out_of_range("SubImage out of range");

		return true;
	}

	void Texture2D::Internals::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(ID, name, parameter);
	}

	void Texture2D::Internals::GetParameter(uint32_t name, int* parameter) const {
		glGetTextureParameteriv(ID, name, parameter);
	}
}
