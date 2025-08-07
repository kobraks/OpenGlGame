#pragma once
#include "Engine/Core/BufferAccessorBase.h"

namespace Engine {
	class Buffer;
	class BufferView : public BufferAccessorBase<BufferView>{
	public:
		BufferView() = default;

		BufferView(std::byte* data, SizeType size, bool allowWrite = false);
		BufferView(const void* data, SizeType size, bool allowWrite = false);

		BufferView(const Buffer& buffer, SizeType offset = 0, bool allowWrite = true);
		BufferView(const Buffer& buffer, SizeType offset, SizeType length, bool allowWrite = true);

		BufferView(const BufferView& buffer, SizeType offset = 0);
		BufferView(const BufferView& buffer, SizeType offset, SizeType length);

		bool operator==(const BufferView& rth) const;

		template<typename T>
		static BufferView From(T& object) {
			return {reinterpret_cast<std::byte*>(&object), sizeof(T)};
		}

		BufferView Slice(SizeType offset, SizeType length) const;

		Buffer ToBuffer() const;

        SizeType Size() const { return m_Size; }

		std::byte* Data() { return m_Data; }
		const std::byte* Data() const { return m_Data; }

		void Read(std::byte* destination, SizeType size, SizeType offset = 0) const;

		void Fill(std::byte value);

		void FillZeros() { Fill(static_cast<std::byte>(0)); }

		void Write(const std::byte* data, SizeType size, SizeType offset = 0);
		void Write(const BufferView& buffer, SizeType offset = 0) { Write(buffer.Data(), buffer.Size(), offset); }

		template <typename T>
		void Write(const T& value, SizeType offset = 0) {
			Write(reinterpret_cast<const std::byte*>(&value), sizeof(T), offset);
		}

		bool IsWritable() const { return m_AllowWrite; }

        operator bool() const { return m_Data != nullptr; }

		bool Empty() const { return m_Data == nullptr || m_Size == 0; }
	private:
        std::byte* m_Data = nullptr;
        SizeType m_Size = 0;

		bool m_AllowWrite = true; // Allows writing to the buffer view, useful for read-only views
	};
}
