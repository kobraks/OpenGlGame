#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Logging/LogTypes.h"

#include <imgui.h>

#include <vector>
#include <mutex>
#include <limits>
namespace Engine {
	class LogStorage;

	class LogPanel {
	public:
		LogPanel(Ref<LogStorage> storage);

		void Render();

		void SetVisible(bool visible) { m_Show = visible; }
		bool IsVisible() const { return m_Show; }

		void SetAllowScrolling(bool enable) { m_AllowScrolling = enable; }
		void SetAutoPopup(bool enable) { m_AutoPopUp = enable; }

		void ClearFilters();

		bool ShouldPopupFor(spdlog::level::level_enum level) const;

		uint64_t VisibleMessageCount() const;

		std::vector<std::size_t> CreateVisibleMessagesSnapshot() const;
	private:
		void RenderOptions();
		void RenderFilters();
		void RenderStatistics() const;
		void RenderMessagePanel();
		void RenderTableContent();
		void RenderClipped(const std::vector<std::size_t>& visibleMessages, int itemCount, int startIndex = -1);
		void RenderRow(const LogMessageEntry& entry);
		void RenderSelected(const LogMessageEntry& entry);

		void BeginTable();
		void EndTable();

		void SetUpTableColumns();

		void Clear();

		void SortVisibleMessages();

		bool PassFilters(const LogMessageEntry& entry) const;
		bool FiltersChanged();
		void UpdateVisibleIndices();

		Scope<ImGuiTextFilter> m_Filter;

		bool m_Show = true;
		bool m_Pause = false;
		bool m_AllowScrolling = true;
		bool m_ScrollToBottom = false;
		bool m_AutoPopUp = true;

		int32_t m_LoggerFilter = -1;
		int32_t m_SeverityFilter = -1;
		int32_t m_MinSeverityToPopup = static_cast<int32_t>(spdlog::level::warn);

		uint64_t m_SelectedIndex = std::numeric_limits<uint64_t>::max();

		std::string m_LastFilterText;
		int32_t m_LastLoggerFilter = -1;
		int32_t m_LastSeverityFilter = -1;

		mutable std::mutex m_Mutex;

		std::vector<std::size_t> m_VisibleMessages;
		Ref<LogStorage> m_Storage;
	};
}
