#pragma once
#include "Engine/Core/Base.h"

#include <functional>
#include <string>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>
#include <ImGui/imgui.h>

namespace Engine {
	using CallbackFunction = std::function<int(ImGuiInputTextCallbackData *)>;

	bool InputText(
		std::string_view label,
		std::string &string,
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void *userData            = nullptr
		);

	bool Combo(std::string_view label, int32_t &currentItem, std::string_view itemList, int32_t maxHeightInItems = -1);

	template <typename T>
	bool Combo(std::string_view label, int32_t &currentItem, std::vector<T> &itemList, int32_t maxHeightInItems = -1) {
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), itemList.size(), maxHeightInItems);
	}

	bool InputTextMultiline(
		std::string_view label,
		std::string &string,
		const ImVec2 &size        = ImVec2(0, 0),
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void *userData            = nullptr
		);

	template <class... Args>
	void Text(fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::TextUnformatted(fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class... Args>
	void TextColored(const ImVec4 &color, fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::TextColored(color, fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class... Args>
	void BulledText(fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::BulletText(fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class... Args>
	bool TreeNodeEx(ImGuiTreeNodeFlags flags, fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::TreeNodeEx(fmt::format(format, std::forward<Args>(args)...).c_str(), flags);
	}

	bool ToggleButton(std::string_view name, bool *v);
}
