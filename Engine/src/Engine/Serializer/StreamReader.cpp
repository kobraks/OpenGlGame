#include "pch.h"
#include "StreamReader.h"

namespace Engine {
	bool StreamReader::ReadBuffer(Buffer &buffer, Stream::BufferSize size) {
		buffer.Allocate(static_cast<BufferAccessorBase<Buffer>::SizeType>(size));
		return ReadData(buffer.Data(), buffer.Size());
	}

	bool StreamReader::ReadBuffer(Buffer& buffer) {
		Stream::BufferSize size = 0;
		if (!ReadRaw<Stream::BufferSize>(size))
			return false;

		return ReadBuffer(buffer, size);
	}

	bool StreamReader::ReadString(std::string &string) {
		Stream::StringSize size = 0;
		if (!ReadRaw<Stream::StringSize>(size))
			return false;

		string.resize(size);
		return ReadData(reinterpret_cast<std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}
}
