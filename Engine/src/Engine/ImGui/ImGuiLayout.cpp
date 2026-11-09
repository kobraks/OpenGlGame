#include "pch.h"
#include "ImGuiLayout.h"

#include "Engine/ImGui/ImGuiWidgets.h"
#include "Engine/ImGui/ImGuiDrawUtils.h"
#include "Engine/ImGui/ImGuiScoped.h"

#include <ImGui/imgui.h>

namespace Engine {
	namespace Utils {
		static void PrepareLabel(std::string_view label) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			TextUnformatted(label);
		}
	}

	void BeginPropertyGrid(float labelColumnWidth) {
		BeginPropertyGrid("##PropertyGrid", labelColumnWidth);
	}

	void BeginPropertyGrid(std::string_view label, float labelColumnWidth) {

		ImGui::BeginTable(label.data() , 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadInnerX);

		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelColumnWidth);
		ImGui::TableSetupColumn("field", ImGuiTableColumnFlags_WidthStretch);
	}

	void EndPropertyGrid() {
		ImGui::EndTable();
	}

	void PropertyRow(std::string_view label) {
		Utils::PrepareLabel(label);

		ImGui::TableNextColumn();
	}

	void PropertyRow(std::string_view label, std::string_view tooltip) {
		Utils::PrepareLabel(label);

		ImGui::SameLine();
		HelpMarker(tooltip);

		ImGui::TableNextColumn();
	}

	void PropertyRow(std::string_view label, const std::function<void()>& drawField) {
		PropertyRow(label);

		drawField();
	}

	void PropertyRow(std::string_view label, std::string_view tooltip, const std::function<void()>& drawField) {
		PropertyRow(label, tooltip);

		drawField();
	}

	void PropertyRowWithReset(std::string_view label, const std::function<void()>& drawField, std::string_view tooltip,
		const std::function<void()> onReset, float buttonSize) {
		PropertyRow(label, tooltip);

		ScopedID propertyRow(label);
		drawField();
		ImGui::SameLine();
		if (onReset && ImGui::Button("Reset", ImVec2(buttonSize, 0))) {
			onReset();
		} else if (!onReset) {
			ImGui::BeginDisabled();
			ImGui::Button("Reset", ImVec2(buttonSize, 0));
			ImGui::EndDisabled();
		}
	}

	void PropertyRowWithReset(std::string_view label, const std::function<void()>& drawField,
	                          const std::function<void()> onReset, float buttonSize) {
		PropertyRow(label);

		ScopedID propertyRow(label);
		drawField();
		ImGui::SameLine();
		if (ImGui::Button("Reset", ImVec2(buttonSize, 0))) {
			onReset();
		}
	}

	void PropertyRowEnd() {
	}
}
