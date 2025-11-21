#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Serializer/StreamReader.h"
#include "Engine/Serializer/StreamWriter.h"

namespace Engine {
	class BufferStreamWriter : public StreamWriter {
	public:
		BufferStreamWriter(BufferView& targetBuffer, Buffer::SizeType position = 0);
		BufferStreamWriter(const BufferStreamWriter&) = delete;
		virtual ~BufferStreamWriter() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		std::size_t GetStreamPosition() const override { return m_BufferPosition; }
		void SetStreamPosition(std::size_t position) override { m_BufferPosition = position; }
		bool WriteData(const std::byte *data, Stream::SizeType size) final;

		BufferView GetBuffer() const {
			const auto length = m_BufferPosition - m_BufferBeginPosition;
			return m_TargetBuffer.Slice(m_BufferBeginPosition, length);
		}
	private:
		BufferView& m_TargetBuffer;
		Buffer::SizeType m_BufferPosition = 0;
		Buffer::SizeType m_BufferBeginPosition = 0;
	};

	class BufferStreamReader : public StreamReader {
	public:
		BufferStreamReader(BufferView& targetBuffer, Buffer::SizeType position = 0);
		BufferStreamReader(const BufferStreamReader&) = delete;
		virtual ~BufferStreamReader() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		std::size_t GetStreamPosition() const override { return m_BufferPosition; }
		void SetStreamPosition(std::size_t position) override { m_BufferPosition = position; }
		bool ReadData(std::byte *destination, Stream::SizeType size) final;

		BufferView GetBuffer() const {
			const auto length = m_BufferPosition - m_BufferBeginPosition;
			return m_TargetBuffer.Slice(m_BufferBeginPosition, length);
		}

	private:
		BufferView& m_TargetBuffer;
		Buffer::SizeType m_BufferPosition = 0;
		Buffer::SizeType m_BufferBeginPosition = 0;
	};
}
