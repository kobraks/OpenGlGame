#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Buffer.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Renderer/BufferEnums.h"

#include "Engine/Utils/Renderer/BufferUsageMapper.h"

namespace Engine {
	class MappedBufferRegion {
		friend class BufferObject;
	public:
		~MappedBufferRegion();

		bool IsValid() const { return m_Data != nullptr && m_Size > 0; }
		explicit operator bool() const { return IsValid(); }

		const std::byte* Read(uint32_t offset = 0) const;
		std::byte* Read(uint32_t offset = 0);

		void Write(const std::byte* data, uint32_t size, uint32_t offset = 0);

		template<typename T>
		void Write(const T& value, uint32_t offset = 0) {
			return Write(&value, sizeof(T), offset);
		}

		template<typename T>
		T Read(const uint32_t offset = 0) const {
			const std::byte* data = Read();
			ENGINE_ASSERT(offset + sizeof(T) <= m_Size);

			if (offset + sizeof(T) > m_Size)
				throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, sizeof(T), m_Size));

			T value;
			std::memcpy(&value, data + offset, sizeof(T));

			return value;
		}

		void Write(const BufferView& buffer, uint32_t offset = 0) { return Write(buffer.Data(), static_cast<uint32_t>(buffer.Size()), offset); }
		void Write(std::span<std::byte> bytes, uint32_t offset = 0) { return Write(bytes.data(), static_cast<uint32_t>(bytes.size_bytes()), offset); }

		template<typename T>
		void Write(std::span<const T> span, uint32_t offset = 0) { return Write(reinterpret_cast<const std::byte*>(span.data()), static_cast<uint32_t>(span.size_bytes()), offset); }

		Buffer Copy(uint32_t size = 0, uint32_t offset = 0) const;
		void CopyTo(Buffer& buffer, uint32_t offset = 0, uint32_t size = 0) const;
		void CopyTo(MappedBufferRegion& buffer, uint32_t offset = 0, uint32_t size = 0) const;
		void CopyTo(MappedBufferRegion& dstBuffer, uint32_t srcOffset = 0, uint32_t dstOffset = 0, uint32_t size = 0) const;

		void CopyFrom(const BufferView& buffer, uint32_t offset, uint32_t size);
		void CopyFrom(MappedBufferRegion& buffer, uint32_t offset = 0, uint32_t size = 0);
		void CopyFrom(MappedBufferRegion& srcBuffer, uint32_t srcOffset = 0, uint32_t dstOffset = 0, uint32_t size = 0);

		template<typename T>
		void CopyFrom(std::span<const T> span, uint32_t offset = 0) {
			CopyFrom(BufferView{ reinterpret_cast<const std::byte*>(span.data()), span.size_bytes() }, offset);
		}

		BufferView View(uint32_t offset = 0, uint32_t size = 0) const;

		uint32_t Size() const { return m_Size; }

		BufferAccess Access() const { return m_Access; }

		void Flush(uint32_t offset = 0, uint32_t size = 0);

		std::span<std::byte> AsSpan(uint32_t offset = 0);
		std::span<const std::byte> AsSpan(uint32_t offset = 0) const;

		std::span<std::byte> AsSpan(uint32_t count, uint32_t offset);
		std::span<const std::byte> AsSpan(uint32_t count, uint32_t offset) const;

		template<typename T>
		std::span<T> AsSpan(uint32_t offset = 0);

		template<typename T>
		std::span<const T> AsSpan(uint32_t offset = 0) const;

		template<typename T>
		std::span<T> AsSpan(uint32_t elementCount, uint32_t offset);

		template<typename T>
		std::span<const T> AsSpan(uint32_t elementCount, uint32_t offset) const;

		template<typename T>
		bool IsAligned(uint32_t offset) const;

		MappedBufferRegion(const MappedBufferRegion&) = delete;
		MappedBufferRegion(MappedBufferRegion&&) noexcept = delete;

		MappedBufferRegion& operator=(const MappedBufferRegion&) = delete;
		MappedBufferRegion& operator=(MappedBufferRegion&&) noexcept = delete;
	protected:
		MappedBufferRegion(BufferAccess access, const BufferObject& buffer);
		MappedBufferRegion(BufferAccess access, const BufferObject& buffer, uint32_t size, uint32_t offset);

	private:
		std::byte* m_Data = nullptr;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;

		const BufferAccess m_Access;
		const BufferObject& m_Buffer;
	};

	template <typename T>
	std::span<T> MappedBufferRegion::AsSpan(uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		ENGINE_ASSERT(IsAligned<T>(offset), "MappedBufferRegion::AsSpan<T>(): Misaligned data access.");

		const auto count = m_Size - offset;
		ENGINE_ASSERT(count % sizeof(T) == 0, "MappedBufferRegion::AsSpan<T>(): Size is not a multiple of T.");

		return { reinterpret_cast<T*>(m_Data + offset), count / sizeof(T) };
	}

	template <typename T>
	std::span<const T> MappedBufferRegion::AsSpan(uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		ENGINE_ASSERT(IsAligned<T>(offset), "MappedBufferRegion::AsSpan<T>(): Misaligned data access.");

		const auto count = m_Size - offset;

		ENGINE_ASSERT(count % sizeof(T) == 0, "MappedBufferRegion::AsSpan<T>(): Size is not a multiple of T.");


		return { reinterpret_cast<const T*>(m_Data + offset), count / sizeof(T) };
	}

	template <typename T>
	std::span<T> MappedBufferRegion::AsSpan(uint32_t elementCount, uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(IsAligned<T>(offset), "MappedBufferRegion::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "MappedBufferRegion::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	std::span<const T> MappedBufferRegion::AsSpan(uint32_t elementCount, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(IsAligned<T>(offset), "MappedBufferRegion::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "MappedBufferRegion::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<const T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	bool MappedBufferRegion::IsAligned(uint32_t offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::IsAligned<T>(): Offset {} exceeds buffer size {}", offset, m_Size));

		return Utils::IsAlignTo<T>(m_Data, offset);
	}
}