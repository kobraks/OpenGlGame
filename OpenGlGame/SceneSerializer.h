#pragma once
#include "Types.h"


namespace Game
{
	class Entity;
	class Scene;

	class SceneSerializer
	{
		Ref<Scene> m_Scene;
	public:
		explicit SceneSerializer(const Ref<Scene> &scene);

		void Serialize(const std::string &filePath);
		bool Deserialize(const std::string &filePath);

	private:
	};
}
