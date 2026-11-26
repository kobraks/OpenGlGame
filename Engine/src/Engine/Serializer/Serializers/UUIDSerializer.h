#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

#include "Engine/Serializer/StreamReader.h"
#include "Engine/Serializer/StreamWriter.h"

#include "Engine/Serializer/Serializers/Serializer.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace Engine {
	struct UUIDSerializer {
		static bool Serialize(StreamWriter& writer, const UUID& value) {
			std::array<std::byte, 16> bytes{};
			std::size_t i = 0;

			for (std::uint8_t part : value.Value()) {
				bytes[i++] = static_cast<std::byte>(part);
			}

			return writer.WriteData(bytes.data(), bytes.size());
		}

		static bool Deserialize(StreamReader& reader, UUID& value) {
			std::array<std::byte, 16> bytes{};
			
			if (!reader.ReadData(bytes.data(), bytes.size())) 
				return false;

			UUID::Type raw{};
			auto it = raw.begin();
			for (auto b : bytes) {
				*it++ = std::to_integer<std::uint8_t>(b);
			}

			value = UUID{ raw };
			return true;
		}
	};

	template<>
	struct Serializer<UUID> { using Type = UUIDSerializer; };
}