#include "pch.h"
#include "Engine/Renderer/ShaderSource.h"

#include <boost/regex.hpp>

namespace Engine {
	ShaderSource ShaderSource::Load(std::filesystem::path path) {
		std::unordered_set<std::string> files;
		files.emplace(path.string());

		return ShaderSource(Parse(GetFileContent(path), files));
	}

	ShaderSource::ShaderSource(std::string source) : m_Source(std::move(source)) {}

	void ShaderSource::Source(const std::string &source) {
		m_Source = source;
	}

	std::stringstream ShaderSource::GetFileContent(std::filesystem::path path) {
		const auto name = path.string();

		LOG_ENGINE_DEBUG("Loading file: \"{}\"", name);
		std::ifstream file(path, std::ios::in);

		if (!file.good()) {
			LOG_ENGINE_ERROR("Unable to open and fetch content from: \"{}\"", name);
			return std::stringstream{};
		}

		std::stringstream stream;
		stream << file.rdbuf();

		return stream;
	}

	std::string ShaderSource::Parse(std::stringstream stream, std::unordered_set<std::string> &files) {
		std::stringstream result;

		for (std::string line; std::getline(stream, line);) {
			if (IsInclude(line)) {
				result << ParseInclude(GetInclude(line), files);
			}
			else
				result << line;
		}

		return result.str();
	}

	std::string ShaderSource::ParseInclude(std::string_view fileName, std::unordered_set<std::string> &files) {
		auto filePath = std::filesystem::path(fileName);

		if (files.contains(filePath.string())) {
			LOG_ENGINE_WARN("File already included: \"{}\"", fileName);
			return "";
		}

		LOG_ENGINE_DEBUG("Including file: \"{}\"", fileName);
		return Parse(GetFileContent(filePath), files);
	}

	bool ShaderSource::IsInclude(std::string_view string) {
		return false;
	}

	std::string_view ShaderSource::GetInclude(std::string_view string) {
		const auto begin = string.begin() + static_cast<long long>(string.find_first_of('"'));
		const auto end = begin + static_cast<long long>(string.find_last_of('"'));

		return {begin, end};
	}
}
