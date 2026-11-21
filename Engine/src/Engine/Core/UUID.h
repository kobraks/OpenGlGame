#pragma once
#include "Engine/Core/Base.h"

#include <boost/uuid/uuid.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace Engine {
	class UUID {
	public:
		using Type = boost::uuids::uuid;

		constexpr UUID() noexcept = default;
		constexpr explicit UUID(Type identifier) noexcept : m_ID(identifier) {}

		[[nodiscard]] static UUID Generate();
		[[nodiscard]] static UUID FromName(std::string_view name);
		[[nodiscard]] static UUID FromName(std::string_view name, const UUID& namespaceID);

		[[nodiscard]] static UUID Parse(std::string_view text);
		[[nodiscard]] static std::optional<UUID> TryParse(std::string_view text) noexcept;

		[[nodiscard]] constexpr static UUID Invalid() noexcept { return UUID{}; }
		[[nodiscard]] constexpr bool IsValid() const noexcept { return !m_ID.is_nil(); }
		[[nodiscard]] std::string ToString() const;
		[[nodiscard]] constexpr const Type& Value() const noexcept { return m_ID; };

		[[nodiscard]] constexpr bool operator==(const UUID& other) const noexcept = default;
		[[nodiscard]] constexpr std::strong_ordering operator<=>(const UUID& other) const noexcept = default;

	private:
		Type m_ID{};
	};
}

template<>
struct fmt::formatter<Engine::UUID> {
	bool ShowBase = false;
	bool Hex = false;
	bool Upper = false;

	constexpr auto parse(fmt::format_parse_context& context) {
		auto iterator = context.begin();
		const auto end = context.end();

		if (iterator == end || *iterator == '}')
			return iterator;

		if (*iterator == '#') {
			ShowBase = true;
			++iterator;
		}

		if (iterator != end && (*iterator == 'x' || *iterator == 'X')) {
			Hex = true;
			Upper = (*iterator == 'X');
			++iterator;
		} else {
			throw fmt::format_error("Invalid UUID format specifier");
		}

		if (iterator != end && *iterator != '}')
			throw fmt::format_error("Invalid UUID format specifier");

		return iterator;
	}

	template<typename FormatContext>
	auto format(const Engine::UUID& identifier, FormatContext& context) const {
		if (!Hex)
			return fmt::format_to(context.out(), "{}", identifier.ToString());

		constexpr std::string_view lowerDigits = "0123456789abcdef";
		constexpr std::string_view upperDigits = "0123456789ABCDEF";
		const std::string_view digits = Upper ? upperDigits : lowerDigits;

		std::array<char, 32> hexDigits{};
		std::size_t length = 0;
		bool hasNonzeroDigit = false;

		for (const std::uint8_t byte : identifier.Value()) {
			for (const std::uint8_t nibble : {static_cast<std::uint8_t>(byte >> 4), static_cast<std::uint8_t>(byte & 0x0F)}) {
				if (nibble != 0)
					hasNonzeroDigit = true;
				if (hasNonzeroDigit)
					hexDigits[length++] = digits[nibble];
			}
		}

		if (!hasNonzeroDigit)
			hexDigits[length++] = '0';

		if (ShowBase && hasNonzeroDigit)
			return fmt::format_to(context.out(), "0x{}", std::string_view(hexDigits.data(), length));

		return fmt::format_to(context.out(), "{}", std::string_view(hexDigits.data(), length));
	}
};

namespace std {
	template <>
	struct hash<Engine::UUID> {
		std::size_t operator()(const Engine::UUID& identifier) const noexcept {
			return boost::uuids::hash_value(identifier.Value());
		}
	};
}