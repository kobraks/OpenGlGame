#include "pch.h"
#include "ImGuiDrawUtils.h"

#include "Engine/ImGui/ImGuiWidgets.h"

#include "Engine/Utils/StdUtils.h"

namespace Engine {
	void HelpMarker(std::string_view description) {
		ImGui::TextDisabled("(?)");

		if (ImGui::IsItemHovered())
			SetTooltip(description);
	}
}