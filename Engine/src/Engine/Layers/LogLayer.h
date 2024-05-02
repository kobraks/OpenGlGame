#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Layers/Layer.h"

#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <spdlog/sinks/base_sink.h>

struct ImGuiTextFilter;

namespace Engine {
	class LogLayer: public Layer, public spdlog::sinks::base_sink<std::mutex> {
	public:
		LogLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnEvent(Event &event) override;

		void Clear();

		void Visible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }

		void AutoPopUp(bool autoPopUp) { m_AutoPopUp = autoPopUp; }
		bool AutoPupUp() const { return m_AutoPopUp; }

		void AllowScrolling(bool allowScrolling) { m_AllowScrolling = allowScrolling; }
		bool AllowScrolling() const { return m_AllowScrolling; }

		void ScrollToBottom(bool scrollToBottom) { m_ScrollToBottom = scrollToBottom; }
		bool ScrollToBottom() const { return m_ScrollToBottom; }

		void MinLogLevelToPopUp(const int32_t level) {
			m_MinLogLevelToPopUp = std::clamp(
			                                  m_MinLogLevelToPopUp,
			                                  static_cast<int32_t>(spdlog::level::trace),
			                                  static_cast<int32_t>(spdlog::level::critical)
			                                 );
		}

		int32_t MinLogLevelToPopUp() const { return m_MinLogLevelToPopUp; }

	protected:
		void sink_it_(const spdlog::details::log_msg &msg) override;
		void flush_() override;

	private:
		struct Source;
		struct Message;
		struct MessageEntry;


		void LoggerCombo(Ref<spdlog::logger> logger);

		static void SetUpTable();

		void BeginTable();
		void EndTable();

		void PrintMessagesTable();
		void PrintMessage(size_t i, MessageEntry &message);
		void PrintSelectedMessage(size_t i, MessageEntry &message);

		static std::string GetTimeAsString(const spdlog::log_clock::time_point& time);

		struct Source {
			Source(const spdlog::source_loc &loc);
			Source(const spdlog::details::log_msg &msg);

			std::string File;
			std::string Function;
			int Line{0};
			size_t ThreadId{0};

			void Print();
		};

		struct Message {
			Message(const spdlog::memory_buf_t &formatted, const spdlog::details::log_msg &msg, size_t shortLen = 15);

			std::string Name;
			std::string Desc;
			std::string_view ShortDesc;
			std::string Text;
			std::string Time;
			spdlog::level::level_enum Level;
		};

		struct MessageEntry {
			MessageEntry(const spdlog::memory_buf_t &formatted, const spdlog::details::log_msg &msg, size_t shortLen = 15);

			Message Message;
			Source Source;

			Color Color;
			size_t IdHash          = 0;
			size_t IdSelectedHash  = 0;
			size_t IdTextMultiline = 0;
			bool Selected          = false;

			void GenerateHash(size_t i);
		};

		bool m_Show           = true;
		bool m_ScrollToBottom = true;
		bool m_AutoPopUp      = true;
		bool m_AllowScrolling = true;

		int32_t m_MinLogLevelToPopUp       = 0;
		inline static size_t s_MaxMessages = 1000;

		Scope<ImGuiTextFilter> m_Filter;

		std::vector<MessageEntry> m_Messages;
	};
}
