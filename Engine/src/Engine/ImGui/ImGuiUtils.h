#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Color.h"

#include "Engine/Utils/StdUtils.h"

#include <functional>
#include <type_traits>

#include <fmt/format.h>

#include <ImGui/imgui.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Engine {
	using CallbackFunction = std::function<int(ImGuiInputTextCallbackData *)>;

	template<typename Container>
	auto ComboFromContainer(std::string_view label, int32_t& currentItem, const Container& items, auto&& getter, int32_t maxHeightInItems = -1) {
		return ImGui::Combo(Utils::EnsureNullTerminated(label), &currentItem, getter, const_cast<void*>(static_cast<const void*>(&items)), static_cast<int>(items.size()), maxHeightInItems);
	}

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
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>, "Combo only supports std::string or std::string_view as vector element types");

		return false;
	}

	template<>
	inline bool Combo(std::string_view label, int32_t& currentItem, std::vector<std::string>& itemList, int32_t maxHeightInItems) {
		auto getter = [](void* data, int idx)-> const char* {
			const auto items = static_cast<std::vector<std::string>*>(data);

			return items->at(idx).c_str();
		};

		return ComboFromContainer(label, currentItem, itemList, getter, maxHeightInItems);
	}

	template<>
	inline bool Combo(std::string_view label, int32_t& currentItem, std::vector<std::string_view>& itemList, int32_t maxHeightInItems) {
		auto getter = [](void* data, int idx)-> const char* {
			const auto items = static_cast<std::vector<std::string_view>*>(data);

			thread_local std::string item;
			return Utils::EnsureNullTerminated(items->at(idx), item);
			};

		return ComboFromContainer(label, currentItem, itemList, getter, maxHeightInItems);
	}

	bool DragVec2(std::string_view label, glm::vec2& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool DragVec3(std::string_view label, glm::vec3& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool DragVec4(std::string_view label, glm::vec4& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

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
	void BulletText(fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::BulletText(fmt::format(format, std::forward<Args>(args)...).c_str());
	}

	template <class... Args>
	bool TreeNodeEx(ImGuiTreeNodeFlags flags, fmt::format_string<Args...> format, Args &&... args) {
		return ImGui::TreeNodeEx(fmt::format(format, std::forward<Args>(args)...).c_str(), flags);
	}

	struct ToggleButtonStyle {
		ImU32 OnColor = ImGui::GetColorU32(ImGuiCol_ButtonActive);
		ImU32 OffColor = ImGui::GetColorU32(ImGuiCol_Button);

		ImU32 ThumbColor = ImGui::GetColorU32(ImGuiCol_Text);
		ImU32 BorderColor = ImGui::GetColorU32(ImGuiCol_Border);
		ImVec2 Size = { 40.0f, 20.0f};

		float HoverBrightness = 0.1f;
		float IdleDarkening = 0.05f;
	};

	bool ToggleButton(std::string_view label, bool* value, std::string_view tooltip = "", const ToggleButtonStyle* style = nullptr);

	void HelpMarker(std::string_view description);
}
