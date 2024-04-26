#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/KeyCode.h"

namespace sol {
	class state;
}

namespace Engine {
	class Window;

	class Keyboard {
	public:
		// static bool IsKeyPressed(KeyCode key);
		static bool IsKeyPressed(KeyCode key, const Window &window);

		static void RegisterLua(sol::state &lua);
	};
}