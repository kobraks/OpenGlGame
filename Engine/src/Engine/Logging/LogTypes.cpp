#include "pch.h"
#include "LogTypes.h"
#include "Engine/ImGui/ImGuiWidgets.h"

#include "Engine/Logging/LogUtils.h"

#include <fmt/chrono.h>

namespace Engine {
	LogSource::LogSource(const spdlog::source_loc& loc) {
		FileName = loc.filename ? loc.filename : "unknown file)";
		FunctionName = loc.funcname ? loc.funcname : "unknown function";
		LineNumber = loc.line;
	}

	LogSource::LogSource(const spdlog::details::log_msg& msg) : LogSource(msg.source) {
		ThreadID = msg.thread_id;
	}

	LogMessage::LogMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg) {
		const std::string loggerName(msg.logger_name.data(), msg.logger_name.size());
		Description = std::string(msg.payload.data(), msg.payload.size());

		LoggerID = Utils::GetLoggerID(loggerName);
		LoggerName = Utils::GetKnownLoggersName().at(LoggerID);

		FullMessage = to_string(formatted);

		Timestamp = msg.time;
		TimeString = Utils::GetTimeAsString(msg.time);

		Level = msg.level;
	}

	LogMessageHash::LogMessageHash(std::size_t i, LogMessage message) {
		const auto timestampHash = message.Timestamp.time_since_epoch().count();
		const std::string base = fmt::format("{}:{}", timestampHash, i);

		IDHash = std::hash<std::string_view>{}(base);
		IDSelectedHash = std::hash<std::string_view>{}("S" + base);
		IDTextMultiline = std::hash<std::string_view>{}("T" + base);
	}

	LogMessageEntry::LogMessageEntry(uint64_t index, const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg) : Message(formatted, msg), Source(msg), Hash(index, Message) {
		Color = Utils::SelectTextColor(Message.Level);
		Index = index;
	}

}
