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

		void Flush() final { m_Stream.flush(); }
		bool IsStreamGood() const final { return m_Stream.good(); }
		std::size_t GetStreamPosition() const override { return static_cast<std::size_t>(m_Stream.tellp()); }
		void SetStreamPosition(std::size_t position) override { m_Stream.seekp(static_cast<std::ostream::pos_type>(position)); }
		bool WriteData(const std::byte *data, Stream::SizeType size) final;

	protected:
		inline bool StreamFailed() const;

	private:
		std::filesystem::path m_Path;
		mutable std::ofstream m_Stream;
	};

	class FileStreamReader : public StreamReader {
	public:
		FileStreamReader(const std::filesystem::path &path);
		FileStreamReader(const FileStreamReader&) = delete;

		~FileStreamReader() override;

		bool IsStreamGood() const final { return !StreamFailed(); }
		std::size_t GetStreamPosition() const override { return static_cast<std::size_t>(m_Stream.tellg()); }
		void SetStreamPosition(std::size_t position) override { m_Stream.seekg(static_cast<std::istream::pos_type>(position)); }
		bool ReadData(std::byte *destination, Stream::SizeType size) final;

	protected:
		inline bool StreamFailed() const;

	private:
		std::filesystem::path m_Path;
		mutable std::ifstream m_Stream;
	};
}