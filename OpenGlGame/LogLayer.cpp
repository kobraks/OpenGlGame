#include "pch.h"
#include "LogLayer.h"

#include "Log.h"

#include "Application.h"
#include "ImGuiUtils.h"
#include "Keyboard.h"
#include "Utils.h"

namespace Game
{
	static void Print(int severity, const sol::variadic_args &args)
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
		const auto message = ToString(args.lua_state(), args);

		Log::GetScriptLogger()->log(level, message);
	}

	static void SetLogLevel(int severity)
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

		Log::GetScriptLogger()->set_level(static_cast<spdlog::level::level_enum>(severity));
	}

	LogLayer::LogLayer() : Layer("LogLayer") {}

	void LogLayer::OnAttach()
	{
		Application::Get().LuaRegister(*this);
	}

	void LogLayer::OnImGuiRender()
	{
		static constexpr std::string_view LogPopUpLevels = "Trace\0Debug\0Info\0Warn\0Critical";

		if(m_Show)
		{
			ImGuiWindowSize size {ImVec2(700, 400), ImGuiCond_FirstUseEver};
			ImGuiMainWindowProps props{"Log", m_Show};
			ImGuiUniqueGuard<ImGuiMainWindow> guard(props, size);

			if(ImGui::CollapsingHeader("Options"))
			{
				LoggerCombo(Log::GetApplicationLogger());
				LoggerCombo(Log::GetScriptLogger());
				LoggerCombo(Log::GetOpenGLLogger());
				LoggerCombo(Log::GetAssertionLogger());

				ImGui::Checkbox("Allow Scrolling", &m_AllowScrolling);
				ImGui::Checkbox("Auto popup", &m_AutoPopUp);

				if(m_AutoPopUp)
				{
					Combo("Min severity to popup", m_MinLogLevelToPopUp, LogPopUpLevels);
				}
			}

			bool copyButton = ImGui::Button("Copy");
			ImGui::SameLine();
			if(ImGui::Button("Clear")) Clear();

			m_Filter.Draw("Filter", -100.f);

			ImGuiChildWindowProps childProps {"Scrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar};
			ImGuiUniqueGuard<ImGuiChildWindow> childGuard(childProps);
			if(copyButton) ImGui::LogToClipboard();

			for(const auto [message, color] : m_Messages)
			{
				if(!m_Filter.PassFilter(message.c_str())) continue;

				ImGui::TextColored(color, message.c_str());
			}

			if(m_AllowScrolling && m_ScrollToBottom) ImGui::SetScrollHere(1.f);

			m_ScrollToBottom = false;
		}
	}

	void LogLayer::OnUpdate()
	{
		if(Keyboard::IsKeyPressed(Key::F2))
		{
			if(m_Process)
			{
				m_Process = false;
				m_Show      = !m_Show;
			}
		}
		else m_Process = true;
	}

	void LogLayer::Clear()
	{
		m_Messages.clear();
	}

	void LogLayer::sink_it_(const spdlog::details::log_msg &msg)
	{
		if(msg.level >= spdlog::level::err && msg.level < spdlog::level::off)
		{
			if(m_AutoPopUp) m_Show = true;
			m_ScrollToBottom = true;
		}

		spdlog::memory_buf_t formatted;
		formatter_->format(msg, formatted);

		auto message = fmt::to_string(formatted);
		m_Messages.emplace_back(std::make_pair(message, SelectColor(msg.level)));
	}

	void LogLayer::flush_() {}

	void LogLayer::Register(sol::state &state)
	{
		auto levelTable = state.create_table_with(
		                                          "Trace",
		                                          spdlog::level::trace,
		                                          "Debug",
		                                          spdlog::level::debug,
		                                          "Info",
		                                          spdlog::level::info,
		                                          "Warn",
		                                          spdlog::level::warn,
		                                          "Error",
		                                          spdlog::level::err,
		                                          "Err",
		                                          spdlog::level::err,
		                                          "Critical",
		                                          spdlog::level::critical,
		                                          "Off",
		                                          spdlog::level::off,
		                                          "Nlevels",
		                                          spdlog::level::n_levels
		                                         );

		state.create_named_table(
		                         "Log",
		                         "Level",
		                         levelTable,
		                         "Print",
		                         Print,
		                         "Trace",
		                         [](const sol::variadic_args &args) { Print(0, args); },
		                         "Debug",
		                         [](const sol::variadic_args &args) { Print(1, args); },
		                         "Info",
		                         [](const sol::variadic_args &args) { Print(2, args); },
		                         "Warn",
		                         [](const sol::variadic_args &args) { Print(3, args); },
		                         "Error",
		                         [](const sol::variadic_args &args) { Print(4, args); },
		                         "Err",
		                         [](const sol::variadic_args &args) { Print(4, args); },
		                         "Critical",
		                         [](const sol::variadic_args &args) { Print(5, args); },
		                         "SetLevel",
		                         [](const int severity) { SetLogLevel(severity); },
		                         "CurrentLevel",
		                         []() { static_cast<int>(Log::GetScriptLogger()->level()); },
		                         "DumpBacktrace",
		                         []() { Log::GetScriptLogger()->dump_backtrace(); },
		                         "EnableBacktrace",
		                         [](size_t n) { Log::GetScriptLogger()->enable_backtrace(n); }
		                        );
	}

	void LogLayer::LoggerCombo(Pointer<spdlog::logger> logger)
	{
		static constexpr std::string_view LogLevels = "Trace\0Debug\0Info\0Warn\0Critical\0Off";
		int32_t currentOption                       = static_cast<int32_t>(logger->level());

		if(Combo(fmt::format("{0} severity level", logger->name()), currentOption, LogLevels))
		{
			logger->set_level(static_cast<spdlog::level::level_enum>(currentOption));
		}
	}
}
