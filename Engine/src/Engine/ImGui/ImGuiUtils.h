#pragma once

#include "Engine/Core/Base.h"

#include <string>
#include <string_view>
#include <functional>

#include <imgui.h>
#include <type_traits>
#include <fmt/format.h>

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
	void Text(fmt::format_string<Args...> format, Args &&... args)
	{
		return ImGui::TextUnformatted(fmt::format(format, std::forward<Args>(args) ...).c_str());
	}

	template <class ...Args>
	void TextColored(const ImVec4 &color, fmt::format_string<Args...> format, Args &&... args)
	{
		return ImGui::TextColored(color, fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	void BulletText(fmt::format_string<Args...> format, Args &&... args)
	{
		return ImGui::BulletText(fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	bool TreeNodeEx(fmt::format_string<Args...> label, ImGuiTreeNodeFlags flags = 0, Args &&...args)
	{
		return ImGui::TreeNodeEx(fmt::format(label, std::forward<Args>(args)...).c_str(), flags);
	}

	bool ToggleButton(const std::string_view &name, bool *v);
}
