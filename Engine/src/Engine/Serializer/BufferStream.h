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
		BufferStreamWriter(Buffer targetBuffer, size_t position = 0);
		BufferStreamWriter(const BufferStreamWriter&) = delete;
		virtual ~BufferStreamWriter() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		size_t GetStreamPosition() override { return m_BufferPosition; }
		void SetStreamPosition(size_t position) override { m_BufferPosition = position; }
		bool WriteData(const char *data, size_t size) final;

		Buffer GetBuffer() const { return Buffer(m_TargetBuffer, m_BufferPosition); }
	private:
		Buffer m_TargetBuffer;
		size_t m_BufferPosition = 0;
	};

	class BufferStreamReader : public StreamReader {
		BufferStreamReader(Buffer targetBuffer, size_t position = 0);
		BufferStreamReader(const BufferStreamWriter&) = delete;
		virtual ~BufferStreamReader() override = default;

		bool IsStreamGood() const final { return static_cast<bool>(m_TargetBuffer); }
		size_t GetStreamPosition() override { return m_BufferPosition; }
		void SetStreamPosition(size_t position) override { m_BufferPosition = position; }
		bool ReadData(char *destination, size_t size) final;

		Buffer GetBuffer() const { return Buffer(m_TargetBuffer, m_BufferPosition); }

	private:
		Buffer m_TargetBuffer;
		size_t m_BufferPosition = 0;
	};
}
