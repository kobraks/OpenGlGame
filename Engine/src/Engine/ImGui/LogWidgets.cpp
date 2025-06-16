#include "pch.h"
#include "LogWidgets.h"

#include "Engine/Utils/StdUtils.h"
#include "Engine/Logging/LogUtils.h"

namespace Engine {
	void SeverityCombo(std::string_view label, int32_t& selected, ImGuiComboFlags flags) {
		const auto& severities = Utils::GetSeverities();

		if (ImGui::BeginCombo(Utils::EnsureNullTerminated(label), Utils::EnsureNullTerminated(severities[selected]), flags)) {
			for (std::size_t i = 0; i < severities.size(); ++i) {
				const auto& severity = severities[i];

				if (ImGui::Selectable(Utils::EnsureNullTerminated(severity), selected == static_cast<int>(i))) {
					selected = static_cast<int32_t>(i);
				}
			}

			ImGui::EndCombo();
		}
	}

	bool SeverityCombo(std::string_view label, std::string_view preview, int32_t& selected, ImGuiComboFlags flags) {
		const auto& severities = Utils::GetSeverities();
		bool result = false;

		if (ImGui::BeginCombo(Utils::EnsureNullTerminated(label), Utils::EnsureNullTerminated(preview), flags)) {
			if (ImGui::Selectable("Severity: All", selected == -1)) {
				selected = -1;
				result = true;
			}

			for (std::size_t i = 0; i < severities.size(); ++i) {
				const auto& severity = severities[i];

				if (ImGui::Selectable(Utils::EnsureNullTerminated(severity), selected == static_cast<int32_t>(i))) {
					selected = static_cast<int32_t>(i);
					result = true;
				}
			}

			ImGui::EndCombo();
		}

		return result;
	}

	void LoggerCombo(Ref<spdlog::logger> logger, ImGuiComboFlags flags) {
		int32_t currentOption = logger->level();
		SeverityCombo(fmt::format("{} severity level", logger->name()), currentOption);

		logger->set_level(static_cast<spdlog::level::level_enum>(currentOption));
	}

	bool LoggerCombo(std::string_view label, std::string_view preview, int32_t& selected, ImGuiComboFlags flags) {
		const auto& knownLoggers = Utils::GetKnownLoggersName();
		bool result = false;

		if (ImGui::BeginCombo(Utils::EnsureNullTerminated(label), Utils::EnsureNullTerminated(preview), flags)) {
			if (ImGui::Selectable("Logger: All", selected == -1)) {
				selected = -1;
				result = true;
			}

			for (std::size_t i = 0; i < knownLoggers.size(); ++i) {
				const auto& logger = knownLoggers[i];

				if (ImGui::Selectable(Utils::EnsureNullTerminated(logger), selected == static_cast<int32_t>(i))) {
					selected = static_cast<int32_t>(i);
					result = true;
				}
			}

			ImGui::EndCombo();
		}

		return result;
	}
}