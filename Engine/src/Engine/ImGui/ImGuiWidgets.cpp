#include "pch.h"
#include "ImGuiWidgets.h"

#include "Engine/Core/Assert.h"

#include "Engine/ImGui/ImGuiScoped.h"

#include <imgui_internal.h>

namespace Engine {
	namespace Utils {
		static ImVec4 BlendColors(ImU32 onColor, ImU32 offColor, float t) {
			const ImVec4 on = ImGui::ColorConvertU32ToFloat4(onColor);
			const ImVec4 off = ImGui::ColorConvertU32ToFloat4(offColor);
			return ImLerp(on, off, t);
		}
	}

	bool ToggleButton(std::string_view id, std::string_view label, bool* value, std::string_view tooltip,
	                  const ToggleButtonStyle* style) {
		ENGINE_ASSERT(!id.empty());
		if (id.empty()) {
			throw std::invalid_argument("ToggleButton ID cannot be empty.");
		}

		bool clicked = false;

		const static ToggleButtonStyle defaultStyle{};
		const ToggleButtonStyle& s = style ? *style : defaultStyle;

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		const ImVec2 winPos = ImGui::GetCursorPos();

		const ImVec2 size = s.Size;
		const auto imStyle = ImGui::GetStyle();

		const float radius = size.y * 0.5f;

		constexpr float SpeedAnim = 10.0f;

		ImDrawList* draw = ImGui::GetWindowDrawList();

		thread_local std::string tmpID;
		const char* cID = Utils::EnsureNullTerminated(id, tmpID);

		ScopedID scopedID(cID);
		ScopedGroup buttonGroup;

		//Button logic
		if (ImGui::InvisibleButton("##ToggleButton", size)) {
			*value = !*value;
			clicked = true;
		}

		const bool hovered = ImGui::IsItemHovered();
		const bool focused = ImGui::IsItemFocused();

		if (focused && ImGui::IsKeyPressed(ImGuiKey_Space))
			*value = !*value;

		const auto& context = *GImGui;
		const auto widgetID = context.LastItemData.ID;
		const float target = *value ? 1.0f : 0.0f;

		float& animation = *ImGui::GetStateStorage()->GetFloatRef(widgetID, target);
		animation = ImLerp(animation, target, ImGui::GetIO().DeltaTime * SpeedAnim);

		const ImVec4 colorBlend = Utils::BlendColors(s.OnColor, s.OffColor, animation);
		const ImU32 colorBG = hovered
			                      ? ImGui::GetColorU32(ImLerp(colorBlend, ImVec4(1, 1, 1, 1), s.HoverBrightness))
			                      : ImGui::GetColorU32(ImLerp(colorBlend, ImVec4(1, 1, 1, 1), s.IdleDarkening));

		const auto posMax = ImVec2(pos.x + size.x, pos.y + size.y);
		// Background
		draw->AddRectFilled(pos, posMax, colorBG, radius);
		// Border
		draw->AddRect(pos, posMax, s.BorderColor, radius, 0, 1.0f);

		// Thumb
		const float thumbX = ImLerp(pos.x + radius, pos.x + size.x - radius, animation);
		const ImVec2 center = {thumbX, pos.y + radius};
		draw->AddCircleFilled(center, radius - 2.0f, ImGui::GetColorU32(Utils::BlendColors(s.ThumbOnColor, s.ThumbOffColor, animation)));

		// Tooltip
		if (!tooltip.empty() && hovered) {
			SetTooltip(tooltip);
		}

		// Label (to right of toggle)
		if (!label.empty()) {
			ScopedID labelID("ToggleLabel");
			ImGui::SameLine(winPos.x + s.Size.x + imStyle.ItemSpacing.x);
			TextUnformatted(label);
		}

		return clicked;
	}

	bool ToggleButton(std::string_view label, bool* value, std::string_view tooltip, const ToggleButtonStyle* style) {
		return ToggleButton(label, label, value, tooltip, style);
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
