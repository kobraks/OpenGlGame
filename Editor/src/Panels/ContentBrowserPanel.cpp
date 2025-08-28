#include "pch.h"
#include "ContentBrowserPanel.h"

#include "Engine/Project/Project.h"
#include "Engine/Core/Image.h"
#include "Engine/ImGui/ImGuiScoped.h"

#include <ImGui/imgui.h>

#include <algorithm>

namespace Editor {
	static bool DrawButton(std::string name, Engine::Ref<Engine::Texture> icon, float size) {
		const ImVec2 buttonSize = { size, size };
		constexpr ImVec2 uv0 = { 0, 0 };
		constexpr ImVec2 uv1 = { 1, 1 };

		return ImGui::ImageButton(name.c_str(), static_cast<ImTextureID>(icon->RendererID()), buttonSize, uv0, uv1);
	}

	ContentBrowserPanel::ContentBrowserPanel() : m_BaseDirectory(Engine::Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory){
		m_BackArrowIcon = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/BackArrow.png"));
		m_DirectoryIcon = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/DirectoryIcon.png"));
		m_FileIcon = Engine::Texture::Create(Engine::Image::Load("Resources/Icons/FileIcon.png"));
	}

	void ContentBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Content Browser");
		const float cellSize = m_ThumbnailSize + m_Padding;

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory)) {
			if (DrawButton("Back", m_BackArrowIcon, 20.f))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		const float panelWidth = ImGui::GetContentRegionAvail().x;
		const int columnCount = std::max(static_cast<int>(panelWidth / cellSize), 1);

		ImGui::Columns(columnCount, 0, false);

		for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
			DrawAssetEntry(directoryEntry);
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumbnail Size", &m_ThumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &m_Padding, 0, 32);

		ImGui::End();
	}

	void ContentBrowserPanel::DrawAssetEntry(const std::filesystem::directory_entry& directoryEntry) {
		const auto& path = directoryEntry.path();
		const std::string fileNameString = path.filename().string();

		Engine::ScopedID id(fileNameString);
		{
			Engine::ScopedStyleColor styleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
			DrawButton("Button", GetIconForEntry(directoryEntry), m_ThumbnailSize);
		}

		if (ImGui::IsItemActive() && ImGui::BeginDragDropSource()) {
			const auto relativePath = std::filesystem::relative(path, m_BaseDirectory);
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (directoryEntry.is_directory())
				m_CurrentDirectory /= path.filename();
		}
		ImGui::TextWrapped("%s", fileNameString.c_str());
	}

	Engine::Ref<Engine::Texture> ContentBrowserPanel::GetIconForEntry(
		const std::filesystem::directory_entry& directoryEntry) const {

		if (directoryEntry.is_directory())
			return m_DirectoryIcon;

		return m_FileIcon;
	}
}
