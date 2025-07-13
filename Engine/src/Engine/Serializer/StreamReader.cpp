#include "pch.h"
#include "StreamReader.h"

namespace Engine {
	bool StreamReader::ReadBuffer(Buffer &buffer, Buffer::SizeType size) {
		if(size == 0) {
			if(!ReadData(reinterpret_cast<std::byte*>(&size), sizeof(Buffer::SizeType)))
				return false;
		}

		buffer.Allocate(size);
		return ReadData(buffer.Data(), buffer.Size());
	}

	bool StreamReader::ReadString(std::string &string) {
		std::size_t size;
		if (!ReadData(reinterpret_cast<std::byte *>(&size), sizeof(std::size_t)))
			return false;

		string.resize(size);
		return ReadData(reinterpret_cast<std::byte*>(string.data()), sizeof(std::string::value_type) * size);
	}
}
