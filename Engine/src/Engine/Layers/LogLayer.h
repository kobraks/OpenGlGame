#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Layers/Layer.h"

#include "Engine/Events/KeyEvent.h"

#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <spdlog/sinks/base_sink.h>

struct ImGuiTextFilter;

namespace Engine {
	struct LogSource {
		LogSource(const spdlog::source_loc& loc);
		LogSource(const spdlog::details::log_msg& msg);

		std::string File;
		std::string Function;
		int Line{ 0 };
		size_t ThreadId{ 0 };

		void Print();
	};

	struct LogMessage {
		LogMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg);

		std::string Name;
		std::string Desc;
		std::string Text;
		std::string Time;
		spdlog::level::level_enum Level;
	};

	struct LogMessageEntry {
		LogMessageEntry(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg);

		LogMessage Message;
		LogSource Source;

		Color Color;
		size_t IdHash = 0;
		size_t IdSelectedHash = 0;
		size_t IdTextMultiline = 0;

		void GenerateHash(size_t i);
	};

	struct LogBufferSnapshot {
		std::vector<LogMessageEntry> Messages;
		std::vector<size_t> VisibleMessageIndices;
	};

	struct LogBufferCopier {
	public:
		LogBufferCopier(std::mutex& mutex, const std::vector<LogMessageEntry>& sourceMessages, const std::vector<size_t>& sourceVisible, LogBufferSnapshot& outSnapshot);
	};

	class LogLayer : public Layer, public spdlog::sinks::base_sink<std::mutex> {
	public:
		LogLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		bool OnKeyPressEvent(KeyPressedEvent& event);

		void Clear();

		void Visible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }

		void AutoPopUp(bool autoPopUp) { m_AutoPopUp = autoPopUp; }
		bool AutoPopUp() const { return m_AutoPopUp; }

		void AllowScrolling(bool allowScrolling) { m_AllowScrolling = allowScrolling; }
		bool AllowScrolling() const { return m_AllowScrolling; }

		void ScrollToBottom(bool scrollToBottom) { m_ScrollToBottom = scrollToBottom; }
		bool ScrollToBottom() const { return m_ScrollToBottom; }

		void MinLogLevelToPopUp(const int32_t level) {
			m_MinLogLevelToPopUp = std::clamp(
				level,
				static_cast<int32_t>(spdlog::level::trace),
				static_cast<int32_t>(spdlog::level::critical)
			);
		}

		int32_t MinLogLevelToPopUp() const { return m_MinLogLevelToPopUp; }

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;

	private:
		void UpdateVisibleMessages();

		void LoggerCombo(Ref<spdlog::logger> logger);

		static void SetUpTable();

		void BeginTable();
		void EndTable();

		void PrintMessagesTable();
		void PrintTable();
		void PrintClippedTable(LogBufferSnapshot& snapshot, int itemCount, int startIndex = -1);
		void PrintMessage(size_t i, LogMessageEntry& messageEntry);
		void PrintSelectedMessage(size_t i, LogMessageEntry& message);

		bool m_Show = true;
		bool m_ScrollToBottom = true;
		bool m_AutoPopUp = true;
		bool m_AllowScrolling = true;
		bool m_Pause = false;

		std::string m_LastFilterText;

		int32_t m_MinLogLevelToPopUp = 0;
		inline static size_t s_MaxMessages = 1000;
		size_t m_SelectedMessageIndex;

		Scope<ImGuiTextFilter> m_Filter;

		std::vector<LogMessageEntry> m_Messages;
		std::vector<size_t> m_VisibleMessageIndices;

		std::mutex m_Mutex;
	};
}
