#pragma once
#include "Engine/Asset/Asset.h"
#include "Engine/Utils/StdUtils.h"

#include <unordered_map>
#include <any>

namespace Engine {
	struct AssetMetadata {
		AssetType Type = AssetType::None;
		std::filesystem::path ImportPath;
		uint64_t TimeStamp = 0;
		std::unordered_map<std::string, std::any, TransparentStringHash, TransparentStringEqual> UserVariables;

		template<typename T>
		T GetOr (std::string_view index, const T& defaultValue) const {
			const auto it = UserVariables.find(index);
			if (it != UserVariables.end())
				return std::any_cast<T>(it->second);

			return defaultValue;
		}

		template<typename T>
		T Get(std::string_view index) const {
			const auto it = UserVariables.find(index);
			if (it != UserVariables.end())
				return std::any_cast<T>(it->second);

			throw std::out_of_range("Key not found in UserVariables");
		}

		std::any& At(std::string_view index) {
			const auto it = UserVariables.find(index);
			if (it != UserVariables.end())
				return it->second;

			throw std::out_of_range("Key not found in UserVariables");
		}

		std::any& operator[](std::string_view index) {
			return At(index);
		}

		std::any At(std::string_view index) const {
			const auto it = UserVariables.find(index);
			if (it != UserVariables.end())
				return it->second;

			throw std::out_of_range("Key not found in UserVariables");
		}

		std::any operator[](std::string_view index) const {
			return At(index);
		}

		operator bool() const { return Type != AssetType::None; }
	};
}