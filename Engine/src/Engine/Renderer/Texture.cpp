#include "pch.h"
#include "Engine/Renderer/Texture.h"

#include "Engine/Core/Image.h"

#include "Engine/Utils/Renderer/GLEnumConverters.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"
#include "Engine/Utils/Renderer/ImageFormatTraits.h"
#include "Engine/Utils/Renderer/FilterModeUtils.h"
#include "Engine/Utils/Renderer/PixelStoreScope.h"
#include "Engine/Utils/Renderer/ChannelUtils.h"

#include "glad/glad.h"

#include <fmt/format.h>

#include <cmath>
#include <array>
#include <cstddef>

namespace Engine {
	namespace Utils {
		static uint32_t CalculateMaxMipLevels(const Vector2u size) {
			return 1u + static_cast<uint32_t>(std::floor(std::log2(std::max(size.Width, size.Height))));
		}

		constexpr static GLenum GetTextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static Texture::IDType GenTexture(bool multisampled) {
			Texture::IDType name;

			glCreateTextures(GetTextureTarget(multisampled), 1, &name);

			return name;
		}

		void inline static CheckIfValidSize(const Vector2u& size) {
			const uint32_t maxSize = Texture::GetMaxSize();
			const uint32_t buffSize = size.Width * size.Height;

			ENGINE_ASSERT(buffSize != 0)
			ENGINE_ASSERT(size.Width <= maxSize && size.Height <= maxSize);

			if (buffSize == 0)
				throw std::runtime_error("Texture dimensions cannot be 0");


			if (size.Width > maxSize || size.Height > maxSize) {
				throw std::out_of_range(fmt::format(
					"Unable to create texture with size of {}, texture is too big. Maximum texture size is {}", size,
					maxSize));
			}
		}

		void inline static CheckSubRegionSize(const Vector2i& offset, const Vector2u& size, const Vector2u& texSize) {
			ENGINE_ASSERT(offset.X >= 0 && offset.Y >= 0);
			if (offset.X < 0 || offset.Y < 0) throw std::out_of_range("Negative subimage offset");

			const uint32_t width = size.Width + offset.X;
			const uint32_t height = size.Height + offset.Y;

			ENGINE_ASSERT(texSize.Width >= width && texSize.Height >= height);
			if (texSize.Width < width || texSize.Height < height)
				throw std::out_of_range("SubImage out of range");
		}

		inline Engine::Vector2u MipSize(const Engine::Vector2u& size, uint32_t level) {
			auto width = std::max(1u, size.Width >> level);
			auto height = std::max(1u, size.Height >> level);

			return { width, height };
		}
	}

	void Texture::Resize(const Vector2u& size) {
		if (size != m_GLState->Size) {
			LOG_ENGINE_INFO("Resizing texture '{}' from {} to {}", Label(), m_GLState->Size, size);
			const auto Image = ToImage();

			Recreate(size);

			Update(Image);
		}
	}

	void Texture::ResizeNoCopy(const Vector2u& size) {
		if (size != m_GLState->Size) {
			LOG_ENGINE_INFO("Resizing texture '{}' from {} to {}", Label(), m_GLState->Size, size);
			Recreate(size);
		}
	}

	void Texture::Invalidate() {
		m_GLState->Invalidate();
	}

	std::string Texture::DebugInfo() const {
		return fmt::format("Texture: {}x{}, format: {}, levels: {}, samples: {}, usage: {}, label: '{}'",
		                   m_GLState->Size.Width, m_GLState->Size.Height,
		                   m_GLState->ImageFormat,
		                   m_GLState->Levels,
		                   m_GLState->Samples,
		                   m_GLState->Usage,
		                   m_GLState->Label);
	}

	Texture::Texture(bool multisampled) : m_GLState(MakeRef<GLState>(multisampled)) {
	}

	void Texture::Initialize(const TextureSpec& spec) {
		SetupStorage(spec.Levels, spec.Samples, spec.Size, spec.Usage, spec.ImageFormat);
		SetupDefaultParameters(spec.Usage);

		if (!spec.Picture) return;

		ENGINE_ASSERT(spec.Samples == 1, "Initial pixels cannot be uploaded to multisampled textures");

		const auto picture = *spec.Picture;

		auto [defFormat, defType] = Utils::GetDefaultFormatAndType(spec.ImageFormat);
		const DataFormat dataFormat = picture.Format.value_or(defFormat);
		const DataType dataType = picture.DataType.value_or(defType);

		if (picture.Pixels) {
			const auto size = picture.Size == Vector2u{ 0, 0 } ? spec.Size : picture.Size;

			UploadPixels(picture.Pixels, size, { 0, 0 }, dataFormat, dataType, picture.MipLevel, picture.RowStrideBytes, picture.FlipY);
		}
	}

	void Texture::Recreate(const Vector2u& size) {
		Invalidate();
		ReAlloc(size);
		ReapplyParameters();
	}

	void Texture::GenerateMipMaps() const {
		if (m_GLState->Multisampled || m_GLState->Levels <= 1)
			return;

		m_GLState->MipMapGenerated = true;
		glGenerateTextureMipmap(m_GLState->ID);
	}

	void Texture::Bind() const {
		glBindTexture(Utils::GetTextureTarget(m_GLState->Multisampled), static_cast<GLuint>(*this));
	}

	void Texture::BindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, static_cast<GLuint>(*this));
	}

	void Texture::Unbind() const {
		glBindTexture(Utils::GetTextureTarget(m_GLState->Multisampled), 0);
	}

	void Texture::UnbindUnit(uint32_t sampler) const {
		glBindTextureUnit(sampler, 0);
	}

	Ref<Texture> Texture::Create(const TextureSpec& spec) {
		auto texture = Ref<Texture>(new Texture(spec.Samples > 1));

		texture->Initialize(spec);
		texture->SetLabel(spec.Label);

		if (spec.Levels > 1 && spec.Samples == 1 && spec.GenerateMips)
			texture->GenerateMipMaps();

		LOG_ENGINE_DEBUG("Created texture: {}", texture->DebugInfo());
		return texture;
	}

	Ref<Texture> Texture::Create(const Ref<Image>& image, Engine::ImageFormat imageFormat, uint32_t levels,
	                             uint32_t samples,
	                             const std::string& label) {
		TextureSpec spec;
		TextureSpec::InitialPixels initialPicture;

		initialPicture.Format = DataFormat::RGBA;
		initialPicture.DataType = DataType::UnsignedByte;
		initialPicture.Pixels = image->GetPixels().data();

		spec.Picture = initialPicture;
		spec.Size = image->Size();
		spec.ImageFormat = imageFormat;
		spec.Samples = samples;
		spec.Levels = levels;
		spec.Label = label;
		spec.Usage = TextureUsage::Default;

		return Create(spec);
	}

	Ref<Texture> Texture::Create(const Ref<Image>& image, TextureUsage usage, Engine::ImageFormat imageFormat,
	                             uint32_t levels, uint32_t samples,
	                             const std::string& label) {
		TextureSpec spec;
		TextureSpec::InitialPixels initialPicture;

		initialPicture.Format = DataFormat::RGBA;
		initialPicture.DataType = DataType::UnsignedByte;
		initialPicture.Pixels = image->GetPixels().data();

		spec.Picture = initialPicture;
		spec.Size = image->Size();
		spec.ImageFormat = imageFormat;
		spec.Samples = samples;
		spec.Levels = levels;
		spec.Label = label;
		spec.Usage = usage;

		return Create(spec);
	}


	void Texture::SetLabel(const std::string& label) {
		if (label.empty() && m_GLState->Label.empty())
			return;

		glObjectLabel(GL_TEXTURE, static_cast<GLuint>(*this), -1, label.data());
		m_GLState->Label = label;
	}

	void Texture::SetWrapping(WrapMode s) {
		SetWrappingS(s);
	}

	void Texture::SetWrapping(WrapMode s, WrapMode t) {
		SetWrappingS(s);
		SetWrappingT(t);
	}

	void Texture::SetWrappingS(WrapMode wrapping) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot specify wrapping S on multisampled texture");
			return;
		}

		m_GLState->Wrapping.S = wrapping;
		SetParameter(GL_TEXTURE_WRAP_S, static_cast<int>(Utils::EnumToGLConstant(wrapping)));
	}

	void Texture::SetWrappingT(WrapMode wrapping) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot specify wrapping T on multisampled texture");
			return;
		}

		m_GLState->Wrapping.T = wrapping;
		SetParameter(GL_TEXTURE_WRAP_T, static_cast<int>(Utils::EnumToGLConstant(wrapping)));
	}

	void Texture::SetFilters(FilterMode min, FilterMode mag) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set filters on multisampled texture");
			return;
		}

		SetMinFilter(min);
		SetMagFilter(mag);
	}

	void Texture::SetMinFilter(FilterMode filter) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set min filter on multisampled texture");
			return;
		}

		m_GLState->Filter.Min = filter;
		SetParameter(GL_TEXTURE_MIN_FILTER, static_cast<int>(Utils::EnumToGLConstant(filter)));
	}

	void Texture::SetMagFilter(FilterMode filter) {
		if (IsMultisampled()) {
			LOG_GL_WARN("Cannot set mag filter on multisampled texture");
			return;
		}

		if (filter > FilterMode::Linear) {
			LOG_ENGINE_ERROR("Only possible values for mag filter is Nearest or Linear");
			return;
		}

		m_GLState->Filter.Mag = Utils::SanitizeMag(filter);
		SetParameter(GL_TEXTURE_MAG_FILTER, static_cast<int>(Utils::EnumToGLConstant(Utils::SanitizeMag(filter))));
	}

	Ref<Image> Texture::ToImage(uint32_t mipLevel) const {
		// ENGINE_ASSERT(Utils::IsColorFormat(m_GLState->ImageFormat) && !Utils::IsIntegerColorFormat(m_GLState->ImageFormat))

		const Vector2u mipSize = Utils::MipSize(m_GLState->Size, mipLevel);
		const uint64_t pixelCount = static_cast<uint64_t>(mipSize.Width) * static_cast<uint64_t>(mipSize.Height);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);
		constexpr uint32_t bytesPerPixel = Utils::ChannelsFor(DataFormat::RGBA) * Utils::BytesPerChannel(DataType::UnsignedByte);
		const uint32_t bufSize = static_cast<uint32_t>(pixelCount) * bytesPerPixel;

		ENGINE_ASSERT(bufSize == pixels.size() * sizeof(Color));

		GetImage(pixels.data(), bufSize, DataFormat::RGBA, DataType::UnsignedByte, mipLevel);
		return Image::Create(mipSize, pixels.data());
	}

	Ref<Image> Texture::GetImage(const Vector2u& size, const Vector2i& offset, uint32_t mipLevel) const {
		// ENGINE_ASSERT(Utils::IsColorFormat(m_GLState->ImageFormat) && !Utils::IsIntegerColorFormat(m_GLState->ImageFormat))
		const uint64_t pixelCount = static_cast<uint64_t>(size.Width) * static_cast<uint64_t>(size.Height);

		std::vector<Color> pixels;
		pixels.resize(pixelCount);
		constexpr uint32_t bytesPerPixel = Utils::ChannelsFor(DataFormat::RGBA) * Utils::BytesPerChannel(DataType::UnsignedByte);
		const uint32_t bufSize = static_cast<uint32_t>(pixelCount) * bytesPerPixel;

		ENGINE_ASSERT(bufSize == pixels.size() * sizeof(Color));

		GetImage(pixels.data(), bufSize, size, offset, DataFormat::RGBA, DataType::UnsignedByte, mipLevel);
		return Image::Create(size, pixels.data());
	}

	void Texture::Clear(const Color& color) {
		Clear(&color.Code, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Clear(int value) {
		std::array vArray = { value, value, value, value };
		Clear(vArray.data(), DataFormat::RGBA, DataType::Int);
	}

	void Texture::Clear(const void* pixels, DataFormat dataFormat, DataType dataType) {
		Utils::UnpackAlignmentScope unpack(1);
		glClearTexImage(static_cast<GLuint>(*this), 0, Utils::EnumToGLConstant(dataFormat),
		                Utils::EnumToGLConstant(dataType),
		                pixels);

		// LOG_GL_TRACE("Clearing texture: label='{}', full size={}, format={}, type={}", Label(), Size(), dataFormat, dataType);
	}

	void Texture::ClearRegion(const Color& color, const Vector2i& offset, const Vector2u& size) {
		ClearRegion(&color.Code, offset, size, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::ClearRegion(int value, const Vector2i& offset, const Vector2u& size) {
		std::array vArray = { value, value, value, value };
		ClearRegion(vArray.data(), offset, size, DataFormat::RGBA, DataType::Int);
	}

	void Texture::ClearRegion(const void* pixels, const Vector2i& offset, const Vector2u& size, DataFormat dataFormat,
	                          DataType dataType) {
		Utils::CheckSubRegionSize(offset, size, Size());
		Utils::UnpackAlignmentScope unpack(1);

		glClearTexSubImage(static_cast<GLuint>(*this), 0, offset.X, offset.Y, 0, static_cast<GLsizei>(size.Width),
		                   static_cast<GLsizei>(size.Height), 1,
		                   Utils::EnumToGLConstant(dataFormat),
		                   Utils::EnumToGLConstant(dataType), pixels);

		LOG_GL_TRACE("Clearing texture region: offset={}, size={}, format={}, type={}",
		             offset, size, dataFormat, dataType);
	}

	void Texture::GetPixels(void* pixels, uint32_t bufSize, DataFormat format, DataType type, uint32_t mipLevel, uint32_t rowStrideBytes) const {
		ENGINE_ASSERT(pixels && bufSize != 0);
		if (!pixels || bufSize == 0)
			throw std::invalid_argument("Invalid pixel buffer");

		ENGINE_ASSERT(!IsMultisampled(), "Resolve MSAA texture before reading pixels");

		const Vector2u mipSize = Utils::MipSize(m_GLState->Size, mipLevel);
		const uint32_t channels = Utils::ChannelsFor(format);
		const uint32_t bpc = Utils::BytesPerChannel(type);
		const uint32_t bpp = channels * bpc;

		const uint64_t tightRowBytes = static_cast<uint64_t>(mipSize.Width) * static_cast<uint64_t>(bpp);
		const uint64_t tightTotal = tightRowBytes * static_cast<uint64_t>(mipSize.Height);

		if (rowStrideBytes != 0) {
			ENGINE_ASSERT((rowStrideBytes % bpp) == 0, "RowStrideBytes must be a multiple of bytes-per-pixel");
			ENGINE_ASSERT(rowStrideBytes >= tightRowBytes, "RowStrideBytes smaller than tight row size");

			const uint64_t minBuf = static_cast<uint64_t>(rowStrideBytes) * static_cast<uint64_t>(mipSize.Height);
			ENGINE_ASSERT(bufSize >= minBuf)
			if (bufSize < minBuf)
				throw std::out_of_range("Pixel buffer too small for requested row stride");
		} else {
			ENGINE_ASSERT(bufSize >= tightTotal)
			if (bufSize < tightTotal)
				throw std::out_of_range("Pixel buffer too small for requested image size");
		}

		const GLenum glFormat = Utils::EnumToGLConstant(format);
		const GLenum glType = Utils::EnumToGLConstant(type);

		Utils::PackAlignmentScope pack(1);
		if (rowStrideBytes == 0) {
			glGetTextureImage(static_cast<GLuint>(*this), static_cast<GLint>(mipLevel), glFormat, glType, static_cast<GLsizei>(bufSize), pixels);
		} else {
			Utils::PackRowLengthScope pack(static_cast<int>(rowStrideBytes / bpp));
			glGetTextureSubImage(static_cast<GLuint>(*this), static_cast<GLint>(mipLevel), 0, 0, 0, static_cast<GLsizei>(mipSize.Width), static_cast<GLsizei>(mipSize.Height), 1, glFormat, glType, static_cast<GLsizei>(bufSize), pixels);
		}
	}

	void Texture::Update(const uint8_t* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
	                     DataType dataType) {
		Update(static_cast<const void*>(pixels), size, offset, format, dataType);
	}

	void Texture::Update(const Color* pixels, const Vector2u& size, const Vector2i& offset) {
		Update(pixels, size, offset, DataFormat::RGBA, DataType::UnsignedByte);
	}

	void Texture::Update(const Texture& texture) {
		Update(texture, {0, 0});
	}

	void Texture::Update(const Texture& texture, const Vector2i& offset) {
		Update(texture.ToImage(), offset);
	}

	void Texture::Update(Ref<Image> image) {
		Update(image->GetPixels().data(), image->Size(), {0, 0});
	}

	void Texture::Update(Ref<Image> image, const Vector2i& offset) {
		Update(image->GetPixels().data(), image->Size(), offset);
	}

	void Texture::Swap(Texture& to) {
		LOG_ENGINE_TRACE("Swapping texture '{}' <-> '{}'", Label(), to.Label());
		std::swap(m_GLState, to.m_GLState);
	}

	uint32_t Texture::GetMaxSize() {
		static bool checked = false;
		static int32_t size = 0;

		if (!checked) {
			checked = true;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		}

		return static_cast<uint32_t>(size);
	}

	uint32_t Texture::QueryMaxColorSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	uint32_t Texture::QueryMaxIntegerSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	uint32_t Texture::QueryMaxDepthSamples() {
		static uint32_t maxSamples = 0;

		if (maxSamples == 0) {
			GLint tmp = 0;
			glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &tmp);
			maxSamples = static_cast<uint32_t>(tmp);
		}

		return maxSamples;
	}

	void Texture::Update(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat dataFormat,
	                     DataType dataType) {
		UploadPixels(pixels, size, offset, dataFormat, dataType);
	}

	void Texture::ReAlloc(const Vector2u& size) {
		const auto& samples = m_GLState->Samples;
		const auto& imageFormat = m_GLState->ImageFormat;
		const auto& usage = m_GLState->Usage;
		const auto& levels = m_GLState->Levels;

		SetupStorage(levels, samples, size, usage, imageFormat);
	}

	void Texture::SetupStorage(uint32_t levels, uint32_t samples, const Vector2u& size, TextureUsage usage,
	                           enum ImageFormat imageFormat) {
		Utils::CheckIfValidSize(size);

		m_GLState->MipMapGenerated = false;
		m_GLState->Size = size;
		m_GLState->ImageFormat = imageFormat;
		m_GLState->Samples = samples;
		m_GLState->Usage = usage;
		const auto& imageFormatGL = m_GLState->ImageFormatGL = Utils::EnumToGLConstant(imageFormat);

		if (m_GLState->Multisampled) {
			if (levels > 1) {
				LOG_ENGINE_WARN("Multisampled textures do not support mipmaps, ignoring levels > 1");
			}
		}
		else if (levels == 0) {
			levels = Utils::CalculateMaxMipLevels(size);
		}
		else if (levels > 1) {
			const uint32_t maxLevels = Utils::CalculateMaxMipLevels(size);
			if (levels > maxLevels) {
				LOG_ENGINE_WARN(
					"Requested {} mip levels, but only {} are possible for texture size of {}. Clamping to {}",
					levels, maxLevels, size, maxLevels);
				levels = maxLevels;
			}
		}

		if (usage == TextureUsage::DepthStencil) {
			ENGINE_ASSERT(Utils::IsDepthFormat(imageFormat), "DepthStencil usage requires a depth-capable format.");
		}
		if (usage == TextureUsage::Storage) {
			ENGINE_ASSERT(Utils::IsStorageCapable(imageFormat),
			              "Storage usage requires image load/store-capable format.");
		}

		if (samples > 1) {
			glTextureStorage2DMultisample(static_cast<GLuint>(*this), static_cast<GLsizei>(samples), imageFormatGL,
			                              static_cast<GLsizei>(size.Width), static_cast<GLsizei>(size.Height),
			                              GL_FALSE);

			m_GLState->Levels = 1;
		}
		else {
			glTextureStorage2D(static_cast<GLuint>(*this), static_cast<GLsizei>(levels), imageFormatGL,
			                   static_cast<GLsizei>(size.Width),
			                   static_cast<GLsizei>(size.Height));

			m_GLState->Levels = levels;
		}

		LOG_GL_DEBUG("Allocating texture storage: {}x{}, format: {}, usage: {}, samples: {}",
		             size.Width, size.Height, imageFormat, usage, samples);
	}

	void Texture::UploadPixels(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
	                           DataType dataType) {
		UploadPixels(pixels, size, offset, format, dataType, 0, 0, false);
	}

	void Texture::UploadPixels(const void* pixels, const Vector2u& size, const Vector2i& offset, DataFormat format,
		DataType dataType, uint32_t mipLevel, uint32_t rowStrideBytes, bool flipY) {

		ENGINE_ASSERT(pixels);

		if (!pixels)
			return;

		ENGINE_ASSERT(!IsMultisampled(), "Cannot upload pixels to multisampled texture");
		if (IsMultisampled())
			throw std::runtime_error("Resolve MSAA texture before upload pixels");

		ENGINE_ASSERT(mipLevel < m_GLState->Levels, fmt::format("Mip level {} out of range for texture with {} levels", mipLevel,
			m_GLState->Levels));

		if (mipLevel >= m_GLState->Levels) {
			throw std::out_of_range(fmt::format("Mip level {} out of range for texture with {} levels", mipLevel, m_GLState->Levels));
		}

		const Vector2u targetSize = Utils::MipSize(m_GLState->Size, mipLevel);
		Utils::CheckSubRegionSize(offset, size, targetSize);


		Utils::UnpackAlignmentScope unpack(1);

		std::unique_ptr<std::byte[]> flipped = nullptr;
		const uint32_t bpc = Utils::BytesPerChannel(dataType);
		const uint32_t channels = Utils::ChannelsFor(format);
		const uint32_t bpp = bpc * channels;
		const uint32_t tightBytes = size.Width * bpp;

		const void* uploadPtr = pixels;

		if (flipY) {
			flipped.reset(new std::byte[static_cast<std::size_t>(tightBytes) * size.Height]);
			auto* dst = flipped.get();

			const uint8_t* srcBase = static_cast<const uint8_t*>(pixels);
			const uint32_t stride = (rowStrideBytes != 0) ? rowStrideBytes : tightBytes;

			for (uint32_t y = 0; y < size.Height; ++y) {
				const uint32_t srcY = (size.Height - 1 - y);
				const auto* srcRow = srcBase + static_cast<std::size_t>(srcY) * stride;
				auto* dstRow = reinterpret_cast<uint8_t*>(dst) + static_cast<std::size_t>(y) * tightBytes;
				std::memcpy(dstRow, srcRow, tightBytes);
			}

			uploadPtr = flipped.get();
			rowStrideBytes = 0;
		}

		Scope<Utils::UnpackRowLengthScope> rowLengthScope;
		if (rowStrideBytes != 0) {
			ENGINE_ASSERT((rowStrideBytes % bpp) == 0, "RowStrideBytes must be  a multiple of bytes-per-pixel");

			const int rowLengthPixels = static_cast<int>(rowStrideBytes / bpp);
			rowLengthScope = MakeScope<Utils::UnpackRowLengthScope>(rowLengthPixels);
		}

		glTextureSubImage2D(static_cast<GLuint>(*this), static_cast<GLint>(mipLevel), offset.X, offset.Y, static_cast<GLsizei>(size.Width),
			static_cast<GLsizei>(size.Height), Utils::EnumToGLConstant(format),
			Utils::EnumToGLConstant(dataType), uploadPtr);

		LOG_GL_TRACE("Uploading texture data: size={}x{}, offset={}x{}, format={}, type={}, stride={} bytes, flipY={}", size.Width, size.Height, offset.X, offset.Y, format,
			dataType, rowStrideBytes, flipY);
	}

	void Texture::GetImage(void* pixels, uint32_t bufSize, DataFormat format, DataType dataType, uint32_t mipLevel) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Received uninitialized pointer to memory");

		ENGINE_ASSERT(!IsMultisampled(), "Resolve MSAA texture before reading pixels");
		if (IsMultisampled())
			throw std::runtime_error("Resolve MSAA texture before reading pixels");

		const Vector2u targetSize = Utils::MipSize(m_GLState->Size, mipLevel);
		const uint32_t bpp = Utils::ChannelsFor(format) * Utils::BytesPerChannel(dataType);
		const uint64_t bufSizeNeeded = static_cast<uint64_t>(targetSize.Width) * static_cast<uint64_t>(targetSize.Height) * bpp;

		ENGINE_ASSERT(bufSize >= bufSizeNeeded);
		if (bufSize < bufSizeNeeded)
			throw std::out_of_range("Buffer is too small");

		Utils::PackAlignmentScope pack(1);
		glGetTextureImage(static_cast<GLuint>(*this), static_cast<GLint>(mipLevel), Utils::EnumToGLConstant(format), Utils::EnumToGLConstant(dataType), static_cast<GLsizei>(bufSize),
		                  pixels);
	}

	void Texture::GetImage(void* pixels, uint32_t bufSize, const Vector2u& size, const Vector2i& offset, DataFormat format, DataType dataType, uint32_t mipLevel) const {
		ENGINE_ASSERT(pixels);
		if (!pixels)
			throw std::runtime_error("Received uninitialized pointer to memory");

		ENGINE_ASSERT(!IsMultisampled(), "Resolve MSAA texture before reading pixels");
		if (IsMultisampled())
			throw std::runtime_error("Resolve MSAA texture before reading pixels");

		const Vector2u targetSize = Utils::MipSize(m_GLState->Size, mipLevel);
		const uint32_t bpp = Utils::ChannelsFor(format) * Utils::BytesPerChannel(dataType);
		const uint64_t bufSizeNeeded = static_cast<uint64_t>(size.Width) * static_cast<uint64_t>(size.Height) * bpp;

		ENGINE_ASSERT(bufSize >= bufSizeNeeded);
		if (bufSize < bufSizeNeeded)
			throw std::out_of_range("Buffer is too small");

		Utils::CheckSubRegionSize(offset, size, targetSize);
		Utils::PackAlignmentScope pack(1);

		glGetTextureSubImage(static_cast<GLuint>(*this), static_cast<GLint>(mipLevel), offset.X, offset.Y, 0, static_cast<GLsizei>(size.Width),
		                     static_cast<GLsizei>(size.Height), 1, Utils::EnumToGLConstant(format), Utils::EnumToGLConstant(dataType),
		                     static_cast<GLsizei>(bufSize), pixels);
	}

	void Texture::SetParameter(uint32_t name, int parameter) {
		glTextureParameteri(static_cast<GLuint>(*this), name, parameter);
	}

	void Texture::GetParameter(uint32_t name, int* parameter) const {
		glGetTextureParameteriv(static_cast<GLuint>(*this), name, parameter);
	}

	void Texture::SetupDefaultParameters(TextureUsage usage) {
		if (usage == TextureUsage::DepthStencil || usage == TextureUsage::Storage)
			return;

		SetFilters(FilterMode::Nearest, FilterMode::Nearest);
		SetWrapping(WrapMode::Repeat, WrapMode::Repeat);
	}

	void Texture::ValidateSize(Vector2u size) {
		Utils::CheckIfValidSize(size);
	}

	void Texture::ReapplyParameters() {
		if (!m_GLState->Multisampled && (m_GLState->Usage != TextureUsage::DepthStencil && m_GLState->Usage !=
			TextureUsage::Storage)) {
			SetFilters(m_GLState->Filter.Min, m_GLState->Filter.Mag);
			SetWrapping(m_GLState->Wrapping.S, m_GLState->Wrapping.T);
		}
		if (!m_GLState->Label.empty()) {
			glObjectLabel(GL_TEXTURE, static_cast<GLuint>(*this), -1, m_GLState->Label.c_str());
		}
	}

	Texture::GLState::GLState(bool multisampled) : ID(Utils::GenTexture(multisampled)), Multisampled(multisampled) {
	}

	Texture::GLState::~GLState() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
	}

	void Texture::GLState::Invalidate() {
		if (ID != 0)
			glDeleteTextures(1, &ID);
		ID = Utils::GenTexture(Multisampled);
		if (!Label.empty())
			glObjectLabel(GL_TEXTURE, ID, -1, Label.data());
	}
}
