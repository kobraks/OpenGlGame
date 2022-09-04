#pragma once
#include "Engine/Core/Vector2.h"

#include <sol/state.hpp>

namespace Game
{
	class Window;

	class Mouse
	{
	public:
		using CodeType = uint16_t;

		enum Button : CodeType
		{
			// From glfw3.h
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			ButtonLeft = Button0,
			ButtonRight = Button1,
			ButtonMiddle = Button2,

			ButtonLast = Button7
		};;

		static bool IsButtonPressed(CodeType button);
		static bool IsButtonPressed(CodeType button, const Window &relative);

		static Vector2i GetPosition();
		static Vector2i GetPosition(const Window &relative);

		static void SetPosition(const Vector2i &position);
		static void SetPosition(const Vector2i &position, const Window &relative);

		static void SetPosition(int x, int y, const Window &relative);
		static void SetPosition(int x, int y);

		static void RegisterLua(sol::state& lua);
	};
}
