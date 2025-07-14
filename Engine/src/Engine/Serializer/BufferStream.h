#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"

#include "Engine/Serializer/StreamReader.h"
#include "Engine/Serializer/StreamWriter.h"

#include <filesystem>
#include <fstream>

namespace Engine {
	class BufferStreamWriter : public StreamWriter {
	public:
		BufferStreamWriter(Buffer& targetBuffer, Buffer::SizeType position = 0);
		BufferStreamWriter(const BufferStreamWriter&) = delete;
		virtual ~BufferStreamWriter() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		std::size_t GetStreamPosition() override { return m_BufferPosition; }
		void SetStreamPosition(std::size_t position) override { m_BufferPosition = position; }
		bool WriteData(const std::byte *data, std::size_t size) final;

		Buffer GetBuffer() const { return { m_TargetBuffer, m_BufferPosition }; }
	private:
		Buffer& m_TargetBuffer;
		Buffer::SizeType m_BufferPosition = 0;
	};

	class BufferStreamReader : public StreamReader {
	public:
		BufferStreamReader(Buffer& targetBuffer, Buffer::SizeType position = 0);
		BufferStreamReader(const BufferStreamWriter&) = delete;
		virtual ~BufferStreamReader() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		std::size_t GetStreamPosition() override { return m_BufferPosition; }
		void SetStreamPosition(std::size_t position) override { m_BufferPosition = position; }
		bool ReadData(std::byte *destination, std::size_t size) final;

		Buffer GetBuffer() const { return { m_TargetBuffer, m_BufferPosition };}

	private:
		Buffer& m_TargetBuffer;
		Buffer::SizeType m_BufferPosition = 0;
	};
}
