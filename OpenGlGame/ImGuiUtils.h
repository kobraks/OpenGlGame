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

	class ImGuiUniqueGuard
	{
		bool m_Opened = false;

	public:
		ImGuiUniqueGuard(const ImVec2 &position, const std::string_view &name, bool &open, ImGuiWindowFlags flags = 0);
		ImGuiUniqueGuard(const std::string_view &name, bool &open, ImGuiWindowFlags flags = 0);
		~ImGuiUniqueGuard();

		ImGuiUniqueGuard(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard(ImGuiUniqueGuard &&)      = delete;

		ImGuiUniqueGuard& operator =(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard& operator =(ImGuiUniqueGuard &&)      = delete;

		bool Get() const { return m_Opened; }

		operator bool() const { return m_Opened; }
	};

	class ImGuiGuard
	{
		bool m_Opened       = false;
		size_t *m_ReferenceCount = nullptr;

	public:
		ImGuiGuard(const ImVec2 &position, const std::string_view &name, bool &open, ImGuiWindowFlags flags = 0);
		ImGuiGuard(const std::string_view &name, bool &open, ImGuiWindowFlags flags = 0);
		~ImGuiGuard();

		ImGuiGuard(const ImGuiGuard &guard);
		ImGuiGuard(ImGuiGuard &&guard);

		ImGuiGuard& operator=(const ImGuiGuard &guard);
		ImGuiGuard& operator=(ImGuiGuard &&guard);

		bool Get() const { return m_Opened; }
		operator bool() const { return m_Opened; }

		size_t GetReferenceCount() const { return *m_ReferenceCount; }
	};

	class ImGuiChildUniqueGuard
	{
		bool m_Opened;

	public:
		ImGuiChildUniqueGuard(
			ImGuiID id,
			const ImVec2 &size     = ImVec2(0, 0),
			bool border            = false,
			ImGuiWindowFlags flags = 0
			);
		ImGuiChildUniqueGuard(
			const std::string_view &id,
			const ImVec2 &size     = ImVec2(0, 0),
			bool border            = false,
			ImGuiWindowFlags flags = 0
			);

		~ImGuiChildUniqueGuard();

		ImGuiChildUniqueGuard(const ImGuiChildUniqueGuard &) = delete;
		ImGuiChildUniqueGuard(ImGuiChildUniqueGuard &&)      = delete;


		ImGuiChildUniqueGuard& operator =(const ImGuiChildUniqueGuard &) = delete;
		ImGuiChildUniqueGuard& operator =(ImGuiChildUniqueGuard &&)      = delete;

		bool Get() const { return m_Opened; }
		operator bool() const { return m_Opened; }
	};

	class ImGuiChildGuard
	{
		bool m_Opened;
		size_t *m_ReferenceCount;

	public:
		ImGuiChildGuard(ImGuiID id, const ImVec2 &size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
		ImGuiChildGuard(
			const std::string_view &id,
			const ImVec2 &size     = ImVec2(0, 0),
			bool border            = false,
			ImGuiWindowFlags flags = 0
			);

		~ImGuiChildGuard();

		ImGuiChildGuard(const ImGuiChildGuard &guard);
		ImGuiChildGuard(ImGuiChildGuard &&guard);


		ImGuiChildGuard& operator =(const ImGuiChildGuard &guard);
		ImGuiChildGuard& operator =(ImGuiChildGuard &&guard);

		bool Get() const { return m_Opened; }
		operator bool() const { return m_Opened; }
		
		size_t GetReferenceCount() const { return *m_ReferenceCount; }
	};

	bool InputText(
		const std::string_view &label,
		std::string &string,
		ImGuiInputTextFlags flags = 0,
		CallbackFunction callback = nullptr,
		void *userData            = nullptr
		);

	bool Combo(
		const std::string_view &label,
		int32_t &currentItem,
		const std::string_view &itemList,
		int32_t maxHeightInItems = -1
		);

	template <typename Type>
	bool Combo(
		const std::string_view &label,
		int32_t &currentItem,
		const std::vector<Type> itemList,
		int32_t maxHeightInItems = -1
		)
	{
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), itemList.size(), maxHeightInItems);
	}

	template <class ...Args>
	void Text(const std::string_view &text, Args &&... args)
	{
		return ImGui::Text(fmt::format(text, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	void TextColored(const ImVec4 &color, const std::string_view &text, Args &&... args)
	{
		return ImGui::TextColored(color, fmt::format(text, std::forward<Args>(args)...));
	}
}
