#pragma once
#include "Engine/Core/BufferAccessorBase.h"

namespace Engine {
	class BufferView;

	class Buffer : public BufferAccessorBase<Buffer> {
	public:
		friend class BufferView;
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
		static Buffer Copy(const BufferView& buffer);
		static Buffer FromSpan(std::span<const std::byte> span);

		void CopyFrom(const void* data, SizeType size, SizeType offset = 0);
		void CopyFrom(const BufferView& buffer, SizeType offset = 0);

		void Allocate(SizeType size);
		void Resize(SizeType newSize);
		void Release();
		void ZeroInitialize() { if (!Empty()) Fill(static_cast<std::byte>(0)); }
		void Fill(std::byte value);

		std::byte* Data() { return m_Data.get(); }
		const std::byte* Data() const { return m_Data.get(); }

		SizeType Size() const { return m_Size; }

		BufferView Slice(SizeType offset, SizeType length) const;

		Buffer Clone() const;

		void Read(std::byte* destination, SizeType size, SizeType offset = 0) const;

		void Write(const std::byte* data, SizeType size, SizeType offset = 0);
		void Write(const BufferView& buffer, SizeType offset = 0);

		template <typename T>
		void Write(const T& value, SizeType offset = 0) {
			Write(reinterpret_cast<const std::byte*>(&value), sizeof(T), offset);
		}
	private:
		std::unique_ptr<std::byte[]> m_Data = nullptr;
		SizeType m_Size = 0;
	};
}
