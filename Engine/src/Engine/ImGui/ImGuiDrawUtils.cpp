#include "pch.h"
#include "ImGuiDrawUtils.h"

#include "Engine/Utils/StdUtils.h"

namespace Engine {
	void HelpMarker(std::string_view description) {
		ImGui::TextDisabled("(?)");

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", Utils::EnsureNullTerminated(description));
	}
}