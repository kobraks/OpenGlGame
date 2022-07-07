#pragma once

#include <string>
#include <string_view>
#include "Types.h"

#include "imgui.h"

namespace Game
{
	struct ImGuiWindowPosition
	{
		ImVec2 Position;
		ImGuiCond Cond = 0;
		ImVec2 Pivot   = {0, 0};
	};

	struct ImGuiWindowSize
	{
		ImVec2 Size;
		ImGuiCond Cond = 0;
	};

	struct ImGuiMainWindowProps
	{
		std::string_view Name;
		bool &Open;
		ImGuiWindowFlags Flags = 0;
	};

	struct ImGuiChildWindowProps
	{
		std::string_view Id;
		ImVec2 Size            = {0, 0};
		bool Border            = false;
		ImGuiWindowFlags Flags = 0;
	};

	struct ImGuiTableProps
	{
		std::string_view Id;
		int ColumnsCount;
		ImGuiTableFlags Flags   = 0;
		const ImVec2 &OuterSize = ImVec2(-FLT_MIN, 0);
		float InnerWidth        = 0.0f;
	};

	class ImGuiMainWindow
	{
		bool m_Opened = false;

		bool m_SizeSet = false;
		bool m_PosSet  = false;

		ImGuiMainWindowProps m_Props;
		ImGuiWindowSize m_Size;
		ImGuiWindowPosition m_Pos;
	public:
		ImGuiMainWindow(const ImGuiMainWindowProps &props);
		ImGuiMainWindow(const ImGuiMainWindowProps &props, const ImGuiWindowPosition &position);
		ImGuiMainWindow(
			const ImGuiMainWindowProps &props,
			const ImGuiWindowPosition &position,
			const ImGuiWindowSize &size
			);
		ImGuiMainWindow(const ImGuiMainWindowProps &props, const ImGuiWindowSize &size);

		void Begin();
		void End();

		operator bool() const { return m_Opened; }
		bool IsOpened() const { return m_Opened; }

		void SetPosition(const ImGuiWindowPosition &position);
		void SetSize(const ImGuiWindowSize &size);
	};

	class ImGuiChildWindow
	{
		bool m_Opened = false;
		ImGuiChildWindowProps m_Props;

	public:
		ImGuiChildWindow(const ImGuiChildWindowProps &props);

		void Begin();
		void End();

		operator bool() const { return m_Opened; }
		bool IsOpened() const { return m_Opened; }
	};

	class ImGuiTable
	{
		ImGuiTableProps m_Props;
	public:
		ImGuiTable(const ImGuiTableProps &props);

		void Begin();
		void End();
	};

	class ImGuiGroup
	{
	public:
		ImGuiGroup() = default;
		void Begin();
		void End();
	};

	class ImGuiID
	{
		std::variant<std::string, int, const void*> Id;

	public:
		explicit ImGuiID(const std::string &id) : Id(id){}
		template <typename ...Args>
		explicit ImGuiID(std::string_view format, Args &&... args) : Id(
		                                                                fmt::format(format, std::forward<Args>(args)...)
		                                                               ) {}

		explicit ImGuiID(int i) : Id(i) {}
		explicit ImGuiID(const void *p) : Id(p) {}

		void Begin();
		void End();
	};

	template <class ImGuiMenu>
	class ImGuiUniqueGuard
	{
		ImGuiMenu m_ImGuiMenu;
		bool m_IsLocked = true;
	public:
		template <typename ...Args>
		ImGuiUniqueGuard(Args && ... args) : m_ImGuiMenu(std::forward<Args>(args)...)
		{
			m_ImGuiMenu.Begin();
		}

		~ImGuiUniqueGuard()
		{
			Unlock();
		}

		ImGuiUniqueGuard(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard(ImGuiUniqueGuard &&)      = delete;

		ImGuiUniqueGuard& operator =(const ImGuiUniqueGuard &) = delete;
		ImGuiUniqueGuard& operator =(ImGuiUniqueGuard &&)      = delete;

		ImGuiMenu Get() const { return m_ImGuiMenu; }

		operator ImGuiMenu() const { return m_ImGuiMenu; }

		bool IsLocked() const { return m_IsLocked; }

		void Lock()
		{
			if(m_IsLocked)
				return;

			m_IsLocked = true;
			m_ImGuiMenu.Begin();
		}

		void Unlock()
		{
			m_IsLocked = false;
			m_ImGuiMenu.End();
		}
	};

	template <class ImGuiMenu>
	class ImGuiGuard
	{
		ImGuiMenu m_ImGuiMenu;
		size_t *m_ReferenceCount = nullptr;
		bool *m_IsLocked         = nullptr;

	public:
		template <typename ...Args>
		ImGuiGuard(Args &&... args) : m_ImGuiMenu(std::forward<Args>(args)...),
		                              m_ReferenceCount(new size_t(1)),
		                              m_IsLocked(new bool(true))
		{
			m_ImGuiMenu.Begin();
		}

		~ImGuiGuard()
		{
			--(*m_ReferenceCount);

			if((*m_ReferenceCount) == 0)
			{
				delete m_ReferenceCount;
				delete m_IsLocked;
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
			m_ImGuiMenu      = guard.m_ImGuiMenu;
			m_IsLocked       = guard.m_IsLocked;

			++(*m_ReferenceCount);

			return *this;
		}

		ImGuiGuard& operator=(ImGuiGuard &&guard) noexcept
		{
			m_ReferenceCount = guard.m_ReferenceCount;
			m_ImGuiMenu      = guard.m_ImGuiMenu;
			m_IsLocked       = guard.m_IsLocked;

			guard.m_ReferenceCount = nullptr;
			guard.m_IsLocked       = nullptr;

			return *this;
		}

		ImGuiMenu Get() const { return m_ImGuiMenu; }
		operator ImGuiMenu() const { return m_ImGuiMenu; }

		size_t GetReferenceCount() const { return *m_ReferenceCount; }

		bool IsLocked() const { return *m_IsLocked; }

		void Lock()
		{
			if(*m_IsLocked)
				return;

			(*m_IsLocked) = true;
			m_ImGuiMenu.Begin();
		}

		void Unlock()
		{
			(*m_IsLocked) = false;
			m_ImGuiMenu.End();
		}
	};
}
