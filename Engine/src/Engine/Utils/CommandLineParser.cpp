#include "pch.h"
#include "CommandLineParser.h"

#include <iostream>


namespace Engine {
	CommandLineParser::CommandLineParser(const ApplicationCommandLineArgs& commandLine) {
		Parse(commandLine);
	}

	void CommandLineParser::Parse(const ApplicationCommandLineArgs& commandLine) {
		for (size_t i = 0; i < commandLine.Count; ++i) {
			const auto arg = commandLine[i];

			if (arg.starts_with("--")) {
				const size_t eqPos = arg.find('=');
				if (eqPos != std::string::npos) {
					m_Args.emplace(arg.substr(2, eqPos - 2), arg.substr(eqPos + 1));
				} else {
					m_Args.emplace(arg.substr(2), "true");
				}
			} else if (arg.starts_with("-")) {
				m_Args.emplace(arg.substr(1), "true");
			}
		}
	}

	bool CommandLineParser::Has(const std::string& key) const {
		return m_Args.contains(key);
	}

	std::string CommandLineParser::Get(const std::string& key, const std::string& defaultValue) const {
		const auto it = m_Args.find(key);

		return it != m_Args.end() ? it->second : defaultValue;
	}

	int CommandLineParser::GetInt(const std::string& key, int defaultValue) const {
		const auto it = m_Args.find(key);
		if (it != m_Args.end()) {
			try {
				return std::stoi(it->second);
			} catch (...) {}
		}

		return defaultValue;
	}

	float CommandLineParser::GetFloat(const std::string& key, float defaultValue) const {
		const auto it = m_Args.find(key);
		if (it != m_Args.end()) {
			try {
				return std::stof(it->second);
			}
			catch (...) {}
		}

		return defaultValue;
	}

	bool CommandLineParser::GetBool(const std::string& key, bool defaultValue) const {
		const auto it = m_Args.find(key);
		if (it != m_Args.end()) {
			const std::string& val = it->second;
			return val == "1" || val == "true" || val == "yes";
		}

		return defaultValue;
	}

	void CommandLineParser::PrintUsage() const {
		std::cout << "Available Command Line Options:\n";
		for (const auto& [key, val] : m_Args) {
			std::cout << "  --" << key;
			if (val != "true") std::cout << "=" << val;
			std::cout << "\n";
		}
	}
}
