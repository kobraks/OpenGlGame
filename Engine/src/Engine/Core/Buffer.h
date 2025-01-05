#pragma once
#include "Engine/Core/Base.h"

namespace Engine {
	struct Buffer {
		using SizeType = uint64_t;

		void *Data;
		SizeType Size;

		Buffer();
		Buffer(void *data, SizeType size);
		Buffer(const Buffer &other, SizeType size);

		static Buffer Copy(const Buffer &other);
		static Buffer Copy(const void *data, SizeType size);

		void Allocate(SizeType size);
		void Release();

		void ZeroInitialize();

		template <typename T>
		T& Read(SizeType offset = 0);

		template <typename T>
		const T& Read(SizeType offset = 0);

		uint8_t* ReadBytes(SizeType size, SizeType offset = 0) const;
		void Write(const void *data, SizeType size, SizeType offset = 0);

		operator bool() const;

		uint8_t& operator[](SizeType idx);
		uint8_t operator[](SizeType idx) const;

		template <typename T>
		T* As() const;

		SizeType GetSize() const { return Size; }
	};

	template <typename T>
	T& Buffer::Read(SizeType offset) {
		return *static_cast<T*>(static_cast<uint8_t*>(Data) + offset);
	}

	template <typename T>
	const T& Buffer::Read(SizeType offset) {
		return *static_cast<T*>(static_cast<uint8_t*>(Data) + offset);
	}

	template <typename T>
	T* Buffer::As() const {
		return static_cast<T*>(Data);
	}
}
