#pragma once
#include "Engine/Core/Base.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_set>

namespace Engine {
	class ShaderSource {
	public:
		ShaderSource() = default;
		ShaderSource(std::string source);

		static ShaderSource Load(std::filesystem::path path);

		std::string_view Source() const { return m_Source; }
		void Source(const std::string &source);

	private:
		std::string m_Source;

		static std::stringstream GetFileContent(std::filesystem::path path);
		static std::string Parse(std::stringstream stream, std::unordered_set<std::string> &files);

		static std::string ParseInclude(std::string_view fileName, std::unordered_set<std::string> &files);

		static bool IsInclude(std::string_view string);
		static std::string_view GetInclude(std::string_view string);
	};
}