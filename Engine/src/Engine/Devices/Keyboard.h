#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/KeyCodes.h"

#include <sol/state.hpp>

namespace Game
{
	class Window;
	
	class Keyboard
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyPressed(KeyCode key, const Window& window);

		static void RegisterLua(sol::state& lua);
	private:
	};
}
