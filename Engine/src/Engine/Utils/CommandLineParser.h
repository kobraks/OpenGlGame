#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/ApplicationCommandLineArgs.h"

#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

namespace Engine {
	class CommandLineParser {
	public:
		CommandLineParser(const ApplicationCommandLineArgs& commandLine);

		bool Has(const std::string& key) const;
		const std::unordered_map<std::string, std::string>& GetParsedArgs() const { return m_Args; }

		size_t GetArgsCount() const { return m_Args.size(); }

		std::string Get(const std::string& key, const std::string& defaultValue = "") const;
		int GetInt(const std::string& key, int defaultValue = 0) const;
		float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
		bool GetBool(const std::string& key, bool defaultValue = false) const;

		void PrintUsage() const;
	private:
		void Parse(const ApplicationCommandLineArgs& commandLine);
		std::unordered_map<std::string, std::string> m_Args;
	};

}