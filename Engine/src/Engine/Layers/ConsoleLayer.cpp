#include "pch.h"
#include "Engine/Layers/ConsoleLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Devices/Keyboard.h"
#include "Engine/ImGui/ImGuiGuard.h"
#include "Engine/ImGui/ImGuiUtils.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Lua/LuaUtils.h"

#include <fstream>
#include <imgui.h>

namespace Game
{
	static constexpr uint32_t InputTextFlags = ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_EnterReturnsTrue;

	ConsoleLayer::ConsoleLayer() : Layer("ConsoleLayer") {}

	void ConsoleLayer::OnAttach()
	{
		m_App      = &Application::Get();
		auto &game = *m_App;

		game.RegisterShortcut(Shortcut([this]() { m_Show = !m_Show; }, Key::Tilde));

		m_Lua = &game.GetLua();
		m_Messages.reserve(255);

		game.LuaRegister(*this);

		std::ifstream file("ConsoleHistory.txt");

		if(file.good() && file.is_open())
		{
			std::string line;
			while(std::getline(file, line))
				m_History.emplace_back(line);
		}

		file.close();
	}

	void ConsoleLayer::OnImGuiRender()
	{
		if(!m_Show)
			return;

		ImGuiMainWindowProps props{"Console", m_Show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize};
		ImGuiWindowSize size{{520, 600}, ImGuiCond_Appearing};
		ImGuiUniqueGuard<ImGuiMainWindow> guard(props, size);

		const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

		{
			ImGuiChildWindowProps childProps{
				"ConsoleLog",
				{520 - ImGui::GetStyle().ItemSpacing.x, -footerHeightToReserve},
				false,
				ImGuiWindowFlags_HorizontalScrollbar
			};
			ImGuiUniqueGuard<ImGuiChildWindow> childGuard(childProps);
			for(auto &[message, color] : m_Messages)
				ImGui::TextColored(ImVec4(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f), message.c_str());

			if (m_ScrollToBottom)
			{
				m_ScrollToBottom = false;
				ImGui::SetScrollHereY(1.f);
			}
		}

		ImGui::Separator();
		ImGui::PushID("ConsoleInputText");
		if(InputText(
		             "",
		             m_Command,
		             InputTextFlags,
		             [](ImGuiInputTextCallbackData *data) { return static_cast<ConsoleLayer*>(data->UserData)->TextCallback(data); },
		             this
		            ))
		{
			PrintMessage(fmt::format("> {}", m_Command), Color(0.f, 1.f, 0.f, 1.f));
			// m_Messages.emplace_back(std::make_pair(fmt::format("> {}", m_Command), Color(0.f, 1.f, 0.f, 1.f)));
			m_History.emplace_back(m_Command);
			m_ReclaimFocus = true;

			const auto executionResult = m_Lua->do_string(m_Command);
			if(!executionResult.valid())
			{
				sol::error error = executionResult;
				PRINT_ERROR("{}", error.what());

				PrintMessage(error.what(), Color(1.f, 0.f, 0.f, 1.f));
				// m_Messages.emplace_back(std::make_pair(error.what(), Color(1.f, 0.f, 0.f, 1.f)));
			}

			m_HistoryPos = -1;
			m_Command.clear();
		}
		ImGui::PopID();

		ImGui::SetItemDefaultFocus();
		if(m_ReclaimFocus)
		{
			ImGui::SetKeyboardFocusHere(-1);
			m_ReclaimFocus = false;
		}
	}

	void ConsoleLayer::OnEvent(Event &event) { }

	void ConsoleLayer::OnDetach()
	{
		std::ofstream file("ConsoleHistory.txt", std::ios::trunc);

		if(file.good())
			for(const auto &historyEntry : m_History)
				file << historyEntry << "\n";

		file.close();
	}

	void ConsoleLayer::Clear()
	{
		m_Messages.clear();
	}

	void ConsoleLayer::ClearHistory()
	{
		m_History.clear();
		m_HistoryPos = -1;
	}

	void ConsoleLayer::PrintMessage(const std::string &message, const Color &color)
	{
		m_ScrollToBottom = true;
		m_Messages.emplace_back(std::make_pair(message, color));
	}

	void ConsoleLayer::Register(sol::state &state)
	{
		auto consoleMetaTable = state.create_table_with();

		consoleMetaTable.set_function("Print", [&](sol::variadic_args args) { PrintMessage(ToString(args)); });
		consoleMetaTable.set_function("Clear", [&]() { Clear(); });
		consoleMetaTable.set_function("ClearHistory", [&]() { ClearHistory(); });

		auto consoleTable = state.create_named_table("Console");

		SetAsReadOnlyTable(consoleTable, consoleMetaTable, Deny);

		state["Clear"]        = consoleTable["Clear"];
		state["Print"]        = consoleTable["Print"];
		state["ClearHistory"] = consoleTable["ClearHistory"];
	}

	int ConsoleLayer::TextCallback(void *data)
	{
		auto myData = static_cast<ImGuiInputTextCallbackData*>(data);

		if(myData->EventFlag == ImGuiInputTextFlags_CallbackHistory)
		{
			const int prevHistoryPos = m_HistoryPos;

			if(myData->EventKey == ImGuiKey_UpArrow)
			{
				if(m_HistoryPos == -1)
					m_HistoryPos = static_cast<int32_t>(m_History.size() - 1ull);
				else if(m_HistoryPos > 0)
					m_HistoryPos--;
			}
			else if(myData->EventKey == ImGuiKey_DownArrow)
			{
				if(m_HistoryPos != -1)
				{
					if(++m_HistoryPos >= m_History.size())
						--m_HistoryPos;
				}
			}

			if(prevHistoryPos != m_HistoryPos)
			{
				const auto *historyStr = (m_HistoryPos >= 0) ? m_History[m_HistoryPos].c_str() : "";

				myData->DeleteChars(0, myData->BufTextLen);
				myData->InsertChars(0, historyStr);
			}
		}

		return 0;
	}
}
