#include "pch.h"
#include "ImGuiInput.h"

#include "Engine/Utils/StdUtils.h"

#include <imgui_internal.h>

namespace Engine {
	constexpr size_t DefaultInputTextReserve = 32;
	constexpr size_t DefaultInputMultilineReserve = 64;

	struct InputTextCallbackData {
		std::string* String;
		CallbackFunction Callback;
		void* UserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData* data) {
		auto userData = static_cast<InputTextCallbackData*>(data->UserData);

		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			auto string = userData->String;
			ENGINE_ASSERT((data->Buf == string->c_str()));
			string->resize(data->BufTextLen);
			data->Buf = const_cast<char*>(string->c_str());
		}
		else if (userData->Callback) {
			data->UserData = userData->UserData;
			return userData->Callback(data);
		}

		return 0;
	}

	bool InputText(
		std::string_view label,
		std::string& string,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void* userData
	) {
		ENGINE_ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0);

		if (string.capacity() == 0)
			string.reserve(DefaultInputTextReserve);

		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallbackData data{ &string, callback, userData };
		return ImGui::InputTextEx(Utils::EnsureNullTerminated(label), nullptr, string.data(), static_cast<int>(string.capacity()), ImVec2(0, 0), flags, InputTextCallback, &data);
	}


	bool InputTextMultiline(
		std::string_view label,
		std::string& string,
		const ImVec2& size,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void* userData
	) {
		ENGINE_ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0);


		if (string.capacity() == 0)
			string.reserve(DefaultInputMultilineReserve);

		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallbackData data{ &string, callback, userData };

		return ImGui::InputTextEx(Utils::EnsureNullTerminated(label), nullptr, string.data(), static_cast<int>(string.capacity()), size, flags | ImGuiInputTextFlags_Multiline, InputTextCallback, &data);
	}
}