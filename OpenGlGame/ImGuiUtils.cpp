#include "pch.h"
#include "ImGuiUtils.h"

#include "Assert.h"

namespace Game
{
	struct InputTextCallbackData
	{
		std::string *String;
		CallbackFunction Callback;
		void *UserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData *data)
	{
		auto userData = static_cast<InputTextCallbackData*>(data->UserData);
		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			auto string = userData->String;
			ASSERT((data->Buf == string->c_str()), "");
			string->resize(data->BufTextLen);
			data->Buf = const_cast<char*>(string->c_str());
		}
		else if(userData->Callback)
		{
			data->UserData = userData->UserData;
			return userData->Callback(data);
		}

		return 0;
	}

	bool InputText(const std::string_view &label, std::string &string, ImGuiInputTextFlags flags, CallbackFunction callback, void *userData)
	{
		ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0, "");
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallbackData data{&string, callback, userData};
		return ImGui::InputText(label.data(), string.data(), string.capacity(), flags, InputTextCallback, &data);
	}

	bool Combo(const std::string_view &label, int32_t &currentItem, const std::string_view &itemList, int32_t maxHeightInItems)
	{
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), maxHeightInItems);
	}

	bool InputTextMultiline(
		const std::string_view &label,
		std::string &string,
		const ImVec2 &size,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void *userData
		)
	{
		ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0, "");
		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallbackData data{&string, callback, userData};
		
		return ImGui::InputTextMultiline(label.data(), string.data(), string.size(), size, flags, InputTextCallback, &data);
	}
}
