#pragma once
#include <filesystem>

#include "Engine/Renderer/Texture2D.h"

namespace Editor {
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Engine::Ref<Engine::Texture2D> m_BackArrowIcon;
		Engine::Ref<Engine::Texture2D> m_DirectoryIcon;
		Engine::Ref<Engine::Texture2D> m_FileIcon;
	};
}
