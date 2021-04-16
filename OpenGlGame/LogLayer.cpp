#include "pch.h"
#include "LogLayer.h"

#include <chrono>

#include "Log.h"

#include "Application.h"
#include "ImGuiGuard.h"
#include "ImGuiUtils.h"
#include "Keyboard.h"
#include "LuaUtils.h"

#include "KeyEvent.h"

namespace
{
	constexpr Game::Color SelectTextColor(spdlog::level::level_enum level)
	{
		switch(level)
		{
			case spdlog::level::trace: return Game::Color(204, 204, 204);
			case spdlog::level::debug: return Game::Color(58, 150, 221);
			case spdlog::level::info: return Game::Color(19, 161, 14);
			case spdlog::level::warn: return Game::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical: return Game::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default: return Game::Color(0, 0, 0, 0);
		}
	}

	constexpr Game::Color SelectBackgroundColor(spdlog::level::level_enum level)
	{
		switch(level)
		{
			case spdlog::level::trace: return Game::Color(204, 204, 204);
			case spdlog::level::debug: return Game::Color(58, 150, 221);
			case spdlog::level::info: return Game::Color(19, 161, 14);
			case spdlog::level::warn: return Game::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical: return Game::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default: return Game::Color(0, 0, 0, 0);
		}
	}

	void Print(int severity, const sol::variadic_args &args)
	{
		if(severity < spdlog::level::trace)
		{
			PRINT_SCRIPT_WARN("Unknown log severity level, deducing Trace level");
			severity = spdlog::level::trace;
		}

		if(severity > spdlog::level::off)
		{
			PRINT_SCRIPT_WARN("Unknown log severity level, deducing Critical level");
			severity = spdlog::level::critical;
		}

		const auto level   = static_cast<spdlog::level::level_enum>(severity);
		const auto message = Game::ToString(args.lua_state(), args);

		Game::Log::GetScriptLogger()->log(level, message);
	}

	void SetLogLevel(int severity)
	{
		if(severity < spdlog::level::trace)
		{
			PRINT_SCRIPT_WARN("Unknown log severity level, deducing Trace level");
			severity = spdlog::level::trace;
		}

		if(severity > spdlog::level::off)
		{
			PRINT_SCRIPT_WARN("Unknown log severity level, deducing Critical level");
			severity = spdlog::level::critical;
		}

		Game::Log::GetScriptLogger()->set_level(static_cast<spdlog::level::level_enum>(severity));
	}

	std::string GetFirst(const std::string &string, size_t size)
	{
		return std::string(string.begin(), string.size() > size ? string.begin() + size : string.end());
	}
}

namespace Game
{
	struct Source
	{
		const char *FileName{nullptr};
		const char *FunctionName{nullptr};
		int Line{0};
	};

	struct LogLayer::Message
	{
		std::string Name;
		std::string Desc;
		std::string Text;

		Color Color;

		spdlog::level::level_enum Level;

		size_t ThreadId{0};
		Source Source;

		std::string Time;
		bool Selected = false;
	};

	LogLayer::LogLayer() : Layer("LogLayer") {}

	void LogLayer::OnAttach()
	{
		Application::Get().LuaRegister(*this);
	}

	void LogLayer::OnImGuiRender()
	{
		constexpr std::string_view LogPopUpLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical";

		if(m_Show)
		{
			ImGuiWindowSize size{ImVec2(700, 400), ImGuiCond_FirstUseEver};
			ImGuiMainWindowProps props{"Log", m_Show, ImGuiWindowFlags_NoSavedSettings};
			ImGuiUniqueGuard<ImGuiMainWindow> guard(props, size);

			if(ImGui::CollapsingHeader("Options"))
			{
				LoggerCombo(Log::GetApplicationLogger());
				LoggerCombo(Log::GetScriptLogger());
				LoggerCombo(Log::GetOpenGLLogger());
				LoggerCombo(Log::GetAssertionLogger());

				ToggleButton("Allow Scrolling", &m_AllowScrolling);
				ToggleButton("Auto popup", &m_AutoPopUp);
				// ImGui::Checkbox("Allow Scrolling", &m_AllowScrolling);
				// ImGui::Checkbox("Auto popup", &m_AutoPopUp);

				if(m_AutoPopUp)
				{
					Combo("Min severity to popup", m_MinLogLevelToPopUp, LogPopUpLevels);
				}

				ImGui::Separator();
			}

			// bool copyButton = ImGui::Button("Copy");
			// ImGui::SameLine();
			if(ImGui::Button("Clear"))
				Clear();

			m_Filter.Draw("Filter", -100.f);

			PrintMessagesTable();
		}
	}

	void LogLayer::OnEvent(Event &event)
	{
		if(event.GetEventType() == KeyReleasedEvent::GetStaticType())
		{
			if(const auto release = dynamic_cast<KeyReleasedEvent*>(&event); release)
			{
				if(release->GetKeyCode() == Key::F2)
					m_Show = !m_Show;
			}
		}
	}

	void LogLayer::Clear()
	{
		m_Messages.clear();
	}

	void LogLayer::sink_it_(const spdlog::details::log_msg &msg)
	{
		if(msg.level >= m_MinLogLevelToPopUp && msg.level < spdlog::level::off)
		{
			if(m_AutoPopUp)
			{
				if(!m_Show)
					m_ScrollToBottom = true;

				m_Show = true;
			}
		}

		spdlog::memory_buf_t formatted;
		formatter_->format(msg, formatted);

		time_t time = std::chrono::system_clock::to_time_t(msg.time);
		std::tm tm;
		localtime_s(&tm, &time);

		fmt::memory_buffer timeString;

		format_to(timeString, "{}:{}:{}", tm.tm_hour, tm.tm_min, tm.tm_sec);

		Message message{
			msg.logger_name.data(),
			msg.payload.data(),
			fmt::to_string(formatted),
			SelectTextColor(msg.level),
			msg.level,
			msg.thread_id,
			{msg.source.filename, msg.source.funcname, msg.source.line},
			fmt::to_string(timeString)
		};

		m_Messages.emplace_back(std::move(message));
	}

	void LogLayer::flush_() {}

	void LogLayer::Register(sol::state &state)
	{
		state.new_enum<spdlog::level::level_enum>(
		                                          "Level",
		                                          {
			                                          {"Trace", spdlog::level::trace},
			                                          {"Debug", spdlog::level::debug},
			                                          {"Info", spdlog::level::info},
			                                          {"Warn", spdlog::level::warn},
			                                          {"Error", spdlog::level::err},
			                                          {"Err", spdlog::level::err},
			                                          {"Critical", spdlog::level::critical},
			                                          {"Off", spdlog::level::off},
			                                          {"Nlevels", spdlog::level::n_levels}
		                                          }
		                                         );

		auto loggerMetaTable = state.create_table_with();


		loggerMetaTable["Level"] = state["Level"];

		loggerMetaTable["Print"]    = Print;
		loggerMetaTable["Debug"]    = [](const sol::variadic_args &args) { Print(0, args); };
		loggerMetaTable["Info"]     = [](const sol::variadic_args &args) { Print(1, args); };
		loggerMetaTable["Warn"]     = [](const sol::variadic_args &args) { Print(2, args); };
		loggerMetaTable["Error"]    = [](const sol::variadic_args &args) { Print(3, args); };
		loggerMetaTable["Err"]      = [](const sol::variadic_args &args) { Print(4, args); };
		loggerMetaTable["Critical"] = [](const sol::variadic_args &args) { Print(5, args); };

		loggerMetaTable["SetLevel"]        = [](const int severity) { SetLogLevel(severity); };
		loggerMetaTable["CurrentLevel"]    = []() { return static_cast<int>(Log::GetScriptLogger()->level()); };
		loggerMetaTable["DumpBacktrace"]   = []() { Log::GetScriptLogger()->dump_backtrace(); };
		loggerMetaTable["EnableBacktrace"] = [](size_t n) { Log::GetScriptLogger()->enable_backtrace(n); };

		auto logTable = state.create_named_table("Log");

		SetAsReadOnlyTable(logTable, loggerMetaTable, Deny);

		state["Level"] = sol::nil;
	}

	void LogLayer::LoggerCombo(Pointer<spdlog::logger> logger)
	{
		static constexpr std::string_view LogLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical\0Off";

		int32_t currentOption = logger->level();

		if(Combo(fmt::format("{0} severity level", logger->name()), currentOption, LogLevels))
		{
			logger->set_level(static_cast<spdlog::level::level_enum>(currentOption));
		}
	}

	constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_BordersInnerV |
		ImGuiTableFlags_NoSavedSettings;
	constexpr int TABLE_COLUMN_COUNT = 4;

	void LogLayer::SetUpTable()
	{
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 60, 0);
		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 60, 1);
		ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 80, 2);
		ImGui::TableSetupColumn("Short Description", ImGuiTableColumnFlags_WidthFixed, 500, 3);
	}

	void LogLayer::PrintMessagesTable()
	{
		const ImGuiChildWindowProps childProps{"Scrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar};
		ImGuiUniqueGuard<ImGuiChildWindow> childGuard(childProps);
		// if(copyButton) ImGui::LogToClipboard();


		ImGui::PushID("LoggerTable");
		{
			ImGuiUniqueGuard<ImGuiTable> guard(ImGuiTableProps{"", TABLE_COLUMN_COUNT, TABLE_FLAGS});

			SetUpTable();
			ImGui::TableHeadersRow();

			for(size_t i = 0; i < m_Messages.size(); ++i)
			{
				auto &message = m_Messages[i];

				if(!m_Filter.PassFilter(message.Text.c_str()))
					continue;

				PrintMessage(i, message);
			}
		}
		ImGui::PopID();

		if(m_AllowScrolling && m_ScrollToBottom)
			ImGui::SetScrollHereY(1.0);


		m_ScrollToBottom = false;
	}

	void LogLayer::PrintMessage(size_t i, Message &message)
	{
		// TextColored(message.Color, message.Text);

		ImGui::TableNextRow();
		ImGui::PushID(fmt::format("{}{}", message.Text, i).c_str());
		{
			ImGuiUniqueGuard<ImGuiGroup> guard{};
			ImGui::TableNextColumn();
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, message.Color.ToInteger());
			ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip(message.Text.c_str());
			if(ImGui::IsItemClicked())
				message.Selected = !message.Selected;
			ImGui::SameLine();
			ImGui::Text(message.Time.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(to_string_view(message.Level).data());
			ImGui::TableNextColumn();
			ImGui::Text(message.Name.c_str());
			ImGui::TableNextColumn();
			ImGui::Text(GetFirst(message.Desc, 15).c_str());
		}
		ImGui::PopID();

		if(message.Selected)
		{
			ImGui::EndTable();

			ImGui::PushID(fmt::format("Options{}{}", message.Text, i).c_str());
			{
				ImGuiUniqueGuard<ImGuiGroup> guard();
				ImGui::Text("File: %s", message.Source.FileName);
				ImGui::Text("Function name: %s", message.Source.FunctionName);
				ImGui::Text("Line: %s", message.Source.Line);

				ImGui::Separator();

				ImGui::Text("Time: %s", message.Time.c_str());
				ImGui::Text("Name: %s", message.Name.c_str());
				ImGui::Text("Level: %s", to_string_view(message.Level).data());

				InputTextMultiline(fmt::format("{}", i), message.Desc, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
				if(ImGui::Button("Copy"))
					ImGui::SetClipboardText(message.Text.c_str());
			}
			ImGui::PopID();

			ImGui::BeginTable("", TABLE_COLUMN_COUNT, TABLE_FLAGS);
			SetUpTable();
		}
	}
}
