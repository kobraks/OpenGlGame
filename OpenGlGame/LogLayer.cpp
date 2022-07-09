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
			case spdlog::level::trace:
				return Game::Color(204, 204, 204);
			case spdlog::level::debug:
				return Game::Color(58, 150, 221);
			case spdlog::level::info:
				return Game::Color(19, 161, 14);
			case spdlog::level::warn:
				return Game::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Game::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Game::Color(0, 0, 0, 0);
		}
	}

	constexpr Game::Color SelectBackgroundColor(spdlog::level::level_enum level)
	{
		switch(level)
		{
			case spdlog::level::trace:
				return Game::Color(204, 204, 204);
			case spdlog::level::debug:
				return Game::Color(58, 150, 221);
			case spdlog::level::info:
				return Game::Color(19, 161, 14);
			case spdlog::level::warn:
				return Game::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Game::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Game::Color(0, 0, 0, 0);
		}
	}

	void Print(sol::this_state state, int severity, const sol::variadic_args &args)
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
		const auto message = Game::ToString(args);

		lua_Debug info;
		//Getting info about caller
		lua_getstack(state, 1, &info);
		lua_getinfo(state, "nSl", &info);

		std::string source = info.source;
		if(!source.empty())
		{
			if(source[0] == '@' || source[0] == '=')
				source = info.short_src;
			else if(!info.name && std::string(info.what) == "main")
				source = "Console";
		}

		Game::Log::GetScriptLogger()->log(
		                                  {source.c_str(), info.currentline, info.name ? info.name : "<Unknown>"},
		                                  level,
		                                  message
		                                 );
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

	constexpr std::string_view GetFirst(const std::string &string, size_t size)
	{
		return {string.begin(), string.size() > size ? string.begin() + size : string.end()};
	}
}

namespace Game
{
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

				if(m_AutoPopUp)
				{
					Combo("Min severity to popup", m_MinLogLevelToPopUp, LogPopUpLevels);
				}

				ImGui::InputInt("Max num of messages", reinterpret_cast<int*>(&s_MaxMessages));

				ImGui::Separator();
			}

			if(ImGui::Button("Clear"))
				Clear();

			m_Filter.Draw("Filter", -100.f);

			ImGui::PushID("Num of messages");
			if(m_Messages.size() >= s_MaxMessages)
				ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
			ImGui::LabelText("", "Num of messages: %i/%i", m_Messages.size(), s_MaxMessages);
			if(m_Messages.size() >= s_MaxMessages)
			{
				ImGui::PopStyleColor();
				ImGui::TextColored(
				                   {1, 0, 0, 1},
				                   "Buffer full for memory save no more messages will be shown please claer buffer"
				                  );
			}
			ImGui::PopID();

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
		if(m_Messages.size() >= s_MaxMessages)
			return;

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

		fmt::format_to(std::back_inserter(timeString), "{}:{}:{}", tm.tm_hour, tm.tm_min, tm.tm_sec);

		const Source source{
			msg.source.filename ? msg.source.filename : "(null)",
			msg.source.funcname ? msg.source.funcname : "(null)",
			msg.source.line
		};

		Message message{
			msg.logger_name.data(),
			msg.payload.data(),
			fmt::to_string(formatted),
			SelectTextColor(msg.level),
			msg.level,
			msg.thread_id,
			source,
			fmt::to_string(timeString),
			std::hash<std::string>()(fmt::format("{}{}", m_Messages.size(), message.Text)),
			std::hash<std::string>()(fmt::format("Selected{}{}", m_Messages.size(), message.Text)),
			std::hash<std::string>()(fmt::format("Text{}\"{}\"", m_Messages.size(), message.Text))
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

		loggerMetaTable["Print"] = Print;
		loggerMetaTable["Debug"] = [](sol::this_state state, const sol::variadic_args &args) { Print(state, 0, args); };
		loggerMetaTable["Info"] = [](sol::this_state state, const sol::variadic_args &args) { Print(state, 1, args); };
		loggerMetaTable["Warn"] = [](sol::this_state state, const sol::variadic_args &args) { Print(state, 2, args); };
		loggerMetaTable["Error"] = [](sol::this_state state, const sol::variadic_args &args) { Print(state, 3, args); };
		loggerMetaTable["Err"] = [](sol::this_state state, const sol::variadic_args &args) { Print(state, 4, args); };
		loggerMetaTable["Critical"] = [](sol::this_state state, const sol::variadic_args &args)
		{
			Print(state, 5, args);
		};

		loggerMetaTable["SetLevel"]        = [](const int severity) { SetLogLevel(severity); };
		loggerMetaTable["CurrentLevel"]    = []() { return static_cast<int>(Log::GetScriptLogger()->level()); };
		loggerMetaTable["DumpBacktrace"]   = []() { Log::GetScriptLogger()->dump_backtrace(); };
		loggerMetaTable["EnableBacktrace"] = [](size_t n) { Log::GetScriptLogger()->enable_backtrace(n); };

		auto logTable = state.create_named_table("Log");

		SetAsReadOnlyTable(logTable, loggerMetaTable);

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

	constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings;
	constexpr int TABLE_COLUMN_COUNT = 5;

	void LogLayer::SetUpTable()
	{
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 35, 0);
		ImGui::TableSetupColumn("TimeString", ImGuiTableColumnFlags_WidthFixed, 60, 1);
		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 60, 2);
		ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 80, 3);
		ImGui::TableSetupColumn("Short Description", ImGuiTableColumnFlags_WidthFixed, 500, 4);
	}

	void LogLayer::PrintMessagesTable()
	{
		const ImGuiChildWindowProps childProps{"Scrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar};
		ImGuiUniqueGuard<ImGuiChildWindow> childGuard(childProps);
		// if(copyButton) ImGui::LogToClipboard();


		ImGui::PushID("LoggerTable");
		{
			ImGuiGuard<ImGuiTable> guard(ImGuiTableProps{"", TABLE_COLUMN_COUNT, TABLE_FLAGS});

			{
				SetUpTable();
				ImGui::TableHeadersRow();

				for(size_t i = 0; i < m_Messages.size(); ++i)
				{
					auto &message = m_Messages[i];

					if(!m_Filter.PassFilter(message.Text.c_str()))
						continue;

					PrintMessage(guard, i, message);
					if(message.Selected)
						PrintSelectedMessage(guard, i, message);
				}
			}
			bool a = true;
		}
		ImGui::PopID();

		if(m_AllowScrolling && m_ScrollToBottom)
			ImGui::SetScrollHereY(1.0);


		m_ScrollToBottom = false;
	}

	void LogLayer::PrintMessage(ImGuiGuard<ImGuiTable> &tableGuard, size_t i, Message &message)
	{
		//Normal row
		ImGui::TableNextRow();
		ImGuiUniqueGuard<ImGuiID> IdGuard(static_cast<int>(message.IdHash));
		{
			ImGuiUniqueGuard<ImGuiGroup> groupGuard{}; // Making sure it is deleted first

			//Begining of new row
			ImGui::TableNextColumn();

			//Setting bg color and making sure it is selectable
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, message.Color.ToInteger());
			ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);

			//Setting hovered info
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip(message.Text.c_str());
			if(ImGui::IsItemClicked())
				message.Selected = !message.Selected;

			//Making sure the first col is the same line
			ImGui::SameLine();
			ImGui::Text("%i", i);
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(message.TimeString.c_str());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(to_string_view(message.Level).data());
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(message.Name.c_str());
			ImGui::TableNextColumn();

			const auto shortDesc = GetFirst(message.Desc, 15);
			ImGui::TextUnformatted(shortDesc.data(), shortDesc.data() + shortDesc.size()); //A mess ...
		}
	}

	void LogLayer::PrintSelectedMessage(ImGuiGuard<ImGuiTable> &tableGuard, size_t i, Message &msg)
	{
		tableGuard.Unlock();

		{
			ImGuiUniqueGuard<ImGuiID> idGuard(static_cast<int>(msg.IdSelectedHash));
			{
				ImGuiUniqueGuard<ImGuiGroup> groupGuard();

				//Soruce text display
				Text("File {}", msg.Source.File);
				Text("Function {}", msg.Source.Function);
				Text("Line {}", msg.Source.Line);

				ImGui::Separator();

				//Printing other stored information
				ImGui::Text("Time: %s", msg.TimeString.c_str());
				ImGui::Text("Name: %s", msg.Name.c_str());
				ImGui::Text("Level: %s", to_string_view(msg.Level).data());

				ImGui::PushID(static_cast<int>(msg.IdTextMultiline));
				InputTextMultiline("", msg.Desc, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopID();

				ImGui::Separator();

				if(ImGui::Button("Copy"))
					ImGui::SetClipboardText(msg.Text.c_str());

				ImGui::Separator();
			}
		}

		tableGuard.Lock();
	}
}
