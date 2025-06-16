#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Logging/LogTypes.h"

#include <mutex>
#include <atomic>
#include <functional>

namespace Engine {
	class LogStorage {
	public:
		using MessageCallbackFunctionType = std::function<void(const LogMessageEntry&, uint64_t)>;

		LogStorage();

		void AddMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg);
		void Clear();

		std::vector<LogMessageEntry> CreateMessageSnapshot() const;

		uint64_t GetMessageCount() const;
		uint64_t GetDroppedMessageCount() const { return m_DroppedMessageCount; }

		void SetMaxMessages(uint64_t max);

		uint64_t GetMaxMessages() const { return m_MaxMessages; }
		uint64_t GetLastIndex() const { return m_NextIndex; }

		LogMessageEntry GetMessageEntry(uint64_t index) const;

		bool HasNewMessageSince(uint64_t lastIndex) const { return lastIndex < m_NextIndex; }

		void SetNewMessageCallback(MessageCallbackFunctionType messageCallback);
		bool HasCallback() const;
	private:
		mutable std::mutex m_Mutex;

		std::vector<LogMessageEntry> m_Messages;

		std::atomic<uint64_t> m_DroppedMessageCount = 0;
		std::atomic<uint64_t> m_MaxMessages = 10000;
		std::atomic<uint64_t> m_NextIndex = 0;

		MessageCallbackFunctionType m_Callback;
	};
}