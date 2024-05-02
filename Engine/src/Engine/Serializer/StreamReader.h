#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"

#include <string>
#include <map>
#include <unordered_map>

namespace Engine {
	class StreamReader {
	public:
		virtual ~StreamReader() = default;

		virtual bool IsStreamGood() const = 0;
		virtual size_t GetStreamPosition() = 0;
		virtual void SetStreamPosition(size_t position) = 0;
		virtual bool ReadData(char *destination, size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool ReadBuffer(Buffer &buffer, size_t size = 0);
		bool ReadString(std::string &string);

		template<typename T>
		bool ReadRaw(T &type) {
			bool success = ReadData(reinterpret_cast<char *>(&type), sizeof(T));
			ENGINE_ASSERT(success);
			return success;
		}

		template<typename T>
		void ReadObject(T &object) {
			T::Deserialize(this, object);
		}

		template<typename Key, typename Value>
		void ReadMap(std::map<Key, Value> &map, uint32_t size = 0) {
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; ++i) {
				Key key;
				if constexpr (std::is_trivial<Key>())
					ReadRaw<Key>(key);
				else
					ReadObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename Key, typename Value>
		void ReadMap(std::unordered_map<Key, Value> &map, uint32_t size = 0) {
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; ++i) {
				Key key;
				if constexpr (std::is_trivial<Key>())
					ReadRaw<Key>(key);
				else
					ReadObject<Key>(key);

				if constexpr(std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename Value>
		void ReadMap(std::unordered_map<std::string, Value> &map, uint32_t size = 0) {
			if (size == 0)
				ReadRaw<uint32_t>(size);

			for (uint32_t i = 0; i < size; ++i) {
				std::string key;
				ReadString(key);

				if constexpr (std::is_trivial<Value>())
					ReadRaw<Value>(map[key]);
				else
					ReadObject<Value>(map[key]);
			}
		}

		template<typename T>
		void ReadArray(std::vector<T> &array, uint32_t size = 0) {
			if (size == 0)
				ReadRaw<uint32_t>(size);

			array.resize(size);

			for (uint32_t i = 0; i < size; ++i) {
				if constexpr(std::is_trivial<T>())
					ReadRaw<T>(array[i]);
				else
					ReadObject<T>(array[i]);
			}
		}

		template<>
		void ReadArray(std::vector<std::string> &array, uint32_t size) {
			if (size == 0)
				ReadRaw<uint32_t>(size);

			array.resize(size);

			for (uint32_t i = 0; i < size; ++i)
				ReadString(array[i]);
		}
	};
}