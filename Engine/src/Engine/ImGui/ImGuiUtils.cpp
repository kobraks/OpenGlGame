#include "pch.h"
#include "Engine/ImGui/ImGuiUtils.h"
#include "Engine/Core/Assert.h"

#include <imgui_internal.h>

namespace Engine {
	struct InputTextCallbackData {
		std::string *String;
		CallbackFunction Callback;
		void *UserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData *data) {
		auto userData = static_cast<InputTextCallbackData*>(data->UserData);

		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			auto string = userData->String;
			ENGINE_ASSERT((data->Buf == string->c_str()));
			string->resize(data->BufTextLen);
			data->Buf = const_cast<char*>(string->c_str());
		}
		else if(userData->Callback) {
			data->UserData = userData->UserData;
			return userData->Callback(data);
		}

		return 0;
	}

	bool InputText(
		std::string_view label,
		std::string &string,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void *userData
		) {
		ENGINE_ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0);

		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallbackData data{&string, callback, userData};
		return ImGui::InputText(label.data(), string.data(), string.capacity(), flags, InputTextCallback, &data);
	}

	bool Combo(std::string_view label, int32_t &currentItem, std::string_view itemList, int32_t maxHeightInItems) {
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), maxHeightInItems);
	}

	bool InputTextMultiline(
		std::string_view label,
		std::string &string,
		const ImVec2 &size,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void *userData
		) {
		ENGINE_ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0);

		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallbackData data{&string, callback, userData};

		return ImGui::InputTextMultiline(
		                                 label.data(),
		                                 string.data(),
		                                 string.capacity(),
		                                 size,
		                                 flags,
		                                 InputTextCallback,
		                                 &data
		                                );
	}

	bool ToggleButton(std::string_view name, bool *v) {
		//Soruce https://github.com/ocornut/imgui/issues/1537
		const char *id = name.data();
		bool clicked   = false;

		const ImGuiStyle &style = ImGui::GetStyle();
		const ImVec4 *colors    = style.Colors;
		const ImVec2 pos        = ImGui::GetCursorScreenPos();
		const ImVec2 winPos     = ImGui::GetCursorPos();
		ImDrawList *drawList    = ImGui::GetWindowDrawList();

		const float height = ImGui::GetFrameHeight();
		const float width = height * 1.55f;
		const float radius = height * 0.50f;

		ImGui::PushID(id);
		ImGui::BeginGroup();
		ImGui::InvisibleButton(id, ImVec2(width, height));
		if (ImGui::IsItemClicked()) {
			*v = !*v;
			clicked = true;
		}

		ImGuiContext &gg = *GImGui;

		constexpr float ANIM_SPEED = 0.085f;
		float time = *v ? 1.f : 0.f;

		if (gg.LastActiveId == gg.CurrentWindow->GetID(id)) {
			const float tAnim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);

			time = *v ? (tAnim) : (1.f - tAnim);
		}

		ImU32 colorBG;
		if (ImGui::IsItemHovered())
			colorBG = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), *v ? colors[ImGuiCol_ButtonActive] : ImVec4{0.78f, 0.78f, 0.78f, 1.0f}, time));
		else
			colorBG = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), *v ? colors[ImGuiCol_Button] : ImVec4{0.85f, 0.85f, 0.85f, 1.0f}, time));

		drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), colorBG, height * 0.5f);
		drawList->AddCircleFilled(ImVec2(pos.x + radius + time * (width - radius * 2.0f), pos.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

		ImGui::SameLine((winPos.x + width + style.ItemSpacing.x));

		ImGui::PushID("Label");
		ImGui::TextUnformatted(id);
		ImGui::PopID();

		ImGui::EndGroup();
		ImGui::PopID();
		return clicked;
	}
}
