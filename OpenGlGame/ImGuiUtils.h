#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <optional>

#include "Types.h"

#include "imgui.h"
#include "fmt/format.h"

namespace Game
{
	using CallbackFunction = std::function<int(ImGuiInputTextCallbackData *)>;

	struct ImGuiWindowPosition
	{
		ImVec2 Position;
		ImGuiCond Cond = 0;
		ImVec2 Pivot = {0, 0};
	};

	struct ImGuiWindowSize
	{
		ImVec2 Size;
		ImGuiCond Cond = 0;
	};
	
	struct ImGuiMainWindowProps
	{
		std::string_view Name;
		bool& Open;
		ImGuiWindowFlags Flags = 0;
	};

	struct ImGuiChildWindowProps
	{
		std::string_view Id;
		ImVec2 Size = {0, 0};
		bool Border = false;
		ImGuiWindowFlags Flags = 0;
	};

	class ImGuiMainWindow
	{
		bool m_Opened = false;

		bool m_SizeSet = false;
		bool m_PosSet = false;

		ImGuiMainWindowProps m_Props;
		ImGuiWindowSize m_Size;
		ImGuiWindowPosition m_Pos;
	public:
		ImGuiMainWindow(const ImGuiMainWindowProps& props);
		ImGuiMainWindow(const ImGuiMainWindowProps& props, const ImGuiWindowPosition& position);
		ImGuiMainWindow(const ImGuiMainWindowProps& props, const ImGuiWindowPosition& position, const ImGuiWindowSize& size);
		ImGuiMainWindow(const ImGuiMainWindowProps& props, const ImGuiWindowSize& size);
		
		void Begin();
		void End();

		operator bool() const { return m_Opened; }
		bool IsOpened() const { return m_Opened; }
		
		void SetPosition(const ImGuiWindowPosition& position);
		void SetSize(const ImGuiWindowSize& size);
	};

	class ImGuiChildWindow
	{
		bool m_Opened = false;
		ImGuiChildWindowProps m_Props;
	
	public:
		ImGuiChildWindow(const ImGuiChildWindowProps& props);
		
		void Begin();
		void End();

		operator bool() const { return m_Opened; }
		bool IsOpened() const { return m_Opened; }
		
	};
	
	template<class ImGuiMenu>
	class ImGuiUniqueGuard
	{
		ImGuiMenu m_ImGuiMenu;
	public:
		template<typename ...Args>
		ImGuiUniqueGuard(Args&& ... args) : m_ImGuiMenu(std::forward<Args>(args)...)
		{
			m_ImGuiMenu.Begin();
		}
		
		~ImGuiUniqueGuard()
		{
			m_ImGuiMenu.End();
		}

		ImGuiUniqueGuard(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard(ImGuiUniqueGuard &&)      = delete;

		ImGuiUniqueGuard& operator =(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard& operator =(ImGuiUniqueGuard &&)      = delete;

		ImGuiMenu Get() const { return m_ImGuiMenu; }

		operator ImGuiMenu() const { return m_ImGuiMenu; }
	};

	template<class ImGuiMenu>
	class ImGuiGuard
	{
		ImGuiMenu m_ImGuiMenu;
		size_t *m_ReferenceCount = nullptr;

	public:
		template<typename ...Args>
		ImGuiGuard(Args&&... args) : m_ImGuiMenu(std::forward<Args>(args)...), m_ReferenceCount(new size_t(1))
		{
			m_ImGuiMenu.Begin();
		}
		~ImGuiGuard()
		{
			--(*m_ReferenceCount);

			if ((*m_ReferenceCount) == 0)
			{
				delete m_ReferenceCount;
				m_ImGuiMenu.End();
			}
		}

		ImGuiGuard(const ImGuiGuard &guard)
		{
			*this = guard;
		}
		ImGuiGuard(ImGuiGuard &&guard) noexcept
		{
			*this = std::move(guard);
		}

		ImGuiGuard& operator=(const ImGuiGuard &guard)
		{
			m_ReferenceCount = guard.m_ReferenceCount;
			m_ImGuiMenu = guard.m_ImGuiMenu;

			++(*m_ReferenceCount);
			
			return *this;
		}
		ImGuiGuard& operator=(ImGuiGuard &&guard) noexcept
		{
			m_ReferenceCount = guard.m_ReferenceCount;
			m_ImGuiMenu = guard.m_ImGuiMenu;

			guard.m_ReferenceCount = nullptr;
			
			return *this;
		}

		ImGuiMenu Get() const { return m_ImGuiMenu; }
		operator ImGuiMenu() const { return m_ImGuiMenu; }

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

	template <class ...Args>
	void BulletText(const std::string_view& text, Args&&... args)
	{
		return ImGui::BulletText(fmt::format(text, std::forward<Args>(args)...).c_str());
	}

	template <class ...Args>
	bool TreeNodeEx(const std::string_view& label, ImGuiTreeNodeFlags flags = 0, Args&&...args)
	{
		return ImGui::TreeNodeEx(fmt::format(label, std::forward<Args>(args)...).c_str(), flags);
	}
}
