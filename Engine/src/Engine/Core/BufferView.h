#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Utils/CoreUtility.h"

#include <cstddef>
#include <span>

namespace Engine {
	class Buffer;
	class BufferView {
	public:
		using SizeType = uint64_t;

		BufferView() = default;

		BufferView(std::byte* data, SizeType size);
		BufferView(const void* data, SizeType size);

		BufferView(const Buffer& buffer, SizeType offset = 0);
		BufferView(const Buffer& buffer, SizeType offset, SizeType length);

		BufferView(const BufferView& buffer, SizeType offset = 0);
		BufferView(const BufferView& buffer, SizeType offset, SizeType length);

		bool operator==(const BufferView& rth) const;

		template<typename T>
		static BufferView From(T& object) {
			return {reinterpret_cast<std::byte*>(&object), sizeof(T)};
		}

        template<typename T>
        T& Read(SizeType offset = 0);

        template<typename T>
        const T& Read(SizeType offset = 0) const;

        template<typename T>
        T* As();

        template<typename T>
        const T* As() const;

		std::span<std::byte> AsSpan(SizeType offset = 0);
		std::span<const std::byte> AsSpan(SizeType offset = 0) const;

		std::span<std::byte> AsSpan(SizeType count, SizeType offset );
		std::span<const std::byte> AsSpan(SizeType count, SizeType offset) const;

		template<typename T>
        std::span<T> AsSpan(SizeType offset = 0);

        template<typename T>
        std::span<const T> AsSpan(SizeType offset = 0) const;

		template<typename T>
		std::span<T> AsSpan(SizeType elementCount, SizeType offset);

		template<typename T>
		std::span<const T> AsSpan(SizeType elementCount, SizeType offset = 0) const;

		BufferView Slice(SizeType offset, SizeType length) const;

        SizeType Size() const { return m_Size; }

		std::byte* Data() { return m_Data; }
		const std::byte* Data() const { return m_Data; }

        operator bool() const { return m_Data != nullptr; }

		template<typename T>
		bool IsAligned(SizeType offset = 0) const;

		template<typename T>
		T* Begin() { return As<T>();  }
		template<typename T>
		T* End() { return As<T>() + (m_Size / sizeof(T)); }

		template<typename T>
		const T* Begin() const { return As<T>(); }
		template<typename T>
		const T* End() const { return As<T>() + (m_Size / sizeof(T)); }
	private:
        std::byte* m_Data = nullptr;
        SizeType m_Size = 0;
	};

	template <typename T>
	T& BufferView::Read(SizeType offset) {
		ENGINE_ASSERT(offset + sizeof(T) <= m_Size);
		if (offset + sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("BufferView::Read<T>: Overflow at offset {} with type size {}", offset, sizeof(T)));
		}
		ENGINE_ASSERT(IsAligned<T>(offset))

		return *reinterpret_cast<T*>(m_Data + offset);
	}

	template <typename T>
	const T& BufferView::Read(SizeType offset) const {
		ENGINE_ASSERT(offset + sizeof(T) <= m_Size);
		if (offset + sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("BufferView::Read<T>: Overflow at offset {} with type size {}", offset, sizeof(T)));
		}
		ENGINE_ASSERT(IsAligned<T>(offset))

		return *reinterpret_cast<const T*>(m_Data + offset);
	}

	template <typename T>
	T* BufferView::As() {
		ENGINE_ASSERT(sizeof(T) <= m_Size);
		if (sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("BufferView::As<T>: Overflow with type size {}", sizeof(T)));
		}

		return reinterpret_cast<T*>(m_Data);
	}

	template <typename T>
	const T* BufferView::As() const {
		ENGINE_ASSERT(sizeof(T) <= m_Size);
		if (sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("BufferView::As<T>: Overflow with type size {}", sizeof(T)));
		}

		return reinterpret_cast<const T*>(m_Data);
	}

	template <typename T>
	std::span<T> BufferView::AsSpan(SizeType offset) {
		ENGINE_ASSERT(offset <= m_Size);
		ENGINE_ASSERT(IsAligned<T>(offset), "BufferView::AsSpan<T>(): Misaligned data access.");

		if (offset > m_Size)
			throw std::out_of_range("BufferView::AsSpan<T>(): Out of bounds access");

		const auto count = m_Size - offset;

		return { reinterpret_cast<T*>(m_Data + offset), count / sizeof(T)};
	}

	template <typename T>
	std::span<const T> BufferView::AsSpan(SizeType offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		ENGINE_ASSERT(IsAligned<T>(offset), "BufferView::AsSpan<T>(): Misaligned data access.");

		if (offset > m_Size)
			throw std::out_of_range("BufferView::AsSpan<T>(): Out of bounds access");

		const auto count = m_Size - offset;

		return { reinterpret_cast<const T*>(m_Data + offset), count / sizeof(T) };
	}

	template <typename T>
	std::span<T> BufferView::AsSpan(SizeType elementCount, SizeType offset) {
		ENGINE_ASSERT(IsAligned<T>(offset), "BufferView::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "BufferView::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	std::span<const T> BufferView::AsSpan(SizeType elementCount, SizeType offset) const {
		ENGINE_ASSERT(IsAligned<T>(offset), "BufferView::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "BufferView::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferView::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<const T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	bool BufferView::IsAligned(SizeType offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::runtime_error(fmt::format("BufferView::IsAligned<T>: Overflow with offset {}", offset));

		return Utils::IsAlignTo<T>(m_Data, offset);
	}
}
