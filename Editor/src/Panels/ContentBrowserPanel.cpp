#include "pch.h"
#include "ContentBrowserPanel.h"

#include "Engine/Project/Project.h"
#include "Engine/Core/Image.h"

#include <ImGui/imgui.h>

namespace Editor {
	ContentBrowserPanel::ContentBrowserPanel() : m_BaseDirectory(Engine::Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory){
		m_BackArrowIcon = Engine::Texture2D::Create(Engine::Image::Load("Resources/Icons/BackArrow.png"));
		m_DirectoryIcon = Engine::Texture2D::Create(Engine::Image::Load("Resources/Icons/DirectoryIcon.png"));
		m_FileIcon = Engine::Texture2D::Create(Engine::Image::Load("Resources/Icons/FileIcon.png"));
	}

	void ContentBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Content Browser");
		static float padding = 16.f;
		static float thumbnailSize = 128.f;
		float cellSize = thumbnailSize + padding;

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory)) {
			if (ImGui::ImageButton("<-", static_cast<ImTextureID>(m_BackArrowIcon->ID()), {20.f, 20.f}, { 0, 1 }, { 1, 0 }))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = static_cast<int>(panelWidth / cellSize);

		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
			const auto& path = directoryEntry.path();
			const std::string fileNameString = path.filename().string();

			ImGui::PushID(fileNameString.c_str());
			//TODO Directory
			const Engine::Ref<Engine::Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
			ImGui::ImageButton("Button", static_cast<ImTextureID>(icon->ID()), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource()) {
				const std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(fileNameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}


}