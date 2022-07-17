#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Layers/Layer.h"
#include "Engine/Lua/LuaRegister.h"

#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <spdlog/sinks/base_sink.h>
#include <imgui.h>

namespace Game
{
	template <class ImGuiMenu>
	class ImGuiGuard;
	class ImGuiTable;

	class LogLayer: public Layer, public LuaRegister, public spdlog::sinks::base_sink<std::mutex>
	{
		struct Source
		{
			std::string File;
			std::string Function;
			int Line{0};
		};

		struct Message
		{
			std::string Name;
			std::string Desc;
			std::string Text;

			Color Color;

			spdlog::level::level_enum Level;

			size_t ThreadId{0};
			Source Source;

			std::string TimeString;
			size_t IdHash = 0;
			size_t IdSelectedHash = 0;
			size_t IdTextMultiline = 0;
			bool Selected = false;
		};

		bool m_Show           = true;
		bool m_ScrollToBottom = true;
		bool m_AutoPopUp      = true;
		bool m_AllowScrolling = true;

		int32_t m_MinLogLevelToPopUp = 0;
		inline static size_t s_MaxMessages = 1000;

		ImGuiTextFilter m_Filter;

		std::vector<Message> m_Messages;
	public:
		LogLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnEvent(Event &event) override;

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
		void sink_it_(const spdlog::details::log_msg &msg) override;
		void flush_() override;

		void Register(sol::state &state) override;

	private:
		void LoggerCombo(Pointer<spdlog::logger> logger);

		static void SetUpTable();

		void PrintMessagesTable();
		void PrintMessage(ImGuiGuard<ImGuiTable>& tableGuard, size_t i, Message &message);
		void PrintSelectedMessage(ImGuiGuard<ImGuiTable>& tableGuard, size_t i, Message &msg);
	};
}
