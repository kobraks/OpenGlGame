#pragma once
#include "pch.h"
#include "pch.h"

#include <utility>
#include <string>

#include "Types.h"


namespace Game
{
	class Entity;
	class Scene;
	
	class SceneSerializer
	{
		sol::state *m_AppState;
		
		Pointer<Scene> m_Scene;
	public:
		SceneSerializer(const Pointer<Scene> scene);

		void Serialize(const std::string &filePath);

		bool Deserialize(const std::string &filePath);

	private:
		void ReadScene(sol::state& state);
		void ProcessScene(sol::table scene);
		void ProcessEntityTable(sol::table entities);
		static int64_t FindEntityId(sol::table entityTable);
		void ProcessEntity(uint64_t index, Entity entity, sol::table entityTable);
		void ProcessComponent(unsigned long long index, Entity entity, std::string name, sol::table component);
	};
}