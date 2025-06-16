#pragma once
#include "Engine/Core/Base.h"
#include <ImGui/imgui.h>

namespace Engine {
	void SeverityCombo(std::string_view label, int32_t& selected, ImGuiComboFlags flags = 0);
	bool SeverityCombo(std::string_view label, std::string_view preview, int32_t& selected, ImGuiComboFlags flags = 0);

	void LoggerCombo(Ref<spdlog::logger> logger, ImGuiComboFlags flags = 0);
	bool LoggerCombo(std::string_view label, std::string_view preview, int32_t& selected, ImGuiComboFlags flags = 0);
}