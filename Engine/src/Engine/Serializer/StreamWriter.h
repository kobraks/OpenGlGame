#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Serializer/StreamConfig.h"

#include "Engine/Serializer/Serializers/Serializer.h"
#include "Engine/Serializer/SerializerConcepts.h"

#include <string>
#include <string_view>
#include <map>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cstdint>
#include <limits>
#include <cstddef>

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
		requires (std:: is_trivially_copyable_v<T> && !std::is_same_v<std::remove_cv_t<T>, std::byte>)
		bool WriteData(const T* data, std::size_t count) {
			constexpr auto maxSize = std::numeric_limits<Stream::SizeType>::max();

			if (count > maxSize / sizeof(T))
				return false;

			if (count == 0)
				return true;

			if (!data)
				return false;

			return WriteData(reinterpret_cast<std::byte*>(data), static_cast<Stream::SizeType>(count * sizeof(T)));
		}

		template<Serialization::RawCopyable T>
		bool WriteRaw(const T &value) {
			return WriteData(reinterpret_cast<const std::byte*>(&value), sizeof(T));
		}

		template<typename T>
		bool WriteObject(const T& object) {
			using Serializer = typename Serializer<T>::Type;
			return T::Serialize(*this, object);
		}

		template <typename T>
		bool WriteValue(const T& value) {
			if constexpr (std::same_as<T, bool>) {
				const uint8_t data = value ? 1 : 0;
				return WriteRaw(data);
			} else if constexpr (Serialization::Scalar<T>) {
				return WriteRaw(value);
			} else if constexpr (Serialization::Enum<T>) {
				using U = std::underlying_type_t<T>;
				return WriteRaw(static_cast<U>(value));
			} else if constexpr (std::same_as<T, std::string> || std::same_as<T, std::string_view>) {
				return WriteString(value);
			}
			else
				return WriteObject(value);
		}

		template<typename Key, typename Value>
		bool WriteMap(const std::map<Key, Value> &map, bool writeSize = true);

		template<typename Key, typename Value>
		bool WriteMap(const std::unordered_map<Key, Value> &map, bool writeSize = true);

		template<typename T>
		bool WriteArray(const std::vector<T> &array, bool writeSize = true);

		template<typename T, std::size_t N>
		bool WriteArray(const std::array<T, N>& array);
	};

	template <typename Key, typename Value>
	bool StreamWriter::WriteMap(const std::map<Key, Value>& map, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::MapSize>(static_cast<Stream::MapSize>(map.size())))
			return false;

		for (const auto &[key, value] : map) {
			WriteValue(key);
			WriteValue(value);
		}

		return true;
	}

	template <typename Key, typename Value>
	bool StreamWriter::WriteMap(const std::unordered_map<Key, Value>& map, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::MapSize>(static_cast<Stream::MapSize>(map.size())))
			return false;

		for (const auto& [key, value] : map) {
			WriteValue(key);
			WriteValue(value);
		}

		return true;
	}

	template <typename T>
	bool StreamWriter::WriteArray(const std::vector<T>& array, bool writeSize) {
		if (writeSize && !WriteRaw<Stream::ArraySize>(static_cast<Stream::ArraySize>(array.size())))
			return false;

		for (const auto &element : array) {
			WriteValue(element);
		}

		return true;
	}

	template <typename T, std::size_t N>
	bool StreamWriter::WriteArray(const std::array<T, N>& array) {
		for (auto element : array) {
			if (!WriteValue(element))
				return false;
		}

		return true;
	}
}
