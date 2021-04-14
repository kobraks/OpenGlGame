#pragma once

#include <spdlog/sinks/base_sink.h>
#include <thread>
#include <vector>
#include <utility>
#include <string>

#include "Layer.h"
#include "Color.h"
#include "imgui.h"
#include "LuaRegister.h"

namespace Game
{
	class LogLayer: public Layer, public LuaRegister, public spdlog::sinks::base_sink<std::mutex>
	{
		struct Message;
		
		bool m_Show           = true;
		bool m_ScrollToBottom = true;
		bool m_AutoPopUp      = true;
		bool m_AllowScrolling = true;

		int32_t m_MinLogLevelToPopUp = 0;

		ImGuiTextFilter m_Filter;

		std::vector<Message> m_Messages;
	public:
		LogLayer();

		virtual void OnAttach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
		
		void Clear();

		void Visible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }

		void AutoPopUp(const bool autoPopUp) { m_AutoPopUp = autoPopUp; }
		bool AutoPopUp() const { return m_AutoPopUp; }

		void AllowScrolling(const bool allowScrolling) { m_AllowScrolling = allowScrolling; }
		bool AllowScrolling() const { return m_AllowScrolling; }

		void ScrollToBottom(const bool scrollToBottom) { m_ScrollToBottom = scrollToBottom; }
		bool ScrollToBottom() const { return m_ScrollToBottom; }

		void MinLogLevelToPopUp(const int32_t level)
		{
			m_MinLogLevelToPopUp = level < 0 ? 0 : level > spdlog::level::critical ? spdlog::level::critical : level;
		}

		int32_t MinLogLevelToPopUp() const { return m_MinLogLevelToPopUp; }

		const auto& GetMessages() const { return m_Messages; }
	protected:
		virtual void sink_it_(const spdlog::details::log_msg &msg) override;
		virtual void flush_() override;

		virtual void Register(sol::state& state) override;

	private:
		void LoggerCombo(Pointer<spdlog::logger> logger);

		static void SetUpTable();
		
		void PrintMessagesTable();
		void PrintMessage(size_t i, Message& message);
	};
}
