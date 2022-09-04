#pragma once

#include "Engine/Core/Base.h"

namespace Game
{
	class Entity;
	class Scene;

	class SceneSerializer
	{
		Ref<Scene> m_Scene;

	public:
		explicit SceneSerializer(const Ref<Scene> &scene);

		void Serialize(const std::filesystem::path& path);
		void Deserialize(const std::filesystem::path& path);
	};
}