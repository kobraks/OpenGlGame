#pragma once

#include <ImGui/imgui.h>

namespace Editor {
	class DockspaceManager {
	public:
		void BeginDockspace();
		void EndDockspace();

		void SetFullscreen(bool enable) { m_OptFullscreen = enable; }
		void SetDockspaceFlags(ImGuiDockNodeFlags flags) { m_Flags = flags; }

	private:
		void SetupDefaultLayout(ImGuiID dockspaceID);

	private:
		bool m_Open = true;
		bool m_OptFullscreen = true;

		ImGuiDockNodeFlags m_Flags = ImGuiDockNodeFlags_None;
	};
}