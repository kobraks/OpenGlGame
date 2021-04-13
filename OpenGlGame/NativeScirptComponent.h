#pragma once

#include "Component.h"
#include "ScriptableEntity.h"

namespace Game
{
	class NativeScriptComponent
	{
	public:
		ScriptableEntity *Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent *);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript     = [](NativeScriptComponent *nsc)
			{
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}
	};
}
