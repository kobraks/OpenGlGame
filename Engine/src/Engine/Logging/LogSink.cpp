#include "pch.h"
#include "LogSink.h"

#include "Engine/Logging/LogStorage.h"

namespace Engine {
	LogSink::LogSink(Ref<LogStorage> storage) : m_Storage(storage) {
	}

	void LogSink::sink_it_(const spdlog::details::log_msg& msg) {
		spdlog::memory_buf_t formatted;
		formatter_->format(msg, formatted);

		m_Storage->AddMessage(formatted, msg);
	}

	void LogSink::flush_() {
	}
}
