#pragma once
#include <filesystem>

#include "Engine/Renderer/Texture.h"

namespace Editor {
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		void SetPadding(float padding) { m_Padding = padding; }
		void SetThumbnailSize(float size) { m_ThumbnailSize = size; }

		float GetPadding() const { return m_Padding; }
		float GetThumbnailSize() const { return m_ThumbnailSize; }

	protected:
		void DrawAssetEntry(const std::filesystem::directory_entry& directoryEntry);
		Engine::Ref<Engine::Texture> GetIconForEntry(const std::filesystem::directory_entry& directoryEntry) const;


	private:
		float m_Padding = 16.f;
		float m_ThumbnailSize = 128.f;

		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Engine::Ref<Engine::Texture> m_BackArrowIcon;
		Engine::Ref<Engine::Texture> m_DirectoryIcon;
		Engine::Ref<Engine::Texture> m_FileIcon;
	};
}
