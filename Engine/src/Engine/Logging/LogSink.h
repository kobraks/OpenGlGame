#pragma once
#include "Engine/Core/Base.h"

#include <spdlog/sinks/base_sink.h>

namespace Engine {
	class LogStorage;

	class LogSink : public spdlog::sinks::base_sink<std::mutex> {
	public:
		explicit LogSink(Ref<LogStorage> storage);

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;

	private:
		Ref<LogStorage> m_Storage;

	};
}