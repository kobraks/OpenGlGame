#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/BufferView.h"

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
		virtual bool WriteData(const std::byte *data, std::size_t size) = 0;

		operator bool() const { return IsStreamGood(); }

		bool WriteBuffer(const BufferView& buffer, bool writeSize = true);
		bool WriteFill(std::byte value, std::size_t size);
		bool WriteZero(std::size_t size) { return WriteFill(std::byte{ 0 }, size); }
		bool WriteString(const std::string &string);
		bool WriteString(std::string_view string);

		template<typename T>
		bool WriteRaw(const T &type) {
			static_assert(std::is_trivially_copyable_v<T>, "StreamWriter::ReadWrite<T> requires T to be trivially copyable");
			bool success = WriteData(reinterpret_cast<const std::byte *>(&type), sizeof(T));
			ENGINE_ASSERT(success);
			return success;
		}

		template<typename T>
		bool WriteObject(const T& object) {
			return T::Serialize(*this, object);
		}

		template<typename Key, typename Value>
		bool WriteMap(const std::map<Key, Value> &map, bool writeSize = true) {
			if (writeSize && !WriteRaw<uint32_t>(static_cast<uint32_t>(map.size())))
				return false;

			for (const auto &[key, value] : map) {
				bool success = true;
				if constexpr (std::is_trivially_copyable_v<Key>)
					success = WriteRaw<Key>(key);
				else
					success = WriteObject<Key>(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = WriteRaw<Value>(value);
				else
					success = WriteObject<Value>(value);


				if (!success)
					return false;
			}

			return true;
		}

		template<typename Key, typename Value>
		bool WriteMap(const std::unordered_map<Key, Value> &map, bool writeSize = true) {
			if (writeSize && !WriteRaw<uint32_t>(static_cast<uint32_t>(map.size())))
				return false;

			for (const auto &[key, value] : map) {
				bool success = true;
				if constexpr (std::is_trivially_copyable_v<Key>)
					success = WriteRaw<Key>(key);
				else
					success = WriteObject<Key>(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = WriteRaw<Value>(value);
				else
					success = WriteObject<Value>(value);

				if (!success)
					return false;
			}

			return true;
		}

		template<typename Value>
		bool WriteMap(const std::unordered_map<std::string, Value> &map, bool writeSize = true) {
			if (writeSize && !WriteRaw<uint32_t>(static_cast<uint32_t>(map.size())))
				return false;

			for (const auto &[key, value] : map) {
				bool success = WriteString(key);

				if (!success)
					return false;

				if constexpr (std::is_trivially_copyable_v<Value>)
					success = WriteRaw<Value>(value);
				else
					success = WriteObject<Value>(value);

				if (!success)
					return false;
			}

			return true;
		}

		template<typename T>
		bool WriteArray(const std::vector<T> &array, bool writeSize = true) {
			if (writeSize && !WriteRaw<uint32_t>(static_cast<uint32_t>(array.size())))
				return false;

			for (const auto &element : array) {
				bool success = false;

				if constexpr (std::is_same_v<T, std::string>) {
					success = WriteString(element);
				} else if constexpr (std::is_trivially_copyable_v<T>) {
					success = WriteRaw<T>(element);
				} else {
					success = WriteObject<T>(element);
				}

				if (!success)
					return false;
			}

			return true;
		}
	};
}