#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"
#include "Engine/Serializer/StreamConfig.h"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cstdint>

namespace Engine {
	class StreamReader {
	public:
		virtual ~StreamReader() = default;

		virtual bool IsStreamGood() const = 0;
		virtual std::size_t GetStreamPosition() const = 0;
		virtual void SetStreamPosition(std::size_t position) = 0;
		virtual bool ReadData(std::byte *destination, Stream::SizeType size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool ReadBuffer(Buffer &buffer, Stream::BufferSize size);
		bool ReadBuffer(Buffer& buffer);
		bool ReadString(std::string &string);

		template<typename T>
		bool ReadRaw(T &type) {
			static_assert(std::is_trivially_copyable_v<T>, "StreamReader::ReadRaw<T> requires T to be trivially copyable");
			bool success = ReadData(reinterpret_cast<std::byte *>(&type), sizeof(T));
			ENGINE_ASSERT(success);
			return success;
		}

		template<typename T>
		bool ReadObject(T &object) {
			return T::Deserialize(*this, object);
		}

		template<typename Key, typename Value>
		bool ReadMap(std::map<Key, Value> &map, Stream::MapSize size);

		template<typename Key, typename Value>
		bool ReadMap(std::map<Key, Value>& map);

		template<typename Key, typename Value>
		bool ReadMap(std::unordered_map<Key, Value> &map, Stream::MapSize size);

		template <typename Key, typename Value>
		bool ReadMap(std::unordered_map<Key, Value>& map);

		template<typename T>
		bool ReadArray(std::vector<T> &array, Stream::ArraySize size);

		template <typename T>
		bool ReadArray(std::vector<T>& array);
	};

	template <typename Key, typename Value>
	bool StreamReader::ReadMap(std::map<Key, Value>& map, Stream::MapSize size) {
		for (Stream::MapSize i = 0; i < size; ++i) {
			Key key;
			if constexpr (std::is_same_v<Key, std::string>) {
				if (!ReadString(key)) {
					return false;
				}
			} else if constexpr (std::is_trivially_copyable_v<Key>) {
				if (!ReadRaw<Key>(key)) {
					return false;
				}
			}
			else {
				if (!ReadObject<Key>(key)) {
					return false;
				}
			}

			if constexpr (std::is_same_v<Value, std::string>) {
				if (!ReadString(map[key])) {
					return false;
				}
			} else if constexpr (std::is_trivially_copyable_v<Value>) {
				if (!ReadRaw<Value>(map[key])) {
					return false;
				}
			}
			else {
				if (!ReadObject<Value>(map[key])) {
					return false;
				}
			};
		}

		return true;
	}

	template <typename Key, typename Value>
	bool StreamReader::ReadMap(std::map<Key, Value>& map) {
		Stream::MapSize size = 0;
		if (!ReadRaw<Stream::MapSize>(size)) {
			return false;
		}

		return ReadMap<Key, Value>(map, size);
	}

	template <typename Key, typename Value>
	bool StreamReader::ReadMap(std::unordered_map<Key, Value>& map, Stream::MapSize size) {
		for (Stream::MapSize i = 0; i < size; ++i) {
			Key key;
			if constexpr (std::is_same_v<Key, std::string>) {
				if (!ReadString(key)) {
					return false;
				}
			}
			else if constexpr (std::is_trivially_copyable_v<Key>) {
				if (!ReadRaw<Key>(key)) {
					return false;
				}
			}
			else {
				if (!ReadObject<Key>(key)) {
					return false;
				}
			}

			if constexpr (std::is_same_v<Value, std::string>) {
				if (!ReadString(map[key])) {
					return false;
				}
			}
			else if constexpr (std::is_trivially_copyable_v<Value>) {
				if (!ReadRaw<Value>(map[key])) {
					return false;
				}
			}
			else {
				if (!ReadObject<Value>(map[key])) {
					return false;
				}
			};
		}

		return true;
	}

	template <typename Key, typename Value>
	bool StreamReader::ReadMap(std::unordered_map<Key, Value>& map) {
		Stream::MapSize size = 0;
		if (!ReadRaw<Stream::MapSize>(size)) {
			return false;
		}

		return ReadMap<Key, Value>(map, size);
	}

	template <typename T>
	bool StreamReader::ReadArray(std::vector<T>& array, Stream::ArraySize size) {
		array.resize(size);

		for (Stream::ArraySize i = 0; i < size; ++i) {
			bool success = true;
			if constexpr (std::is_same_v<T, std::string>) {
				success = ReadString(array[i]);
			} else if constexpr (std::is_trivially_copyable_v<T>) {
				success = ReadRaw<T>(array[i]);
			} else {
				success = ReadObject<T>(array[i]);
			}

			if (!success)
				return false;
		}

		return true;
	}

	template <typename T>
	bool StreamReader::ReadArray(std::vector<T>& array) {
		Stream::ArraySize size = 0;

		if (!ReadRaw<Stream::ArraySize>(size))
			return false;

		return ReadArray<T>(array, size);
	}
}
