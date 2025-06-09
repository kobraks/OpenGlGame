#include "pch.h"
#include "ImGuiWidgets.h"

#include "Engine/Core/Assert.h"

#include "Engine/ImGui/ImGuiScoped.h"

#include <imgui_internal.h>

namespace Engine {
	// bool ToggleButton(std::string_view name, bool *enabled) {
// 	//Soruce https://github.com/ocornut/imgui/issues/1537
// 	const char *id = name.data();
// 	bool clicked   = false;
//
// 	const ImGuiStyle &style = ImGui::GetStyle();
// 	const ImVec4 *colors    = style.Colors;
// 	const ImVec2 pos        = ImGui::GetCursorScreenPos();
// 	const ImVec2 winPos     = ImGui::GetCursorPos();
// 	ImDrawList *drawList    = ImGui::GetWindowDrawList();
//
// 	const float height = ImGui::GetFrameHeight();
// 	const float width = height * 1.55f;
// 	const float radius = height * 0.50f;
//
// 	constexpr float ANIM_SPEED = 0.085f;
//
// 	ScopedID buttonID(id);
// 	ScopedGroup buttonGroup;
//
// 	ImGui::InvisibleButton(id, ImVec2(width, height));
// 	if (ImGui::IsItemClicked()) {
// 		*enabled = !*enabled;
// 		clicked = true;
// 	}
//
// 	const ImGuiContext& context = *GImGui;
//
// 	float time = *enabled ? 1.f : 0.f;
//
// 	if (context.LastActiveId == context.CurrentWindow->GetID(id)) {
// 		const float timeAnim = ImSaturate(context.LastActiveIdTimer / ANIM_SPEED);
//
// 		time = *enabled ? (timeAnim) : (1.f - timeAnim);
// 	}
//
// 	ImU32 colorBG;
// 	if (ImGui::IsItemHovered())
// 		colorBG = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), *enabled ? colors[ImGuiCol_ButtonActive] : ImVec4{ 0.78f, 0.78f, 0.78f, 1.0f }, time));
// 	else
// 		colorBG = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), *enabled ? colors[ImGuiCol_Button] : ImVec4{ 0.85f, 0.85f, 0.85f, 1.0f }, time));
//
// 	drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), colorBG, height * 0.5f);
// 	drawList->AddCircleFilled(ImVec2(pos.x + radius + time * (width - radius * 2.0f), pos.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
//
// 	ImGui::SameLine((winPos.x + width + style.ItemSpacing.x));
//
// 	{
// 		ScopedID labelID("Label");
// 		ImGui::TextUnformatted(id);
//
// 	}
//
// 	return clicked;
// }

	bool ToggleButton(std::string_view label, bool* value, std::string_view tooltip, const ToggleButtonStyle* style) {
		bool clicked = false;

		const ToggleButtonStyle defaultStyle{};
		const ToggleButtonStyle& s = style ? *style : defaultStyle;

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		const ImVec2 winPos = ImGui::GetCursorPos();

		const ImVec2 size = s.Size;
		const auto imStyle = ImGui::GetStyle();

		const float radius = size.y * 0.5f;

		constexpr float SpeedAnim = 10.0f;

		ImDrawList* draw = ImGui::GetWindowDrawList();

		ScopedID buttonID(label);
		ScopedGroup buttonBody;

		// Button logic
		if (ImGui::InvisibleButton(label.data(), size)) {
			*value = !*value;
			clicked = true;
		}

		const bool hovered = ImGui::IsItemHovered();
		const bool focused = ImGui::IsItemFocused();

		if (focused && ImGui::IsKeyPressed(ImGuiKey_Space))
			*value = !*value;


		const auto& context = *GImGui;
		const auto widgetID = context.LastItemData.ID;
		const float target = *value ? 1.0f : 0.f;

		float& animation = *ImGui::GetStateStorage()->GetFloatRef(widgetID, target);
		animation = ImLerp(animation, target, ImGui::GetIO().DeltaTime * SpeedAnim);

		const ImVec4 colorOff = ImGui::ColorConvertU32ToFloat4(s.OffColor);
		const ImVec4 colorOn = ImGui::ColorConvertU32ToFloat4(s.OnColor);
		const ImVec4 colorBlend = ImLerp(colorOff, colorOn, animation);

		ImU32 colorBG;
		if (hovered) {
			colorBG = ImGui::GetColorU32(ImLerp(colorBlend, ImVec4(1, 1, 1, 1), s.HoverBrightness));
		}
		else {
			colorBG = ImGui::GetColorU32(ImLerp(colorBlend, ImVec4(1, 1, 1, 1), s.IdleDarkening));
		}


		const auto posMax = ImVec2(pos.x + size.x, pos.y + size.y);

		// Background
		draw->AddRectFilled(pos, posMax, colorBG, radius);
		// Border
		draw->AddRect(pos, posMax, s.BorderColor, radius, 0, 1.0f);

		// Thumb
		const float thumbX = ImLerp(pos.x + radius, pos.x + size.x - radius, animation);
		const ImVec2 center = { thumbX, pos.y + radius };
		draw->AddCircleFilled(center, radius - 2.0f, s.ThumbColor);

		// Tooltip
		if (!tooltip.empty() && hovered) {
			ImGui::SetTooltip("%s", tooltip.data());
		}

		// Label (to right of toggle)
		if (!label.empty() && !label.starts_with("##")) {
			ScopedID labelID("Label");
			ImGui::SameLine(winPos.x + s.Size.x + imStyle.ItemSpacing.x);
			ImGui::TextUnformatted(label.data());
		}

		return clicked;
	}

	bool Combo(std::string_view label, int32_t& currentItem, std::string_view itemList, int32_t maxHeightInItems) {
		static std::string tmpLabel;
		static std::string tmpItemList;
		const auto cLabel = Utils::EnsureNullTerminated(label, tmpLabel);
		const auto cItemList = Utils::EnsureNullTerminated(itemList, tmpItemList);

		return ImGui::Combo(cLabel, &currentItem, cItemList, maxHeightInItems);
	}

	bool DragVec2(std::string_view label, glm::vec2& vec, float speed, float min, float max, const char* format,
		ImGuiSliderFlags flags) {
		return ImGui::DragFloat2(Utils::EnsureNullTerminated(label), &vec.x, speed, min, max, format, flags);
	}

	bool DragVec3(std::string_view label, glm::vec3& vec, float speed, float min, float max, const char* format,
		ImGuiSliderFlags flags) {
		return ImGui::DragFloat3(Utils::EnsureNullTerminated(label), &vec.x, speed, min, max, format, flags);
	}

	bool DragVec4(std::string_view label, glm::vec4& vec, float speed, float min, float max, const char* format,
		ImGuiSliderFlags flags) {
		return ImGui::DragFloat4(Utils::EnsureNullTerminated(label), &vec.x, speed, min, max, format, flags);
	}
}