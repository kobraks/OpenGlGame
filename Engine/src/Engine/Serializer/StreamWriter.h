#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Serializer/StreamConfig.h"

#include <string>
#include <string_view>
#include <map>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cstdint>

namespace Engine {
	class StreamWriter {
	public:
		virtual ~StreamWriter() = default;

		virtual void Flush() {}
		virtual bool IsStreamGood() const = 0;
		virtual std::size_t GetStreamPosition() const = 0;
		virtual void SetStreamPosition(std::size_t position) = 0;
		virtual bool WriteData(const std::byte *data, Stream::SizeType size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool WriteBuffer(const BufferView& buffer, bool writeSize = true);
		bool WriteFill(std::byte value, Stream::SizeType size);
		bool WriteZero(Stream::SizeType size) { return WriteFill(std::byte{ 0 }, size); }
		bool WriteString(const std::string &string);
		bool WriteString(std::string_view string);

		template<typename T>
		bool WriteRaw(const T &type) {
			static_assert(std::is_trivially_copyable_v<T>, "StreamWriter::WriteRaw<T> requires T to be trivially copyable");
			bool success = WriteData(reinterpret_cast<const std::byte *>(&type), sizeof(T));
			ENGINE_ASSERT(success);
			return success;
		}

		template<typename T>
		bool WriteObject(const T& object) {
			return T::Serialize(*this, object);
		}

		template<typename Key, typename Value>
		bool WriteMap(const std::map<Key, Value> &map, bool writeSize = true);

		template<typename Key, typename Value>
		bool WriteMap(const std::unordered_map<Key, Value> &map, bool writeSize = true);

		template<typename T>
		bool WriteArray(const std::vector<T> &array, bool writeSize = true);
	};

	template <typename Key, typename Value>
	bool StreamWriter::WriteMap(const std::map<Key, Value>& map, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::MapSize>(static_cast<Stream::MapSize>(map.size())))
			return false;

		for (const auto &[key, value] : map) {
			if constexpr (std::is_same_v<Key, std::string> || std::is_same_v<Key, std::string_view>) {
				if (!WriteString(key))
					return false;
			} else if constexpr (std::is_trivially_copyable_v<Key>) {
				if (!WriteRaw<Key>(key))
					return false;
			} else {
				if (!WriteObject<Key>(key))
					return false;
			}

			if constexpr (std::is_same_v<Value, std::string> || std::is_same_v<Value, std::string_view>) {
				if (!WriteString(value))
					return false;
			} else if constexpr (std::is_trivially_copyable_v<Value>) {
				if (!WriteRaw<Value>(value))
					return false;
			}
			else {
				if (!WriteObject<Value>(value))
					return false;
			}
		}

		return true;
	}

	template <typename Key, typename Value>
	bool StreamWriter::WriteMap(const std::unordered_map<Key, Value>& map, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::MapSize>(static_cast<Stream::MapSize>(map.size())))
			return false;

		for (const auto& [key, value] : map) {
			if constexpr (std::is_same_v<Key, std::string> || std::is_same_v<Key, std::string_view>) {
				if (!WriteString(key))
					return false;
			}
			else if constexpr (std::is_trivially_copyable_v<Key>) {
				if (!WriteRaw<Key>(key))
					return false;
			}
			else {
				if (!WriteObject<Key>(key))
					return false;
			}

			if constexpr (std::is_same_v<Value, std::string> || std::is_same_v<Value, std::string_view>) {
				if (!WriteString(value))
					return false;
			}
			else if constexpr (std::is_trivially_copyable_v<Value>) {
				if (!WriteRaw<Value>(value))
					return false;
			}
			else {
				if (!WriteObject<Value>(value))
					return false;
			}
		}

		return true;
	}

	template <typename T>
	bool StreamWriter::WriteArray(const std::vector<T>& array, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::ArraySize>(static_cast<Stream::ArraySize>(array.size())))
			return false;

		for (const auto &element : array) {
			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
				if (!WriteString(element))
					return false;
			} else if constexpr (std::is_trivially_copyable_v<T>) {
				if (!WriteRaw<T>(element))
					return false;
			} else {
				if (!WriteObject<T>(element))
					return false;
			}
		}

		return true;
	}
}
