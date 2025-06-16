#include "pch.h"
#include "LogUtils.h"

#include "Engine/Utils/StdUtils.h"

#include <fmt/chrono.h>

namespace Engine::Utils {
	static constexpr std::array<std::string_view, 6> s_Severities = { "Trace", "Debug", "Info", "Warn", "Error", "Critical" };
	static constexpr std::array<std::string_view, 4> s_KnownLoggers = { APPLICATION_LOGGER_NAME, ENGINE_LOGGER_NAME, GL_LOGGER_NAME, SCRIPT_LOGGER_NAME };
	static std::unordered_map<std::string_view, int32_t, TransparentStringHash, TransparentStringEqual> s_LoggerNameToId;

	const std::array<std::string_view, 6>& GetSeverities() {
		return s_Severities;
	}

	const std::array<std::string_view, 4>& GetKnownLoggersName() {
		return s_KnownLoggers;
	}

	int32_t GetLoggerID(std::string_view loggerName) {
		auto it = s_LoggerNameToId.find(loggerName);

		if (it == s_LoggerNameToId.end()) {
			for (int32_t i = 0; i < static_cast<int32_t>(s_KnownLoggers.size()); ++i) {
				if (s_KnownLoggers[i] == loggerName) {
					s_LoggerNameToId.emplace(s_KnownLoggers[i], i);
					return i;
				}
			}

			s_LoggerNameToId.emplace(s_KnownLoggers[0], 0);
			return 0;
		}

		return it->second;
	}

	std::string GetTimeAsString(const spdlog::log_clock::time_point& time) {
		return fmt::format("{:%T}", std::chrono::round<std::chrono::seconds>(time));
	}

	bool FilterChanged(const std::string& filter, std::string& lastFilter) {
		if (filter != lastFilter) {
			lastFilter = filter;
			return true;
		}

		return false;
	}

	bool FilterChanged(int32_t filter, int32_t& lastFilter) {
		if (filter != lastFilter) {
			lastFilter = filter;
			return true;
		}

		return false;
	}

	bool FilterChanged(const ImGuiTextFilter& filter, std::string& lastFilter) {
		if (filter.InputBuf != lastFilter) {
			lastFilter = filter.InputBuf;
			return true;
		}

		return false;
	}
}
