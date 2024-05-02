#include "pch.h"
#include "StreamReader.h"

namespace Engine {
	bool StreamReader::ReadBuffer(Buffer &buffer, size_t size) {
		buffer.Size = size;

		if(size == 0) {
			if(!ReadData(reinterpret_cast<char*>(buffer.Size), sizeof(size_t)))
				return false;
		}

		buffer.Allocate(buffer.Size);
		return ReadData(static_cast<char *>(buffer.Data), buffer.Size);
	}

	bool StreamReader::ReadString(std::string &string) {
		size_t size;
		if (!ReadData(reinterpret_cast<char *>(&size), sizeof(size_t)))
			return false;

		string.resize(size);
		return ReadData(string.data(), sizeof(std::string::value_type) * size); 
	}
}
