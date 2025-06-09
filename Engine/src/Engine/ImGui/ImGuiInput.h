#pragma once
#include "Engine/Core/Base.h"

#include <ImGui/imgui.h>

namespace Engine {
	using CallbackFunction = std::function<int(ImGuiInputTextCallbackData*)>;

	bool InputText(
		std::string_view label,
		std::string& string,
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void* userData = nullptr
	);

	bool InputTextMultiline(
		std::string_view label,
		std::string& string,
		const ImVec2& size = ImVec2(0, 0),
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void* userData = nullptr
	);
}