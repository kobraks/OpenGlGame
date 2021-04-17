#pragma once

#include "Entity.h"

namespace Game
{
	class ScriptableEntity
	{
		Entity m_Entity;
		
	public:
		virtual ~ScriptableEntity() {}

		template<typename Component>
		Component& GetComponent()
		{
			return m_Entity.GetComponent<Component>();
		}

	protected:
		virtual void OnCreate(){}
		virtual void OnDestroy(){}
		virtual void OnConstUpdate(Time timeStep) {}
		virtual void OnUpdate() {}

		friend class Scene;
	};
}
