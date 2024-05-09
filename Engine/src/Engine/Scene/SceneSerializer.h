#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Scene/Scene.h"

namespace Engine {
	class SceneSerializer {
	public:
		explicit SceneSerializer(const Ref<Scene> &scene);

		void Serialize(const std::filesystem::path &path);
		void Deserialize(const std::filesystem::path &path);

	private:
		Ref<Scene> m_Scene;
	};
}