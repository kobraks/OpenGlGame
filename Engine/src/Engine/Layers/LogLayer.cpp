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

#define COPY_LOG_BUFFERS(snapshot) LogBufferCopier _copier(m_Mutex, m_Messages, m_VisibleMessageIndices, snapshot)

namespace Engine {
	namespace Utils {
		static constexpr Color SelectTextColor(spdlog::level::level_enum level) {
			switch (level) {
			case spdlog::level::trace:
				return Color(204, 204, 204);
			case spdlog::level::debug:
				return Color(58, 150, 221);
			case spdlog::level::info:
				return Color(19, 161, 14);
			case spdlog::level::warn:
				return Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Color(0, 0, 0, 0);
			}
		}

		static constexpr Color SelectBackgroundColor(spdlog::level::level_enum level) {
			switch (level) {
			case spdlog::level::trace:
				return Color(204, 204, 204);
			case spdlog::level::debug:
				return Color(58, 150, 221);
			case spdlog::level::info:
				return Color(19, 161, 14);
			case spdlog::level::warn:
				return Color(249, 241, 165);
			case spdlog::level::err: ;
			case spdlog::level::critical:
				return Color(255, 0, 0);
			case spdlog::level::off:
			case spdlog::level::n_levels: default:
				return Color(0, 0, 0, 0);
			}
		}

		static constexpr std::string_view GetFirst(const std::string& string, size_t size) {
			return {string.begin(), string.size() > size ? string.begin() + size : string.end()};
		}

		static std::string GetTimeAsString(const spdlog::log_clock::time_point& time) {
			return fmt::format("{:%T}", std::chrono::round<std::chrono::seconds>(time));
		}

		static bool FilterChanged(std::string& filter, std::string& lastFilter) {
			if (filter != lastFilter) {
				lastFilter = filter;
				return true;
			}

			return false;
		}

		static bool FilterChanged(ImGuiTextFilter& filter, std::string& lastFilter) {
			if (filter.InputBuf != lastFilter) {
				lastFilter = filter.InputBuf;
				return true;
			}

			return false;
		}

		static void LoggerCombo(Ref<spdlog::logger> logger) {
			static constexpr std::string_view logLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical\0Off";

			int32_t currentOption = logger->level();

			if (Combo(fmt::format("{} severity level", logger->name()), currentOption, logLevels)) {
				logger->set_level(static_cast<spdlog::level::level_enum>(currentOption));
			}
		}

		static constexpr std::string Capitalize(const std::string& str) {
			if (str.empty()) return "";
			std::string result = str;
			result[0] = static_cast<char>(std::toupper(result[0]));
			return result;
		}
	}

	constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_Sortable;
	constexpr int TABLE_COLUMN_COUNT = 5;

	LogSource::LogSource(const spdlog::source_loc& loc) {
		File = loc.filename ? loc.filename : "(null)";
		Function = loc.funcname ? loc.funcname : "(null)";
		Line = loc.line;
	}

	LogSource::LogSource(const spdlog::details::log_msg& msg) : LogSource(msg.source) {
		ThreadId = msg.thread_id;
	}

	void LogSource::Print() const {
		Text("File: {}", File);
		Text("Function: {}", Function);
		Text("Line: {}", Line);
		Text("Thread: {}", ThreadId);
	}

	LogMessage::LogMessage(const spdlog::memory_buf_t& formatted, const spdlog::details::log_msg& msg) {
		Name = std::string(msg.logger_name.data(), msg.logger_name.size());
		Desc = std::string(msg.payload.data(), msg.payload.size());

		Text = to_string(formatted);
		Timestamp = msg.time;
		Time = Utils::GetTimeAsString(msg.time);
		Level = msg.level;
	}

	LogMessageEntry::LogMessageEntry(uint64_t index, const spdlog::memory_buf_t& formatted,
	                                 const spdlog::details::log_msg& msg) : Message(formatted, msg), Source(msg) {
		Color = Utils::SelectTextColor(msg.level);
		Index = index;
		GenerateHash(index);
	}

	void LogMessageEntry::GenerateHash(size_t i) {
		// 	IdHash = std::hash<std::string>()(fmt::format("{}{}", i, Message.Text));
		// IdSelectedHash = std::hash<std::string>()(fmt::format("Selected{}{}", i, Message.Text));
		// IdTextMultiline = std::hash<std::string>()(fmt::format("Text{}\"{}\"", i, Message.Text));

		auto timestampHash = Message.Timestamp.time_since_epoch().count();

		const std::string base = fmt::format("{}:{}", timestampHash, i);

		IdHash = std::hash<std::string_view>{}(base);
		IdSelectedHash = std::hash<std::string_view>{}("S" + base);
		IdTextMultiline = std::hash<std::string_view>{}("T" + base);
	}

	LogBufferCopier::LogBufferCopier(std::mutex& mutex, const std::vector<LogMessageEntry>& sourceMessages,
	                                 const std::vector<size_t>& sourceVisible, LogBufferSnapshot& outSnapshot) {
		std::lock_guard lock(mutex);
		outSnapshot.Messages.reserve(sourceMessages.size());
		outSnapshot.Messages = sourceMessages;

		outSnapshot.VisibleMessageIndices.reserve(sourceVisible.size());
		outSnapshot.VisibleMessageIndices = sourceVisible;
	}

	LogLayer::LogLayer() : Layer("LogLayer") {
		m_Filter = MakeScope<ImGuiTextFilter>();
		m_Messages.reserve(256);
		m_VisibleMessageIndices.reserve(256);
		m_SelectedMessageIndex = s_MaxMessages + 1;
	}

	void LogLayer::OnAttach() {
		Layer::OnAttach();
	}

	void LogLayer::OnImGuiRender() {
		constexpr std::string_view LogPopUpLevels = "Trace\0Debug\0Info\0Warn\0Error\0Critical";

		if (m_Show) {
			ImGui::SetNextWindowSize({700, 400}, ImGuiCond_FirstUseEver);
			ImGui::Begin("Log", &m_Show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking);

			if (ImGui::CollapsingHeader("Options")) {
				ToggleButton("Pause", &m_Pause);
				Utils::LoggerCombo(Log::GetApplicationLogger());
				Utils::LoggerCombo(Log::GetEngineLogger());
				Utils::LoggerCombo(Log::GetGLLogger());
				Utils::LoggerCombo(Log::GetScriptLogger());

				ToggleButton("Allow Scrolling", &m_AllowScrolling);
				ToggleButton("Auto Popup", &m_AutoPopUp);

				if (m_AutoPopUp) {
					Combo("Min severity to popup", m_MinLogLevelToPopUp, LogPopUpLevels);
				}

				ImGui::InputInt("Max num of messages", reinterpret_cast<int*>(&s_MaxMessages));
				ImGui::Separator();
			}

			if (ImGui::Button("Clear"))
				Clear();

			{
				m_Filter->Draw("Filter", 150.0f);
				ImGui::SameLine();

				ImGui::SetNextItemWidth(120.0f);

				const std::string severityLabel = m_SeverityFilter.empty() ? "Severity: All" : "Severity: " + Utils::Capitalize(m_SeverityFilter);
				if (ImGui::BeginCombo("##Severity", severityLabel.c_str())) {
					if (ImGui::Selectable("Severity All", m_SeverityFilter.empty())) {
						m_LastSeverityFilter = m_SeverityFilter;
						m_SeverityFilter.clear();
					}

					static const std::vector<std::string> severities = {
						"trace", "debug", "info", "warn", "error", "critical"
					};
					for (const auto& severity : severities) {
						const bool selected = (m_SeverityFilter == severity);
						if (ImGui::Selectable(Utils::Capitalize(severity).c_str(), selected)) {
							m_LastSeverityFilter = m_SeverityFilter;
							m_SeverityFilter = severity;
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();
				ImGui::SetNextItemWidth(130.f);
				const std::string loggerLabel = m_LoggerFilter.empty() ? "Logger: All" : "Logger: " + Utils::Capitalize(m_LoggerFilter);
				if (ImGui::BeginCombo("##Logger", loggerLabel.c_str())) {
					if (ImGui::Selectable("Logger: All", m_LoggerFilter.empty())) {
						m_LastLoggerFilter = m_LoggerFilter;
						m_LoggerFilter.clear();
					}

					static const std::vector<std::string> knownLoggers = {
						APPLICATION_LOGGER_NAME, ENGINE_LOGGER_NAME, GL_LOGGER_NAME, SCRIPT_LOGGER_NAME
					};
					for (const auto& logger : knownLoggers) {
						const bool selected = (m_LoggerFilter == logger);
						if (ImGui::Selectable(logger.c_str(), selected)) {
							m_LastLoggerFilter = m_LoggerFilter;
							m_LoggerFilter = logger;
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();

				bool clear = false;
				if (ImGui::Button("Clear Filters")) {
					m_LoggerFilter.clear();
					m_SeverityFilter.clear();
					m_Filter->Clear();
					clear = true;
				}

				std::lock_guard guard(m_Mutex);

				const bool filterChanged = Utils::FilterChanged(*m_Filter, m_LastFilterText);
				const bool loggerFilterChanged = Utils::FilterChanged(m_LoggerFilter, m_LastLoggerFilter);
				const bool severityFilterChanged = Utils::FilterChanged(m_SeverityFilter, m_LastSeverityFilter);

				if (clear || (filterChanged || loggerFilterChanged || severityFilterChanged))
					UpdateVisibleMessages();

				ImGui::PushID("Num of messages");
				if (m_Messages.size() >= s_MaxMessages)
					ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
				ImGui::LabelText("##MessagesCount", "Num of messages: %i/%i", m_Messages.size(), s_MaxMessages);
				if (m_Messages.size() >= s_MaxMessages) {
					ImGui::PopStyleColor();
					ImGui::TextColored({1, 0, 0, 1},
					                   "Buffer full for memory save no more messages will be shown please clear buffer");
				}
				if (m_Pause) {
					ImGui::TextColored({1, 0, 0, 1}, "Paused");
				}
				ImGui::PopID();
			}

			PrintMessagesTable();
			ImGui::End();
		}
	}

	void LogLayer::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(LogLayer::OnKeyPressEvent));

		Layer::OnEvent(event);
	}

	bool LogLayer::OnKeyPressEvent(KeyPressedEvent& event) {
		if (event.IsRepeat())
			return false;

		if (event.GetKeyCode() == Key::F2) {
			m_Show = !m_Show;
		}

		return false;
	}

	void LogLayer::Clear() {
		std::lock_guard guard(m_Mutex);

		m_Messages.clear();
		m_VisibleMessageIndices.clear();
	}

	void LogLayer::sink_it_(const spdlog::details::log_msg& msg) {
		std::lock_guard guard(m_Mutex);

		if (m_Pause || m_Messages.size() >= s_MaxMessages)
			return;

		if (msg.level >= m_MinLogLevelToPopUp && msg.level < spdlog::level::off) {
			if (m_AutoPopUp) {
				if (!m_Show)
					m_ScrollToBottom = true;

				m_Show = true;
			}
		}

		spdlog::memory_buf_t formatted;
		formatter_->format(msg, formatted);

		const auto& message = m_Messages.emplace_back(m_NextIndex++, formatted, msg);

		if (PassFilters(message)) {
			m_VisibleMessageIndices.emplace_back(message.Index);
		}
	}

	void LogLayer::flush_() {
	}

	bool LogLayer::PassFilters(const LogMessageEntry& message) const {
		const auto passesTextFilter = m_Filter->PassFilter(message.Message.Text.c_str());
		const auto passesLoggerNameFilter = m_LoggerFilter.empty() || message.Message.Name == m_LoggerFilter;
		const auto passesSeverityLevelFilter = m_SeverityFilter.empty() || to_string_view(message.Message.Level) ==
			m_SeverityFilter;

		return passesTextFilter && passesLoggerNameFilter && passesSeverityLevelFilter;
	}

	void LogLayer::UpdateVisibleMessages() {
		m_VisibleMessageIndices.clear();
		m_VisibleMessageIndices.reserve(m_Messages.capacity());
		for (size_t i = 0; i < m_Messages.size(); ++i) {
			if (PassFilters(m_Messages.at(i))) {
				m_VisibleMessageIndices.emplace_back(i);
			}
		}
	}

	void LogLayer::SortVisibleMessages() {
		if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
			if (sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
				const ImGuiTableColumnSortSpecs& specs = sortSpecs->Specs[0];

				auto compare = [&](size_t aIdx, size_t bIdx) {
					const auto& a = m_Messages[m_VisibleMessageIndices[aIdx]];
					const auto& b = m_Messages[m_VisibleMessageIndices[bIdx]];

					switch (specs.ColumnIndex) {
					case 0: return specs.SortDirection == ImGuiSortDirection_Ascending ? aIdx < bIdx : aIdx > bIdx;
					case 1: return specs.SortDirection == ImGuiSortDirection_Ascending
						               ? a.Message.Timestamp < b.Message.Timestamp
						               : a.Message.Timestamp > b.Message.Timestamp;
					case 2: return specs.SortDirection == ImGuiSortDirection_Ascending
						               ? a.Message.Level < b.Message.Level
						               : a.Message.Level > b.Message.Level;
					case 3: return specs.SortDirection == ImGuiSortDirection_Ascending
						               ? a.Message.Name < b.Message.Name
						               : a.Message.Name > b.Message.Name;
					}

					return false;
				};

				std::ranges::sort(m_VisibleMessageIndices, compare);
				sortSpecs->SpecsDirty = false;
			}
		}
	}

	void LogLayer::SetUpTable() {
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 35, 0);
		ImGui::TableSetupColumn("TimeString", ImGuiTableColumnFlags_WidthFixed, 60, 1);
		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 60, 2);
		ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 80, 3);
		ImGui::TableSetupColumn("Desc", ImGuiTableColumnFlags_WidthFixed, 500, 4);
	}

	void LogLayer::BeginTable() {
		ImGui::BeginTable("LoggerTable", TABLE_COLUMN_COUNT, TABLE_FLAGS);
	}

	void LogLayer::EndTable() {
		ImGui::EndTable();
	}

	void LogLayer::PrintMessagesTable() {
		ImGui::BeginChild("LogScrollArea", {0, 0}, ImGuiChildFlags_Border,
		                  ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoSavedSettings);

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

		uint64_t selectedCopy = 0;
		{
			std::lock_guard guard(m_Mutex);

			SortVisibleMessages();
			selectedCopy = m_SelectedMessageIndex;
		}

		LogBufferSnapshot snapshot;
		COPY_LOG_BUFFERS(snapshot);

		const int total = static_cast<int>(snapshot.VisibleMessageIndices.size());
		if (total == 0 || selectedCopy >= total) {
			PrintClippedTable(snapshot, static_cast<int>(snapshot.VisibleMessageIndices.size()));

			return;
		}

		//First half if selected message
		PrintClippedTable(snapshot, static_cast<int>(selectedCopy + 1));

		//Selected message
		if (selectedCopy <= snapshot.VisibleMessageIndices.size()) {
			const size_t actualIndex = snapshot.VisibleMessageIndices[selectedCopy];
			if (actualIndex < snapshot.Messages.size()) {
				PrintSelectedMessage(snapshot.Messages[actualIndex]);
			}
		}

		//after selected message
		const int remaining = total - static_cast<int>(selectedCopy + 1);
		PrintClippedTable(snapshot, remaining, static_cast<int>(selectedCopy + 1));
	}

	void LogLayer::PrintClippedTable(LogBufferSnapshot& snapshot, int itemCount, int startIndex) {
		if (itemCount > 0) {
			ImGuiListClipper clipper;
			clipper.Begin(itemCount);
			while (clipper.Step()) {
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
					const int globalIndex = (startIndex >= 0 ? startIndex + i : i);
					const auto index = snapshot.VisibleMessageIndices[globalIndex];

					if (index >= snapshot.Messages.size()) continue;

					auto& message = snapshot.Messages[index];
					PrintMessage(message);
				}
			}
		}
	}

	void LogLayer::PrintMessage(const LogMessageEntry& messageEntry) {
		ImGui::TableNextRow();
		const auto& message = messageEntry.Message;

		ImGui::PushID(static_cast<int>(messageEntry.IdHash));
		ImGui::BeginGroup();

		ImGui::TableNextColumn();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, messageEntry.Color.Code);
		ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns);
		if (ImGui::BeginPopupContextItem("LogRightClick")) {
			if (ImGui::Selectable("Copy")) {
				ImGui::SetClipboardText(message.Text.c_str());
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(message.Text.c_str());

		if (ImGui::IsItemClicked()) {
			if (m_SelectedMessageIndex != messageEntry.Index) {
				m_SelectedMessageIndex = messageEntry.Index;
			}
			else {
				m_SelectedMessageIndex = s_MaxMessages + 1;
			}
		}

		ImGui::SameLine();
		ImGui::Text("%llu", static_cast<size_t>(messageEntry.Index));
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(message.Time.c_str());

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(to_string_view(message.Level).data());

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(message.Name.c_str());

		ImGui::TableNextColumn();

		const ImVec2 textSize = ImGui::CalcTextSize(message.Desc.c_str());
		const float contentRegionWidth = ImGui::GetContentRegionAvail().x;

		const auto shortDesc = Utils::GetFirst(message.Desc, contentRegionWidth - textSize.x);
		ImGui::TextUnformatted(shortDesc.data(), shortDesc.data() + shortDesc.size());

		ImGui::EndGroup();
		ImGui::PopID();
	}

	void LogLayer::PrintSelectedMessage(LogMessageEntry& message) {
		if (message.Index != m_SelectedMessageIndex)
			return;

		EndTable();

		ImGui::PushID(static_cast<int>(message.IdSelectedHash));
		ImGui::BeginGroup();

		message.Source.Print();

		ImGui::Separator();
		Text("Time: {}", message.Message.Time);
		Text("Name: {}", message.Message.Name);
		Text("Level: {}", to_string_view(message.Message.Level));

		ImGui::PushID(static_cast<int>(message.IdTextMultiline));
		InputTextMultiline("", message.Message.Desc, ImVec2{0, 0}, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopID();

		ImGui::Separator();
		ImGui::Text("Debug: ");
		ImGui::Text("IdHash: %llu", message.IdHash);
		ImGui::Text("IdSelectedHash: %llu", message.IdSelectedHash);
		ImGui::Text("IdTextMultilineHash: %llu", message.IdTextMultiline);

		ImGui::Separator();

		if (ImGui::Button("Copy"))
			ImGui::SetClipboardText(message.Message.Text.c_str());

		ImGui::Separator();


		ImGui::EndGroup();
		ImGui::PopID();

		BeginTable();
	}
}
