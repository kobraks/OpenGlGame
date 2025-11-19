#include "pch.h"
#include "StreamReader.h"

namespace Engine {
	bool StreamReader::ReadBuffer(Buffer &buffer, Buffer::SizeType size) {
		if(size == 0) {
			if(!ReadRaw<Buffer::SizeType>(size))
				return false;
		}

		buffer.Allocate(size);
		return ReadData(buffer.Data(), buffer.Size());
	}

	bool StreamReader::ReadString(std::string &string) {
		uint64_t size;
		if (!ReadRaw<uint64_t>(size))
			return false;

		string.resize(size);
		return ReadData(reinterpret_cast<std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}
}
