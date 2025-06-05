#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

#include <string>
#include <string_view>
#include <concepts>

namespace Engine {
	template <typename ... Bases>
	struct Overload : Bases ... {
		using is_transparent = void;
		using Bases::operator() ...;
	};

	struct CharPointerHash {
		auto operator()(const char *ptr) const noexcept {
			return std::hash<std::string_view>{}(ptr);
		}
	};

	struct TransparentStringEqual {
		using is_transparent = void;

		bool operator()(const std::string& lhs, const std::string& rhs) const noexcept {
			return lhs == rhs;
		}

		bool operator()(const std::string& lhs, std::string_view rhs) const noexcept {
			return lhs == rhs;
		}

		bool operator()(const std::string_view lhs, const std::string& rhs) const noexcept {
			return lhs == rhs;
		}

		bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
			return lhs == rhs;
		}

		bool operator()(std::string_view lhs, const char* rhs) const noexcept {
			return lhs == std::string_view(rhs);
		}

		bool operator()(const char* lhs, const std::string& rhs) const noexcept {
			return std::string_view(lhs) == rhs;
		}

		bool operator()(const std::string& lhs, const char* rhs) const noexcept {
			return lhs == std::string_view(rhs);
		}

		bool operator()(const char* lhs, const char* rhs) const noexcept {
			return std::strcmp(lhs, rhs);
		}

		bool operator()(const char* lhs, std::string_view rhs) const noexcept {
			return std::string_view(lhs) == rhs;
		}
	};

	using TransparentStringHash = Overload<std::hash<std::string>, std::hash<std::string_view>, CharPointerHash>;

	namespace Utils {
		template <typename T>
		concept HasCStrLike = requires(const T& t) {
			{ t.c_str() } -> std::convertible_to<const char*>;
			{ t.data() } -> std::convertible_to<const char*>;
		};

		const char* EnsureNullTerminated(std::string_view view, std::string& tmpStorage);
		const char* EnsureNullTerminated(std::string_view view);
	}
}