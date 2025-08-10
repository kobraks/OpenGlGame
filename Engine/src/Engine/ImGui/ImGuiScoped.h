#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"

#include "Engine/Utils/StdUtils.h"
#include "Engine/Utils/CoreTraits.h"

#include <ImGui/imgui.h>

#include <initializer_list>

namespace Engine {
	struct ScopedGroup : NonCopyableNonMoveable {
		ScopedGroup() {
			ImGui::BeginGroup();
		}

		~ScopedGroup() {
			ImGui::EndGroup();
		}
	};

	struct ScopedID : NonCopyableNonMoveable {
		explicit ScopedID(int id) {
			ImGui::PushID(id);
		}

		explicit ScopedID(const char* strID) {
			ImGui::PushID(strID);
		}

		explicit ScopedID(std::string_view strID) {
			ImGui::PushID(Utils::EnsureNullTerminated(strID));
		}

		explicit ScopedID(const void* ptrID) {
			ImGui::PushID(ptrID);
		}

		~ScopedID() {
			ImGui::PopID();
		}
	};

	struct ScopedStyleColor : NonCopyableNonMoveable {
		template <typename ...Args>
		ScopedStyleColor(Args&&... args) {
			static_assert(sizeof...(Args) % 2 == 0, "Expected (idx, value) pairs: even number of arguments");
			PushPairs(std::forward<Args>(args)...);
		}

		explicit ScopedStyleColor(std::initializer_list<std::pair<ImGuiCol, ImVec4>> colors) {
			for (const auto& [col, val] : colors) {
				ImGui::PushStyleColor(col, val);
				++m_Count;
			}
		}

		explicit ScopedStyleColor(std::initializer_list<std::pair<ImGuiCol, ImU32>> colors) {
			for (const auto& [col, val] : colors) {
				ImGui::PushStyleColor(col, val);
				++m_Count;
			}
		}

		explicit ScopedStyleColor(std::initializer_list<std::pair<ImGuiCol, Color>> colors) {
			for (const auto& [col, val] : colors) {
				const auto color = val.ToFloat();

				ImGui::PushStyleColor(col, ImVec4(color.x, color.y, color.z, color.w));
				++m_Count;
			}
		}

		ScopedStyleColor(ImGuiCol idx, const ImVec4& color) {
			ImGui::PushStyleColor(idx, color);
			m_Count = 1;
		}

		ScopedStyleColor(ImGuiCol idx, ImU32 color) {
			ImGui::PushStyleColor(idx, color);
			m_Count = 1;
		}

		ScopedStyleColor(ImGuiCol idx, const Color& color) {
			const auto fColor = color.ToFloat();

			ImGui::PushStyleColor(idx, ImVec4(fColor.x, fColor.y, fColor.z, fColor.w));
			m_Count = 1;
		}

		~ScopedStyleColor() {
			ImGui::PopStyleColor(m_Count);
		}

	private:
		template <typename ValueType, typename... Args>
		void PushPairs(ImGuiStyleVar idx, ValueType&& value, Args&&... args) {
			if constexpr (std::is_same_v<ValueType, Color>) {
				const auto fColor = value.ToFloat();
				ImGui::PushStyleColor(idx, ImVec4(fColor.x, fColor.y, fColor.z, fColor.w));
			} else {
				ImGui::PushStyleColor(idx, std::forward<ValueType>(value));
			}

			++m_Count;

			if constexpr (sizeof...(Args) > 0) {
				PushPairs(std::forward<Args&&>(args)...);
			}
		}

		int m_Count = 0;
	};

	struct ScopedStyleVar : NonCopyableNonMoveable {
		template <typename ...Args>
		ScopedStyleVar(Args&&... args){
			static_assert(sizeof...(Args) % 2 == 0, "Expected (idx, value) pairs: even number of arguments");
			PushPairs(std::forward<Args>(args)...);
		}

		explicit ScopedStyleVar(std::initializer_list<std::pair<ImGuiStyleVar, float>> vars) {
			for (const auto& [var, val] : vars) {
				ImGui::PushStyleVar(var, val);
				++m_Count;
			}
		}

		explicit ScopedStyleVar(std::initializer_list<std::pair<ImGuiStyleVar, ImVec2>> vars) {
			for (const auto& [var, val] : vars) {
				ImGui::PushStyleVar(var, val);
				++m_Count;
			}
		}

		ScopedStyleVar(ImGuiStyleVar idx, float value) {
			ImGui::PushStyleVar(idx, value);
			m_Count = 1;
		}

		ScopedStyleVar(ImGuiStyleVar idx, const ImVec2& value) {
			ImGui::PushStyleVar(idx, value);
			m_Count = 1;
		}

		~ScopedStyleVar() {
			ImGui::PopStyleVar(m_Count);
		}
	private:
		template <typename ValueType, typename... Args>
		void PushPairs(ImGuiStyleVar idx, ValueType&& value, Args&&... args) {
			ImGui::PushStyleVar(idx, std::forward<ValueType>(value));
			++m_Count;

			if constexpr (sizeof...(Args) > 0) {
				PushPairs(std::forward<Args&&>(args)...);
			}
		}

		int m_Count = 0;
	};

	struct ScopedDisable : NonCopyableNonMoveable {
		explicit ScopedDisable(bool condition = true) : m_Active(condition) {
			if (condition) {
				ImGui::BeginDisabled();
			}
		}

		~ScopedDisable() {
			if (m_Active) {
				ImGui::EndDisabled();
			}
		}

	private:
		bool m_Active = false;
	};
}