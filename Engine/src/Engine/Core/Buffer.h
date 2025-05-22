#pragma once
#include "Engine/Core/Base.h"
#include <span>
#include <cstddef>

namespace Engine {
	class BufferView;

	class Buffer {
	public:
		friend class BufferView;
		using SizeType = uint64_t;

		Buffer() = default;
		explicit Buffer(SizeType size);
		explicit Buffer(const BufferView& view);
		Buffer(const void* data, SizeType size);
		Buffer(const Buffer& other, SizeType offset = 0);
		Buffer(Buffer&& other) noexcept;
		~Buffer();

		Buffer& operator=(const Buffer& other);
		Buffer& operator=(Buffer&& other) noexcept;

		operator bool() const { return m_Data != nullptr;  }

		bool operator==(const Buffer& rth) const;

		bool Empty() const { return m_Data == nullptr || m_Size == 0; }

		static Buffer Copy(const void* data, SizeType size);
		static Buffer FromSpan(std::span<const std::byte> span);

		void Allocate(SizeType size);
		void Resize(SizeType newSize);
		void Release();
		void ZeroInitialize();

		std::byte* Data() { return m_Data.get(); }
		const std::byte* Data() const { return m_Data.get(); }

		SizeType Size() const { return m_Size; }

		void Write(const void* data, SizeType size, SizeType offset = 0);

		template<typename T>
		T& Read(SizeType offset = 0);

		template<typename T>
		const T& Read(SizeType offset = 0) const;

		template<typename T>
		T* As();

		template<typename T>
		const T* As() const;

		template<typename T>
		std::span<T> AsSpan() { return { As<T>(), m_Size / sizeof(T) }; }

		template<typename T>
		std::span<T> AsSpan() const { return { As<T>(), m_Size / sizeof(T) }; }

		template<typename T>
		bool IsAligned(SizeType offset = 0) const;
	private:
		std::unique_ptr<std::byte[]> m_Data = nullptr;
		SizeType m_Size = 0;
	};

	template <typename T>
	T& Buffer::Read(SizeType offset) {
		ENGINE_ASSERT(offset + sizeof(T) <= m_Size);
		if (offset + sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("Buffer::Read<T>: Overflow at offset {} with type size {}", offset, sizeof(T)));
		}
		ENGINE_ASSERT(IsAligned<T>(offset))

			return *reinterpret_cast<T*>(Data() + offset);
	}

	template <typename T>
	const T& Buffer::Read(SizeType offset) const {
		ENGINE_ASSERT(offset + sizeof(T) <= m_Size);
		if (offset + sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("Buffer::Read<T>: Overflow at offset {} with type size {}", offset, sizeof(T)));
		}
		ENGINE_ASSERT(IsAligned<T>(offset))

			return *reinterpret_cast<T*>(Data() + offset);
	}

	template <typename T>
	T* Buffer::As() {
		ENGINE_ASSERT(sizeof(T) <= m_Size);
		if (sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("Buffer::As<T>: Overflow with type size {}", sizeof(T)));
		}

		return reinterpret_cast<T*>(Data());
	}

	template <typename T>
	const T* Buffer::As() const {
		ENGINE_ASSERT(sizeof(T) <= m_Size);
		if (sizeof(T) > m_Size) {
			throw std::runtime_error(fmt::format("Buffer::As<T>: Overflow with type size {}", sizeof(T)));
		}

		return reinterpret_cast<const T*>(Data());
	}

	template <typename T>
	bool Buffer::IsAligned(SizeType offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::runtime_error(fmt::format("Buffer::IsAligned<T>: Overflow with offset {}", offset));

		return reinterpret_cast<uintptr_t>(Data() + offset) % alignof(T) == 0;
	}
}
