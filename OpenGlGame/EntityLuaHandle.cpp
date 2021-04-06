#include "pch.h"
#include "EntityLuaHandle.h"

#include "Entity.h"
#include "Application.h"
#include "LuaRegister.h"

namespace Game
{
	class LuaHelper: public LuaRegister
	{
		bool m_Registered = false;

		static LuaHelper& GetInstance()
		{
			static LuaHelper instance;
			return instance;
		}

		void Register(sol::state &state) override;
	public:
		static bool Registered()
		{
			return GetInstance().m_Registered;
		}

		static void Register()
		{
			auto &instance = GetInstance();

			if(instance.m_Registered)
				return;

			instance.m_Registered = true;

			Application::Get().LuaRegister(instance);
		}
	};

	void LuaHelper::Register(sol::state &state) { }

	EntityLuaHandle::EntityLuaHandle(Entity *entity) : m_Entity(entity),
	                                                   m_State(Application::Get().GetLua())
	{
		LuaHelper::Register();
	}

	void EntityLuaHandle::SetEntity(Entity *entity)
	{
		if(m_Entity != entity)
			m_Entity = entity;
	}
}
