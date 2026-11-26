#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"

#include "Engine/Serializer/StreamConfig.h"

#include "Engine/Serializer/Serializers/Serializer.h"
#include "Engine/Serializer/SerializerConcepts.h"

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cstdint>
#include <limits>
#include <cstddef>
#include <concepts>

namespace Engine {
	class StreamReader {
	public:
		virtual ~StreamReader() = default;

		virtual bool IsStreamGood() const = 0;
		virtual std::size_t GetStreamPosition() const = 0;
		virtual void SetStreamPosition(std::size_t position) = 0;
		virtual bool ReadData(std::byte *destination, Stream::SizeType size) = 0;

		explicit operator bool() const { return IsStreamGood(); }

		bool ReadBuffer(Buffer &buffer, Stream::BufferSize size);
		bool ReadBuffer(Buffer& buffer);
		bool ReadString(std::string &string);

		template<typename T>
		requires(std::is_trivially_copyable_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> && !std::same_as<T, std::byte>)
		bool ReadData(T* destination, std::size_t count) {
			constexpr auto maxSize = std::numeric_limits<Stream::SizeType>::max();

			if (count > maxSize / sizeof(T))
				return false;

			if (count == 0)
				return true;

			if (!destination)
				return false;

			return ReadData(reinterpret_cast<std::byte*>(destination), static_cast<Stream::SizeType>(count * sizeof(T)));
		}

		template<Serialization::RawCopyable T>
		bool ReadRaw(T &value) {
			return ReadData(reinterpret_cast<std::byte*>(&value), sizeof(T));
		}

		template<typename T>
		bool ReadObject(T &object) {
			using Serializer = typename Serializer<T>::Type;
			return Serializer::Deserialize(*this, object);
		}

		template <typename T>
		bool ReadValue(T& value);

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

		template <typename T, std::size_t N>
		bool ReadArray(std::array<T, N>& array);
	};

	template <typename T>
	bool StreamReader::ReadValue(T& value) {
		if constexpr (std::is_same_v<T, bool>) {
			uint8_t data{};
			if (!ReadRaw(data) || data > 1)
				return false;

			value = data != 0;
		}
		else if constexpr (Serialization::Scalar<T>) {
			T temp{};

			if (!ReadRaw(temp)) {
				return false;
			}

			value = temp;
		} else if constexpr (Serialization::Enum<T>) {
			using U = std::underlying_type_t<T>;

			U data{};

			if (!ReadValue(data))
				return false;

			value = static_cast<T>(data);
			return true;
		} else if constexpr (std::same_as<T, std::string>) {
			std::string temp;

			if (!ReadString(temp))
				return false;

			value = std::move(temp);
		} else {
			return ReadObject(value);
		}

		return true;
	}

	template <typename Key, typename Value>
	bool StreamReader::ReadMap(std::map<Key, Value>& map, Stream::MapSize size) {
		for (Stream::MapSize i = 0; i < size; ++i) {
			Key key {};
			if (!ReadValue(key))
				return false;

			if (!ReadValue(map[key]))
				return false;
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
			Key key{};
			if (!ReadValue(key)) {
				return false;
			}

			if (!ReadValue(map[key]))
				return false;
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
			if (!ReadValue(array[i]))
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

	template <typename T, std::size_t N>
	bool StreamReader::ReadArray(std::array<T, N>& array) {
		std::array<T, N> temp{};

		for (auto& element : temp) {
			if (!ReadValue(element))
				return false;
		}

		array = std::move(temp);
		return true;
	}
}
