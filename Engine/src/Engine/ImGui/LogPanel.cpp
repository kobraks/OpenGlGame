#include "pch.h"
#include "LogPanel.h"

#include "ImGuiInput.h"
#include "Engine/ImGui/ImGuiWidgets.h"
#include "Engine/ImGui/ImGuiScoped.h"
#include "Engine/ImGui/LogWidgets.h"

#include "Engine/Logging/LogUtils.h"
#include "Engine/Logging/LogStorage.h"
#include "Engine/Utils/CoreUtility.h"

namespace Engine {
	constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Sortable;
	constexpr int TABLE_COLUMN_COUNT = 5;
	constexpr uint64_t INVALID_INDEX = std::numeric_limits<uint64_t>::max();


	LogPanel::LogPanel(Ref<LogStorage> storage) : m_Filter(MakeScope<ImGuiTextFilter>()), m_Storage(storage) {
		m_Storage->SetNewMessageCallback([this](const LogMessageEntry& entry, uint64_t index) {
			if (PassFilters(entry)) {
				std::lock_guard lock(m_Mutex);

				if (entry.Message.Level >= m_MinSeverityToPopup && entry.Message.Level < spdlog::level::off) {
					if (m_AutoPopUp) {
						if (!m_Show)
							m_ScrollToBottom = true;

						m_Show = true;
					}
				}

				m_VisibleMessages.emplace_back(index);
			}
		});

		m_SelectedIndex = INVALID_INDEX;
	}

	void LogPanel::Render() {
		if (!m_Show)
			return;

		ImGui::SetNextWindowSize({ 700, 400 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Log", &m_Show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking);

		RenderOptions();
		bool clear = false;
		if (ImGui::Button("Clear Buffer")) {
			Clear();
			clear = true;
		}

		RenderFilters();

		if (ImGui::Button("Clear Filters")) {
			ClearFilters();
		}

		RenderStatistics();

		if (FiltersChanged() || clear) {
			UpdateVisibleIndices();
		}

		RenderMessagePanel();


		ImGui::End();
	}

	void LogPanel::ClearFilters() {
		m_Filter->Clear();
		m_LoggerFilter = -1;
		m_SeverityFilter = -1;
	}

	bool LogPanel::ShouldPopupFor(spdlog::level::level_enum level) const {
		return m_AutoPopUp && !m_Pause && level >= m_MinSeverityToPopup;
	}

	uint64_t LogPanel::VisibleMessageCount() const {
		std::lock_guard lock(m_Mutex);
		return m_VisibleMessages.size();
	}

	std::vector<std::size_t> LogPanel::CreateVisibleMessagesSnapshot() const {
		std::lock_guard lock(m_Mutex);
		return m_VisibleMessages; // coping for thread safety.
	}

	void LogPanel::RenderOptions() {
		if (ImGui::CollapsingHeader("Options")) {
			ToggleButton("Pause", &m_Pause);
			LoggerCombo(Log::GetApplicationLogger());
			LoggerCombo(Log::GetEngineLogger());
			LoggerCombo(Log::GetGLLogger());
			LoggerCombo(Log::GetScriptLogger());

			ToggleButton("Allow Scrolling", &m_AllowScrolling);
			ToggleButton("Auto Popup", &m_AutoPopUp);

			if (m_AutoPopUp) {
				SeverityCombo("Min severity to popup", m_MinSeverityToPopup);
			}

			uint64_t maxMessages = m_Storage->GetMaxMessages();
			uint64_t setMessages = maxMessages;
			ImGui::InputInt("Max num of messages", reinterpret_cast<int*>(&setMessages));
			ImGui::Separator();

			if (setMessages != maxMessages)
				m_Storage->SetMaxMessages(maxMessages);
		}
	}

	void LogPanel::RenderFilters() {
		ScopedID filtersID("Filters");

		m_Filter->Draw("Filter", 150.0f);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);

		const std::string severityLevel = m_SeverityFilter == -1 ? "Severity: All" : "Severity: " + std::string(Utils::GetSeverities()[m_SeverityFilter]);
		auto severityFilter = m_SeverityFilter;
		if (SeverityCombo("##SeverityCombo", severityLevel, severityFilter) ){
			m_LastSeverityFilter = m_SeverityFilter;
			m_SeverityFilter = severityFilter;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(130.0f);
		const std::string loggerName = m_LoggerFilter == -1 ? "Logger: All" : "Logger: " + std::string(Utils::GetKnownLoggersName()[m_LoggerFilter]);
		auto loggerFilter = m_LoggerFilter;
		if (LoggerCombo("##LoggerCombo", loggerName, loggerFilter)) {
			m_LastLoggerFilter = m_LoggerFilter;
			m_LoggerFilter = loggerFilter;
		}
	}

	void LogPanel::RenderStatistics() const {
		ScopedID messagesID("Statistic");
		ScopedStyleVar styleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

		const auto maxMessages = m_Storage->GetMaxMessages();
		const auto bufferSize = m_Storage->GetMessageCount();
		if (bufferSize >= maxMessages)
			ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0, 0, 1 });
		ImGui::Text("Num of messages: %i/%i", static_cast<int>(bufferSize), static_cast<int>(maxMessages));

		if (bufferSize >= maxMessages) {
			ImGui::PopStyleColor();
			ImGui::TextColored({ 1, 0, 0, 1 },
				"Buffer full for memory save no more messages will be shown please clear buffer");
		}

		ImGui::Text("Messages dropped: %i", static_cast<int>(m_Storage->GetDroppedMessageCount()));
		{
			std::lock_guard lock(m_Mutex);
			ImGui::Text("Visible messages: %i", static_cast<int>(m_VisibleMessages.size()));
		}

		if (m_Pause) {
			ImGui::TextColored({ 1, 0, 0, 1 }, "Paused");
		}
	}

	void LogPanel::RenderMessagePanel() {
		ImGui::BeginChild("LogScrollArea", { 0, 0 }, ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoSavedSettings);

		{
			ScopedID loggerTable("LoggerTable");
			BeginTable();
			SetUpTableColumns();

			RenderTableContent();

			EndTable();

			if (m_AllowScrolling && m_ScrollToBottom)
				ImGui::SetScrollHereY(1.f);

			m_ScrollToBottom = false;
		}

		ImGui::EndChild();
	}

	void LogPanel::RenderTableContent() {
		ImGui::TableHeadersRow();
		SortVisibleMessages();
		const auto visibleMessages = CreateVisibleMessagesSnapshot();
		const int total = static_cast<int>(visibleMessages.size());
		int selectedIndex = static_cast<int>(visibleMessages.size() + 1);
			
		if (total == 0) {
			return;
		}

		if (selectedIndex != m_Storage->GetMaxMessages()) {
			for (int i = 0; i < total; ++i) {
				if (m_SelectedIndex == visibleMessages[i]) {
					selectedIndex = i;
				}
			}
		}

		// Print all if no message is selected
		if (total != 0 && selectedIndex > total) {
			RenderClipped(visibleMessages, total);

			return;
		}

		// First half if selected message
		RenderClipped(visibleMessages, selectedIndex + 1);

		// Selected message
		if (selectedIndex <= visibleMessages.size()) {
			const std::size_t actualIndex = visibleMessages[selectedIndex];
			if (actualIndex < m_Storage->GetMessageCount()) {
				RenderSelected(m_Storage->GetMessageEntry(actualIndex));
			}
		}

		// After selected message
		const int remaining = total - (selectedIndex + 1);
		RenderClipped(visibleMessages, remaining, selectedIndex + 1);
	}

	void LogPanel::RenderClipped(const std::vector<std::size_t>& visibleMessages, int itemCount, int startIndex) {
		if (itemCount > 0) {
			ImGuiListClipper clipper;

			clipper.Begin(itemCount);
			while (clipper.Step()) {
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
					const int globalIndex = (startIndex >= 0 ? startIndex + i : i);
					const auto index = visibleMessages[globalIndex];

					if (index >= m_Storage->GetMessageCount()) continue;

					auto message = m_Storage->GetMessageEntry(index);
					RenderRow(message);
				}
			}
		}
	}

	void LogPanel::RenderRow(const LogMessageEntry& entry) {
		ImGui::TableNextRow();
		const auto& message = entry.Message;
		ScopedID ID(static_cast<int>(entry.Hash.IDHash));
		ScopedGroup messageGroup;

		ImGui::TableNextColumn();
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, entry.Color.Code);
		ImGui::Selectable("##selectable", false, ImGuiSelectableFlags_SpanAllColumns);

		if (ImGui::BeginPopupContextItem("LogRightClick")) {
			if (ImGui::Selectable("Copy")) {
				ImGui::SetClipboardText(message.FullMessage.c_str());
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsItemHovered()) {
			SetTooltip(message.FullMessage);
		}

		if (ImGui::IsItemClicked()) {
			if (m_SelectedIndex != entry.Index) {
				m_SelectedIndex = entry.Index;
			} else {
				m_SelectedIndex = INVALID_INDEX;
			}
		}

		ImGui::SameLine();
		TextUnformatted("{}", entry.Index);

		ImGui::TableNextColumn();
		TextUnformatted(message.TimeString);

		ImGui::TableNextColumn();
		TextUnformatted(std::string_view(to_string_view(message.Level)));

		ImGui::TableNextColumn();
		TextUnformatted(message.LoggerName);

		ImGui::TableNextColumn();

		const ImVec2 textSize = ImGui::CalcTextSize(message.Description.c_str());
		const float contentRegionWidth = ImGui::GetContentRegionAvail().x;

		const auto shortDesc = Utils::GetFirst(message.Description, static_cast<std::size_t>(contentRegionWidth - textSize.x));
		TextUnformatted(shortDesc);
	}

	void LogPanel::RenderSelected(const LogMessageEntry& entry) {
		constexpr ImGuiTreeNodeFlags HEADER_FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (entry.Index != m_SelectedIndex)
			return;

		const auto& message = entry.Message;
		const auto& hashes = entry.Hash;
		const auto& source = entry.Source;

		// Break table
		EndTable();

		{
			ScopedID selected(static_cast<int>(hashes.IDSelectedHash));
			ScopedGroup messageGroup;

			ImGui::Separator();
			if (ImGui::CollapsingHeader("Source", HEADER_FLAGS)) {
				TextUnformatted("Function: {}", source.FunctionName);
				TextUnformatted("Line: {}", source.LineNumber);
				TextUnformatted("Thread: {}", source.ThreadID);
			}

			if (ImGui::CollapsingHeader("Metadata", HEADER_FLAGS)) {
				TextUnformatted("Time: {}", message.TimeString);
				TextUnformatted("Logger: {}", message.LoggerName);
				TextUnformatted("Level: {}", to_string_view(message.Level));
			}

			if (ImGui::CollapsingHeader("Message", HEADER_FLAGS)) {
				ScopedID multilineID(static_cast<int>(hashes.IDTextMultiline));

				// ImGui::PushTextWrapPos(0.0f);
				InputTextMultiline("", const_cast<std::string&>(message.Description), ImVec2{ 0, 0 }, ImGuiInputTextFlags_ReadOnly);
				// ImGui::TextWrapped("%s", message.Message.Desc.c_str());
				// ImGui::PopTextWrapPos();
			}

			if (ImGui::CollapsingHeader("Debug", HEADER_FLAGS)) {
				TextUnformatted("IDHash: {}", hashes.IDHash);
				TextUnformatted("IDSelectedHash: {}", hashes.IDSelectedHash);
				TextUnformatted("IDTextMultilineHash: {}", hashes.IDTextMultiline);
			}
			
			ImGui::Separator();

			if (ImGui::Button("Copy")) {
				ImGui::SetClipboardText(message.FullMessage.c_str());
			}

			ImGui::Separator();
		}

		BeginTable();
		SetUpTableColumns();
	}

	void LogPanel::BeginTable() {
		ImGui::BeginTable("LoggerTable", TABLE_COLUMN_COUNT, TABLE_FLAGS);
	}

	void LogPanel::EndTable() {
		ImGui::EndTable();
	}

	void LogPanel::SetUpTableColumns() {
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 35, 0);
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 60, 1);
		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_WidthFixed, 60, 2);
		ImGui::TableSetupColumn("Logger", ImGuiTableColumnFlags_WidthFixed, 80, 3);
		ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed, 500, 4);
	}

	void LogPanel::Clear() {
		std::lock_guard lock(m_Mutex);
		m_VisibleMessages.clear();
		m_Storage->Clear();
		m_SelectedIndex = INVALID_INDEX;
	}

	void LogPanel::SortVisibleMessages() {
		if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs()) {
			if (sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
				const ImGuiTableColumnSortSpecs& specs = sortSpecs->Specs[0];

				auto compare = [&](size_t aIdx, size_t bIdx) {
					const auto& a = m_Storage->GetMessageEntry(aIdx);
					const auto& b = m_Storage->GetMessageEntry(bIdx);

					const auto& messageA = a.Message;
					const auto& messageB = b.Message;

					switch (specs.ColumnIndex) {
					case 0: return specs.SortDirection == ImGuiSortDirection_Ascending ? aIdx < bIdx : aIdx > bIdx;
					case 1: return specs.SortDirection == ImGuiSortDirection_Ascending ? messageA.Timestamp < messageB.Timestamp : messageA.Timestamp > messageB.Timestamp;
					case 2: return specs.SortDirection == ImGuiSortDirection_Ascending ? messageA.Level < messageB.Level : messageA.Level > messageB.Level;
					case 3: return specs.SortDirection == ImGuiSortDirection_Ascending ? messageA.LoggerID < messageB.LoggerID : messageA.LoggerID > messageB.LoggerID;
					}

					return false;
					};

				std::lock_guard guard(m_Mutex);
				std::ranges::sort(m_VisibleMessages, compare);
				sortSpecs->SpecsDirty = false;
			}
		}
	}

	bool LogPanel::PassFilters(const LogMessageEntry& entry) const {
		const auto passesTextFilter = m_Filter->PassFilter(entry.Message.FullMessage.c_str());
		const auto passesLoggerNameFilter = m_LoggerFilter == -1 || entry.Message.LoggerID == m_LoggerFilter;
		const auto passesSeverityLevelFilter = m_SeverityFilter == -1 || entry.Message.Level == m_SeverityFilter;

		return passesTextFilter && passesLoggerNameFilter && passesSeverityLevelFilter;
	}

	bool LogPanel::FiltersChanged() {
		const bool filterChanged = Utils::FilterChanged(*m_Filter, m_LastFilterText);
		const bool loggerFilterChanged = Utils::FilterChanged(m_LoggerFilter, m_LastLoggerFilter);
		const bool severityFilterChanged = Utils::FilterChanged(m_SeverityFilter, m_LastSeverityFilter);

		return filterChanged || loggerFilterChanged || severityFilterChanged;
	}

	void LogPanel::UpdateVisibleIndices() {
		const auto messageCount = m_Storage->GetMessageCount();

		std::lock_guard lock(m_Mutex);
		m_VisibleMessages.clear();
		m_VisibleMessages.reserve(messageCount);
		for (size_t i = 0; i < messageCount; ++i) {
			const auto message = m_Storage->GetMessageEntry(i);
			if (PassFilters(message)) {
				m_VisibleMessages.emplace_back(i);
			}
		}
	}
}
