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
		static Vector2f GetPosition(const Window &relative);
		static Vector2f GetPosition();

		// static void SetPosition(const Vector2i &pos);
		static void SetPosition(const Vector2f &pos, const Window &relative);
		static void SetPosition(const Vector2f &pos);

		// static void SetPosition(int32_t x, int32_t y);
		static void SetPosition(float x, float y, const Window &relative);
		static void SetPosition(float x, float y);

		static float GetX();
		static float GetY();

		static void UpdateDelta();
		static Vector2f GetDelta() { return s_Delta; }

		static void LockCursor(bool enabled);
		static void ShowCursor(bool show);

		static bool IsCursorLocked();
		static bool IsCursorVisible();

		static void ToggleCursorLock();

		static void RegisterLua(sol::state &lua);
	private:
		static Vector2f s_LastPosition;
		static Vector2f s_Delta;

	};
}