#pragma once

#include <string>
#include <string_view>
#include <functional>

#include "Types.h"

#include "imgui.h"
#include "fmt/format.h"

namespace Game
{
	using CallbackFunction = std::function<int(ImGuiInputTextCallbackData *)>;

	bool InputText(
		const std::string_view &label,
		std::string &string,
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void *userData            = nullptr
		);

	bool Combo(const std::string_view &label, int32_t &currentItem, const std::string_view &itemList, int32_t maxHeightInItems = -1);

	template <typename Type>
	bool Combo(const std::string_view &label, int32_t &currentItem, const std::vector<Type> itemList, int32_t maxHeightInItems = -1)
	{
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), itemList.size(), maxHeightInItems);
	}

	bool InputTextMultiline(
		const std::string_view &label,
		std::string &string,
		const ImVec2 &size        = ImVec2(0, 0),
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void *userData            = nullptr
		);

	template <class ...Args>
	void Text(const std::string_view &text, Args &&... args)
	{
		return ImGui::TextUnformatted(fmt::format(text, std::forward<Args>(args) ...).c_str());
	}

	template <class ...Args>
	void TextColored(const ImVec4 &color, const std::string_view &text, Args &&... args)
	{
		return ImGui::TextColored(color, fmt::format(text, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	void BulletText(const std::string_view &text, Args &&... args)
	{
		return ImGui::BulletText(fmt::format(text, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	bool TreeNodeEx(const std::string_view &label, ImGuiTreeNodeFlags flags = 0, Args &&...args)
	{
		return ImGui::TreeNodeEx(fmt::format(label, std::forward<Args>(args)...).c_str(), flags);
	}

	bool ToggleButton(const std::string_view &name, bool *v);
}
