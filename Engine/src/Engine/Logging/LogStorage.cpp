#include "pch.h"
#include "LogStorage.h"

#include <utility>

namespace Engine {
	LogStorage::LogStorage() {
		m_Messages.reserve(static_cast<std::size_t>(m_MaxMessages));
	}

	void LogStorage::AddMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg) {
		std::lock_guard lock(m_Mutex);

		if (m_Messages.size() >= m_MaxMessages) {
			// Pop oldest message
			m_Messages.erase(m_Messages.begin());
			++m_DroppedMessageCount;
		}

		const auto message = m_Messages.emplace_back(m_NextIndex++, formatted, msg);

		if (m_Callback) {
			m_Callback(message, m_Messages.size() - 1);
		}
	}

	void LogStorage::Clear() {
		std::lock_guard lock(m_Mutex);
		m_Messages.clear();
		m_NextIndex = 0;
		m_DroppedMessageCount = 0;
	}

	std::vector<LogMessageEntry> LogStorage::CreateMessageSnapshot() const {
		std::lock_guard lock(m_Mutex);
		return m_Messages; // coping for thread safety.
	}

	uint64_t LogStorage::GetMessageCount() const {
		std::lock_guard lock(m_Mutex);
		return m_Messages.size();
	}

	void LogStorage::SetMaxMessages(uint64_t max) {
		if (max > m_MaxMessages) {
			std::lock_guard lock(m_Mutex);
			m_Messages.reserve(max);
		}

		m_MaxMessages = max;
	}

	LogMessageEntry LogStorage::GetMessageEntry(uint64_t index) const {
		std::lock_guard lock(m_Mutex);

		ENGINE_ASSERT(index < m_Messages.size());
		if (index >= m_Messages.size())
			throw std::out_of_range("Index out of bounds");

		return m_Messages.at(index);
	}

	void LogStorage::SetNewMessageCallback(MessageCallbackFunctionType messageCallback) {
		std::lock_guard lock(m_Mutex);
		m_Callback = std::move(messageCallback);
	}

	bool LogStorage::HasCallback() const {
		std::lock_guard lock(m_Mutex);
		return static_cast<bool>(m_Callback);
	}

}
