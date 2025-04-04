#pragma once
#include <filesystem>

#include "Engine/Renderer/Texture.h"

namespace Editor {
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Engine::Ref<Engine::Texture> m_BackArrowIcon;
		Engine::Ref<Engine::Texture> m_DirectoryIcon;
		Engine::Ref<Engine::Texture> m_FileIcon;
	};
}
