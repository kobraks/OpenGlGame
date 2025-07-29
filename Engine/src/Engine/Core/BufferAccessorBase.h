#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Utils/CoreUtility.h"

#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>

namespace Engine {
	template<class Derived, typename SizeT = uint64_t>
	class BufferAccessorBase {
	public:
		using SizeType = SizeT;

		SizeType Size() const { return DerivedThis()->Size(); }
		const std::byte* Data() const { return DerivedThis()->Data(); }
		std::byte* Data() { return DerivedThis()->Data(); }

		bool ContainsOffset(SizeType offset) const {
			return offset < Size();
		}

		bool ContainsRange(SizeType offset, SizeType rangeSize) const {
			return offset <= Size() && (offset + rangeSize) <= Size();
		}

		template <typename T>
		bool Contains(SizeType offset = 0) const {
			return ContainsRange(offset, sizeof(T));
		}

		template <typename T>
		bool ContainsSpan(SizeType elementCount, SizeType offset = 0) const {
			return ContainsRange(offset, elementCount * sizeof(T));
		}

		template<typename T>
		T& Read(SizeType offset = 0) {
			CheckRead<T>(offset);
			return *reinterpret_cast<T*>(Data() + offset);
		}

		template <typename T>
		const T& Read(SizeType offset = 0) const {
			CheckRead<T>(offset);
			return *reinterpret_cast<const T*>(Data() + offset);
		}

		template <typename T>
		T* As() {
			CheckRead<T>(0);
			return reinterpret_cast<T*>(Data());
		}

		template <typename T>
		const T* As() const {
			CheckRead<T>(0);
			return reinterpret_cast<const T*>(Data());
		}

		std::span<std::byte> AsSpan(SizeType offset = 0) {
			ENGINE_ASSERT(offset <= Size());
			if (offset > Size()) {
				throw std::out_of_range(fmt::format("BufferAccessorBase::AsSpan: Offset {} is out of bounds (size={})", offset, Size()));
			}
			return { Data() + offset, Size() - offset };
		}

		std::span<const std::byte> AsSpan(SizeType offset = 0) const {
			ENGINE_ASSERT(offset <= Size());
			if (offset > Size()) {
				throw std::out_of_range(fmt::format("BufferAccessorBase::AsSpan: Offset {} is out of bounds (size={})", offset, Size()));
			}
			return { Data() + offset, Size() - offset };
		}

		template <typename T>
		std::span<T> AsSpan(SizeType offset = 0) {
			CheckSpan<T>(Size() / sizeof(T), offset);
			return { reinterpret_cast<T*>(Data() + offset), (Size() - offset) / sizeof(T) };
		}

		template <typename T>
		std::span<const T> AsSpan(SizeType offset = 0) const {
			CheckSpan<T>(Size(), offset);
			return { reinterpret_cast<const T*>(Data() + offset), (Size() - offset) / sizeof(T) };
		}

		template <typename T>
		std::span<T> AsSpan(SizeType elementCount, SizeType offset) {
			CheckSpan<T>(elementCount, offset);
			return { reinterpret_cast<T*>(Data() + offset), elementCount };
		}

		template <typename T>
		std::span<const T> AsSpan(SizeType elementCount, SizeType offset) const {
			CheckSpan<T>(elementCount, offset);
			return { reinterpret_cast<const T*>(Data() + offset), elementCount };
		}

		template <typename T>
		bool IsAligned(SizeType offset = 0) const {
			ENGINE_ASSERT(offset <= Size());
			if (offset > Size()) {
				throw std::out_of_range(fmt::format("BufferAccessorBase::IsAligned<T>: Offset {} is out of bounds (size={})", offset, Size()));
			}

			return Utils::IsAlignTo<T>(Data(), offset);
		}

		template<typename T>
		T* Begin() { return As<T>(); }
		template<typename T>
		T* End() { return As<T>() + (Size() / sizeof(T)); }

		template<typename T>
		const T* Begin() const { return As<T>(); }
		template<typename T>
		const T* End() const { return As<T>() + (Size() / sizeof(T)); }

	protected:
		Derived* DerivedThis() { return static_cast<Derived*>(this); }
		const Derived* DerivedThis() const { return static_cast<const Derived*>(this); }

		template<typename T>
		void CheckRead(SizeType offset) const {
			ENGINE_ASSERT(offset + sizeof(T) <= Size());
			ENGINE_ASSERT(IsAligned<T>(offset));

			if (offset + sizeof(T) > Size()) {
				throw std::runtime_error(fmt::format("BufferAccessorBase::Read<T>: Overflow at offset {} with type size {}", offset, sizeof(T)));
			}

			if (!IsAligned<T>(offset)) {
				throw std::runtime_error(fmt::format("BufferAccessorBase::Read<T>: Misaligned access at offset {}", offset));
			}
		}

		template<typename T>
		void CheckSpan(SizeType count, SizeType offset) const {
			ENGINE_ASSERT((count * sizeof(T)) + offset <= Size());
			ENGINE_ASSERT(IsAligned<T>(offset));

			if ((count * sizeof(T)) + offset > Size()) {
				throw std::out_of_range(fmt::format("BufferAccessorBase::AsSpan<T>: Out of bounds (count={}, offset={}, size={})", count, offset, Size()));
			}

			if (!IsAligned<T>(offset)) {
				throw std::runtime_error(fmt::format("BufferAccessorBase::AsSpan<T>: Misaligned access at offset {}", offset));
			}
		}

	private:
		friend Derived;

		BufferAccessorBase() = default;
	};
}