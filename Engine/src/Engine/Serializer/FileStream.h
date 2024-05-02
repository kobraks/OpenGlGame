#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Serializer/StreamReader.h"
#include "Engine/Serializer/StreamWriter.h"

#include <fstream>
#include <filesystem>

namespace Engine {
	class FileStreamWriter : public StreamWriter {
	public:
		FileStreamWriter(const std::filesystem::path &path);
		FileStreamWriter(const FileStreamWriter&) = delete;

		virtual ~FileStreamWriter();

		bool IsStreamGood() const final { return m_Stream.good(); }
		size_t GetStreamPosition() final { return m_Stream.tellp(); }
		void SetStreamPosition(size_t position) final { m_Stream.seekp(position); }
		bool WriteData(const char *data, size_t size) final;

	private:
		std::filesystem::path m_Path;
		std::ofstream m_Stream;
	};

	class FileStreamReader : public StreamReader {
		FileStreamReader(const std::filesystem::path &path);
		FileStreamReader(const FileStreamReader&) = delete;

		virtual ~FileStreamReader();

		bool IsStreamGood() const final { return m_Stream.good(); }
		size_t GetStreamPosition() final { return m_Stream.tellg(); }
		void SetStreamPosition(size_t position) final { m_Stream.seekg(position); }
		bool ReadData(char *destination, size_t size) final;

	private:
		std::filesystem::path m_Path;
		std::ifstream m_Stream;
	};
}