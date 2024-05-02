#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"

#include <string>
#include <map>
#include <unordered_map>

namespace Engine {
	class StreamWriter {
	public:
		virtual ~StreamWriter() = default;

		virtual bool IsStreamGood() const = 0;
		virtual size_t GetStreamPosition() = 0;
		virtual void SetStreamPosition(size_t position) = 0;
		virtual bool WriteData(const char *data, size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		void WriteBuffer(Buffer buffer, bool writeSize = true);
		void WriteZero(size_t size);
		void WriteString(const std::string &string);
		void WriteString(std::string_view string);

		template<typename T>
		void WriteRaw(const T &type) {
			bool success = WriteData(reinterpret_cast<const char *>(&type), sizeof(T));
			ENGINE_ASSERT(success);
		}

		template<typename T>
		void WriteObject(const T& object) {
			T::Serialize(this, object);
		}

		template<typename Key, typename Value>
		void WriteMap(const std::map<Key, Value> &map, bool writeSize = true) {
			if (writeSize)
				WriteRaw<uint32_t>(static_cast<uint32_t>(map.size()));

			for (const auto &[key, value] : map) {
				if constexpr (std::is_trivial<Key>())
					WriteRaw<Key>(key);
				else
					WriteObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename Key, typename Value>
		void WriteMap(const std::unordered_map<Key, Value> &map, bool writeSize = true) {
			if (writeSize)
				WriteRaw<uint32_t>(static_cast<uint32_t>(map.size()));

			for (const auto &[key, value] : map) {
				if constexpr (std::is_trivial<Key>())
					WriteRaw<Key>(key);
				else
					WriteObject<Key>(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename Value>
		void WriteMap(const std::unordered_map<std::string, Value> &map, bool writeSize = true) {
			if (writeSize)
				WriteRaw<uint32_t>(static_cast<uint32_t>(map.size()));

			for (const auto &[key, value] : map) {
				WriteString(key);

				if constexpr (std::is_trivial<Value>())
					WriteRaw<Value>(value);
				else
					WriteObject<Value>(value);
			}
		}

		template<typename T>
		void WriteArray(const std::vector<T> &array, bool writeSize = true) {
			if (writeSize)
				WriteRaw<uint32_t>(static_cast<uint32_t>(array.size()));

			for (const auto &element : array) {
				if constexpr (std::is_trivial<T>())
					WriteRaw<T>(element);
				else
					WriteObject<T>(element);
			}
		}

		template<>
		void WriteArray(const std::vector<std::string> &array, bool writeSize) {
			if (writeSize)
				WriteRaw<uint32_t>(static_cast<uint32_t>(array.size()));

			for (const auto &element : array)
				WriteString(element);
		}
	};
}