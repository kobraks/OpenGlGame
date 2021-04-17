#pragma once

namespace Game
{
	class Entity;

	class EntityLuaHandle
	{
		Entity *m_Entity;
		sol::state& m_State;
	public:
		EntityLuaHandle(Entity *entity = nullptr);

		Entity* GetEntity() const { return m_Entity; }
	private:
		friend Entity;
	};
}
