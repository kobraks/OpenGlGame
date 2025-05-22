#include "pch.h"
#include "StreamReader.h"

namespace Engine {
	bool StreamReader::ReadBuffer(Buffer &buffer, size_t size) {
		uint32_t readSize = static_cast<uint32_t>(size);

		if(size == 0) {
			if(!ReadData(reinterpret_cast<char*>(readSize), sizeof(size_t)))
				return false;
		}

		buffer.Allocate(readSize);
		return ReadData(buffer.As<char>(), buffer.Size());
	}

	bool StreamReader::ReadString(std::string &string) {
		size_t size;
		if (!ReadData(reinterpret_cast<char *>(&size), sizeof(size_t)))
			return false;

		string.resize(size);
		return ReadData(string.data(), sizeof(std::string::value_type) * size); 
	}
}
