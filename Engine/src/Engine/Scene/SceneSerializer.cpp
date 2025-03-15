#include "pch.h"
#include "Engine/Scene/SceneSerializer.h"

namespace Engine {
	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene) {
		
	}

	void SceneSerializer::Serialize(const std::filesystem::path &path) {
		
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path &path) {
		return false;
	}



}