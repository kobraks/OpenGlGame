#include "DockspaceManager.h"

#include "ImGui/imgui_internal.h"

namespace Editor {
	void DockspaceManager::BeginDockspace() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		if (m_OptFullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (m_Flags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::Begin("##DockSpace", &m_Open, windowFlags);
		ImGui::PopStyleVar();

		if (m_OptFullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			const ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			SetupDefaultLayout(dockspaceID);
			ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), m_Flags);
		}
	}

	void DockspaceManager::EndDockspace() {
		ImGui::End();
	}

	void DockspaceManager::SetupDefaultLayout(ImGuiID dockspaceID) {
		if (ImGui::DockBuilderGetNode(dockspaceID))
			return;

		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->Size);

		ImGuiID dockMainID = dockspaceID;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.2f, nullptr, &dockMainID);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.25f, nullptr, &dockMainID);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);
		ImGuiID dockCenter = dockMainID;

		ImGui::DockBuilderDockWindow("Scene Hierarchy", dockLeft);
		ImGui::DockBuilderDockWindow("Properties", dockRight);
		ImGui::DockBuilderDockWindow("Content Browser", dockBottom);
		ImGui::DockBuilderDockWindow("Viewport", dockCenter);

		ImGui::DockBuilderFinish(dockspaceID);
	}
}
