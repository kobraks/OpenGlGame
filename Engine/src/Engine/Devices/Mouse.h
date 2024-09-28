#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/MouseCodes.h"
#include "Engine/Core/Vector2.h"

namespace sol {
	class state;
}

namespace Engine {
	class Window;

	class Mouse {
	public:
		// static bool IsButtonPressed(MouseCode button);
		static bool IsButtonPressed(MouseCode button, const Window &relative);
		static bool IsButtonPressed(MouseCode button);

		// static Vector2i GetPosition();
		static Vector2i GetPosition(const Window &relative);
		static Vector2i GetPosition();

		// static void SetPosition(const Vector2i &pos);
		static void SetPosition(const Vector2i &pos, const Window &relative);
		static void SetPosition(const Vector2i &pos);

		// static void SetPosition(int32_t x, int32_t y);
		static void SetPosition(int32_t x, int32_t y, const Window &relative);
		static void SetPosition(int32_t x, int32_t y);

		static void RegisterLua(sol::state &lua);
	};
}