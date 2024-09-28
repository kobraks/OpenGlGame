#pragma once
#include "Engine/Core/Base.h"
#include <memory>
#include <string.h>

namespace Engine {
	struct Buffer{
		void *Data;
		size_t Size;

		Buffer() : Data(nullptr), Size(0) {}
		Buffer(const void *data, size_t size) : Data(const_cast<void*>(data)), Size(size) {}
		Buffer(const Buffer &other, size_t size) : Data(other.Data), Size(size) {}

		static Buffer Copy(const Buffer &other) {
			Buffer buffer;

			buffer.Allocate(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);

			return buffer;
		}

		static Buffer Copy(const void *data, size_t size) {
			Buffer buffer;

			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);

			return buffer;
		}

		void Allocate(size_t size) {
			delete[] static_cast<uint8_t*>(Data);
			Data = nullptr;

			if (size == 0)
				return;

			Data = new uint8_t[size];
			Size = size;
		}

		void Release() {
			delete[] static_cast<uint8_t*>(Data);

			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize() {
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T &Read(size_t offset = 0) {
			return *static_cast<T*>(static_cast<uint8_t*>(Data) + offset);
		}

		template<typename T>
		const T &Read(size_t offset = 0) {
			return *static_cast<T*>(static_cast<uint8_t*>(Data) + offset);
		}

		uint8_t *ReadBytes(size_t size, size_t offset = 0) const {
			ENGINE_ASSERT(offset + size <= Size, "Buffer overflow!");

			uint8_t* buffer = new uint8_t[size];

			memcpy(buffer, static_cast<uint8_t*>(Data) + offset, size);
			return buffer;
		}

		void Write(const void* data, size_t size, size_t offset = 0) {
			ENGINE_ASSERT(offset + size <= Size, "Buffer overflow!");

			memcpy(static_cast<uint8_t*>(Data) + offset, data, size);
		}

		operator bool() const {
			return Data;
		}

		uint8_t &operator[](size_t idx) {
			return static_cast<uint8_t *>(Data)[idx];
		}

		uint8_t operator[](size_t idx) const {
			return static_cast<uint8_t *>(Data)[idx];
		}

		template<typename T>
		T *As() const {
			return static_cast<T *>(Data);
		}

		inline size_t GetSize()  const { return Size; }
	};
}