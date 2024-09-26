#include "pch.h"
#include "LogLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"

#include "Engine/Events/KeyEvent.h"

#include "Engine/Devices/Keyboard.h"

#include <chrono>
#include <fmt/format.h>
#include <fmt/chrono.h>

#include "Imgui.h"
#include "Engine/ImGui/ImGuiUtils.h"


namespace {
	constexpr Engine::Color SelectTextColor(spdlog::level::level_enum level) {
		switch(level) {
			case spdlog::level::trace:
				return Engine::Color(204, 204, 204);
			case spdlog::level::debug:
				return Engine::Color(58, 150, 221);
			case spdlog::level::info:
				return Engine::Color(19, 161, 14);
			case spdlog::level::warn:
				return Engine::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Engine::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Engine::Color(0, 0, 0, 0);
		}
	}

	constexpr Engine::Color SelectBackgroundColor(spdlog::level::level_enum level) {
		switch(level) {
			case spdlog::level::trace:
				return Engine::Color(204, 204, 204);
			case spdlog::level::debug:
				return Engine::Color(58, 150, 221);
			case spdlog::level::info:
				return Engine::Color(19, 161, 14);
			case spdlog::level::warn:
				return Engine::Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Engine::Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Engine::Color(0, 0, 0, 0);
		}
	}

	constexpr std::string_view GetFirst(const std::string &string, size_t size) {
		return {string.begin(), string.size() > size ? string.begin() + size : string.end()};
	}
}

namespace Engine {
	constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings;
	constexpr int TABLE_COLUMN_COUNT = 5;

	LogLayer::LogLayer() : Layer("LogLayer") {}

	void LogLayer::OnAttach() {
		Layer::OnAttach();
	}

	void LogLayer::OnImGuiRender() {
		constexpr std::string_view LogPopUpLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical";

		if(m_Show) {
			ImGui::SetNextWindowSize({700, 400}, ImGuiCond_FirstUseEver);
			ImGui::Begin("Log", &m_Show, ImGuiWindowFlags_NoSavedSettings);

			if(ImGui::CollapsingHeader("Options")) {
				LoggerCombo(Log::GetApplicationLogger());
				LoggerCombo(Log::GetEngineLogger());
				LoggerCombo(Log::GetGLLogger());
				LoggerCombo(Log::GetScriptLogger());

				ToggleButton("Allow Scrolling", &m_AllowScrolling);
				ToggleButton("Auto Popup", &m_AutoPopUp);

				if (m_AutoPopUp) {
					Combo("Min severity to popup", m_MinLogLevelToPopUp, LogPopUpLevels);
				}

				ImGui::InputInt("Max num of messages", reinterpret_cast<int *>(&s_MaxMessages));
				ImGui::Separator();
			}

			if (ImGui::Button("Clear"))
				Clear();

			m_Filter->Draw("Filter", -100.f);

			ImGui::PushID("Num of messages");
			if (m_Messages.size() >= s_MaxMessages)
				ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
			ImGui::LabelText("", "Num of messages: %i/%i", m_Messages.size(), s_MaxMessages);
			if (m_Messages.size() >= s_MaxMessages) {
				ImGui::PopStyleColor();
				ImGui::TextColored({1, 0, 0, 1}, "Buffer full for memory save no more messages will be shown please clear buffer");
			}
			ImGui::PopID();

			PrintMessagesTable();
			ImGui::End();
		}
	}

	void LogLayer::OnEvent(Event &event) {
		Layer::OnEvent(event);
	}

	void LogLayer::Clear() {
		m_Messages.clear();
	}

	void LogLayer::sink_it_(const spdlog::details::log_msg &msg) {
		if(m_Messages.size() >= s_MaxMessages)
			return;

		if(msg.level >= m_MinLogLevelToPopUp && msg.level < spdlog::level::off) {
			if(m_AutoPopUp) {
				if(!m_Show)
					m_ScrollToBottom = true;

				m_Show = true;
			}
		}

		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);

		m_Messages.emplace_back(formatted, msg, 15).GenerateHash(m_Messages.size() - 1);
	}

	void LogLayer::flush_() {}

	void LogLayer::LoggerCombo(Ref<spdlog::logger> logger) {
		static constexpr std::string_view logLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical\0Off";

		int32_t currentOption = logger->level();

		if(Combo(fmt::format("{} severity level", logger->name()), currentOption, logLevels)) {
			logger->set_level(static_cast<spdlog::level::level_enum>(currentOption));
		}
	}

	void LogLayer::SetUpTable() {
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 35, 0);
		ImGui::TableSetupColumn("TimeString", ImGuiTableColumnFlags_WidthFixed, 60, 1);
		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 60, 2);
		ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 80, 3);
		ImGui::TableSetupColumn("Short Desc", ImGuiTableColumnFlags_WidthFixed, 500, 4);
	}

	void LogLayer::BeginTable() {
		ImGui::BeginTable("", TABLE_COLUMN_COUNT, TABLE_FLAGS);
	}

	void LogLayer::EndTable() {
		ImGui::EndTable();
	}

	void LogLayer::PrintMessagesTable() {
		ImGui::BeginChild("Scrolling", {0, 0}, ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoSavedSettings);

		ImGui::PushID("LoggerTable");
		BeginTable();

		SetUpTable();

		PrintTable();

		EndTable();
		ImGui::PopID();

		if (m_AllowScrolling && m_ScrollToBottom)
			ImGui::SetScrollHereY(1.f);

		m_ScrollToBottom = false;

		ImGui::EndChild();
	}

	void LogLayer::PrintTable() {
		ImGui::TableHeadersRow();
		for (size_t i = 0; i < m_Messages.size(); ++i) {
			auto &message = m_Messages[i];

			if (!m_Filter->PassFilter(message.Message.Text.c_str()))
				continue;

			PrintMessage(i, message);
			if (message.Selected)
				PrintSelectedMessage(i, message);
		}
	}

	void LogLayer::PrintMessage(size_t i, MessageEntry &message) {
		ImGui::TableNextRow();

		ImGui::PushID(static_cast<int>(message.IdHash));
		ImGui::BeginGroup();

		ImGui::TableNextColumn();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, static_cast<ImU32>(message.Color.Code));
		ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);

		if(ImGui::IsItemHovered())
			ImGui::SetTooltip(message.Message.Text.c_str());

		if (ImGui::IsItemClicked())
			message.Selected = !message.Selected;

		ImGui::SameLine();
		ImGui::Text("%i", static_cast<int32_t>(i));
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(message.Message.Time.c_str());

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(to_string_view(message.Message.Level).data());

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(message.Message.Name.c_str());

		ImGui::TableNextColumn();
		const auto shortDesc = GetFirst(message.Message.Desc, 15);
		ImGui::TextUnformatted(shortDesc.data(), shortDesc.data() + shortDesc.size());

		ImGui::EndGroup();
		ImGui::PopID();
	}

	void LogLayer::PrintSelectedMessage(size_t i, MessageEntry &message) {
		EndTable();

		ImGui::PushID(static_cast<int>(message.IdSelectedHash));
		ImGui::BeginGroup();

		message.Source.Print();

		ImGui::Separator();
		Text("Time: {}", message.Message.Time);
		Text("Name: {}", message.Message.Name);
		Text("Level: {}", spdlog::level::to_string_view(message.Message.Level));

		ImGui::PushID(static_cast<int>(message.IdTextMultiline));
		InputTextMultiline("", message.Message.Desc, ImVec2{0, 0}, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopID();

		ImGui::Separator();

		if(ImGui::Button("Copy"))
			ImGui::SetClipboardText(message.Message.Text.c_str());

		ImGui::Separator();


		ImGui::EndGroup();
		ImGui::PopID();

		BeginTable();
	}

	std::string LogLayer::GetTimeAsString(const spdlog::log_clock::time_point &time) {
		return fmt::format("{:%T}", std::chrono::round<std::chrono::seconds>(time));
	}

	LogLayer::Source::Source(const spdlog::source_loc &loc) {
		File     = loc.filename ? loc.filename : "(null)";
		Function = loc.funcname ? loc.funcname : "(null)";
		Line     = loc.line;
	}

	LogLayer::Source::Source(const spdlog::details::log_msg &msg) : Source(msg.source) {
		ThreadId = msg.thread_id;
	}

	void LogLayer::Source::Print() {
		Text("File: {}", File);
		Text("Function: {}", Function);
		Text("Line: {}", Line);
		Text("Thread: {}", ThreadId);
	}

	LogLayer::Message::Message(const spdlog::memory_buf_t &formatted, const spdlog::details::log_msg &msg, size_t shortLen) {
		Name  = msg.logger_name.data();
		Desc  = msg.payload.data();
		ShortDesc = GetFirst(Desc, shortLen);
		Text  = fmt::to_string(formatted);
		Time  = GetTimeAsString(msg.time);
		Level = msg.level;
	}

	LogLayer::MessageEntry::MessageEntry(
		const spdlog::memory_buf_t &formatted,
		const spdlog::details::log_msg &msg,
		size_t shortLen
		) : Message(formatted, msg, shortLen),
		    Source(msg) {
		Color = SelectTextColor(msg.level);
	}

	void LogLayer::MessageEntry::GenerateHash(size_t i) {
		IdHash          = std::hash<std::string>()(fmt::format("{}{}", i, Message.Text));
		IdSelectedHash  = std::hash<std::string>()(fmt::format("Selected{}{}", i, Message.Text));
		IdTextMultiline = std::hash<std::string>()(fmt::format("Text{}\"{}\"",i, Message.Text));
	}
}
