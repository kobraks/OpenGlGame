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

		~FileStreamWriter() override;

		bool IsStreamGood() const final { return m_Stream.good(); }
		std::size_t GetStreamPosition() override { return m_Stream.tellp(); }
		void SetStreamPosition(std::size_t position) override { m_Stream.seekp(position); }
		bool WriteData(const std::byte *data, std::size_t size) final;

	private:
		std::filesystem::path m_Path;
		std::ofstream m_Stream;
	};

	class FileStreamReader : public StreamReader {
	public:
		FileStreamReader(const std::filesystem::path &path);
		FileStreamReader(const FileStreamReader&) = delete;

		~FileStreamReader() override;

		bool IsStreamGood() const final { return m_Stream.good(); }
		std::size_t GetStreamPosition() override { return m_Stream.tellg(); }
		void SetStreamPosition(std::size_t position) override { m_Stream.seekg(position); }
		bool ReadData(std::byte *destination, std::size_t size) final;

	private:
		std::filesystem::path m_Path;
		std::ifstream m_Stream;
	};
}