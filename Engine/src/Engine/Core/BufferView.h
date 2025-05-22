#pragma once
#include "Engine/Core/Base.h"

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

        std::span<std::byte> AsSpan();
        std::span<const std::byte> AsSpan() const;

        template<typename T>
        std::span<T> AsSpan();

        template<typename T>
        std::span<const T> AsSpan() const;

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
	std::span<T> BufferView::AsSpan() {
		return { reinterpret_cast<T*>(m_Data), m_Size / sizeof(T)};
	}

	template <typename T>
	std::span<const T> BufferView::AsSpan() const {
		return { reinterpret_cast<const T*>(m_Data), m_Size / sizeof(T) };
	}

	template <typename T>
	bool BufferView::IsAligned(SizeType offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::runtime_error(fmt::format("BufferView::IsAligned<T>: Overflow with offset {}", offset));

		return reinterpret_cast<uintptr_t>(m_Data + offset) % alignof(T) == 0;
	}
}
