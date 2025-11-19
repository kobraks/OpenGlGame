#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"

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
		virtual bool ReadData(std::byte *destination, std::size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool ReadBuffer(Buffer &buffer, Buffer::SizeType size = 0);
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
		bool ReadMap(std::map<Key, Value> &map, uint32_t size = 0) {
			if (size == 0 && !ReadRaw<uint32_t>(size))
				return false;

			for (uint32_t i = 0; i < size; ++i) {
				bool success = true;
				Key key;
				if constexpr (std::is_trivially_copyable_v<Key>)
					success = ReadRaw<Key>(key);
				else
					success = ReadObject<Key>(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = ReadRaw<Value>(map[key]);
				else
					success = ReadObject<Value>(map[key]);

				if (!success)
					return false;
			}

			return true;
		}

		template<typename Key, typename Value>
		bool ReadMap(std::unordered_map<Key, Value> &map, uint32_t size = 0) {
			if (size == 0 && !ReadRaw<uint32_t>(size))
				return false;

			for (uint32_t i = 0; i < size; ++i) {
				bool success = true;
				Key key;
				if constexpr (std::is_trivially_copyable_v<Key>)
					success = ReadRaw<Key>(key);
				else
					success = ReadObject<Key>(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = ReadRaw<Value>(map[key]);
				else
					success = ReadObject<Value>(map[key]);

				if (!success)
					return false;
			}

			return true;
		}

		template<typename Value>
		bool ReadMap(std::unordered_map<std::string, Value> &map, uint32_t size = 0) {
			if (size == 0 && !ReadRaw<uint32_t>(size))
				return false;

			for (uint32_t i = 0; i < size; ++i) {
				std::string key;
				bool success = ReadString(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = ReadRaw<Value>(map[key]);
				else
					success = ReadObject<Value>(map[key]);

				if (!success)
					return false;
			}

			return true;
		}

		template<typename T>
		bool ReadArray(std::vector<T> &array, uint32_t size = 0) {
			if (size == 0 && !ReadRaw<uint32_t>(size))
				return false;

			array.resize(size);

			for (uint32_t i = 0; i < size; ++i) {
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
	};
}