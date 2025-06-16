#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Color.h"

#include <chrono>
#include <vector>

namespace Engine {
	struct LogSource {
		std::string FileName;
		std::string FunctionName;
		int LineNumber{ 0 };
		std::size_t ThreadID{ 0 };

		LogSource(const spdlog::source_loc& loc);
		LogSource(const spdlog::details::log_msg& msg);
	};

	struct LogMessage {
		std::string_view LoggerName;
		std::string Description;
		std::string FullMessage;
		std::string TimeString;

		int32_t LoggerID{ 0 };

		spdlog::level::level_enum Level{ spdlog::level::info };
		std::chrono::system_clock::time_point Timestamp;

		LogMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg);
	};

	struct LogMessageHash {
		std::size_t IDHash{ 0 };
		std::size_t IDSelectedHash{ 0 };
		std::size_t IDTextMultiline{ 0 };

		LogMessageHash(std::size_t i, LogMessage message);
	};

	struct LogMessageEntry {
		LogMessage Message;
		LogSource Source;
		LogMessageHash Hash;

		Color Color;

		uint64_t Index = 0;

		LogMessageEntry(uint64_t index, const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg);
	};
}