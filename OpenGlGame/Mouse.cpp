#include "pch.h"
#include "Mouse.h"

#include <Windows.h>

#include "Window.h"
#include "Application.h"
#include "GLFW/glfw3.h"

namespace
{
	bool IsMouseButtonPressed(int button)
	{
		if(button < Game::Mouse::Button0)
		{
			button = Game::Mouse::Button0;
			LOG_WARN("Given button does not exits replaced with Button0");
		}

		if(button > Game::Mouse::Button7)
		{
			button = Game::Mouse::Button7;
			LOG_WARN("Given button does not exits replaced with Button7");
		}

		return Game::Mouse::IsButtonPressed(button, Game::Application::Get().GetWindow());
	}

	std::pair<int32_t, int32_t> GetMousePosition()
	{
		const auto pos = Game::Mouse::GetPosition(Game::Application::Get().GetWindow());
		return std::make_pair(pos.X, pos.Y);
	}

	std::pair<int32_t, int32_t> GetMouseScreenPosition()
	{
		const auto pos = Game::Mouse::GetPosition();
		return std::make_pair(pos.X, pos.Y);
	}

	void SetMousePosition(int x, int y)
	{
		Game::Mouse::SetPosition(x, y, Game::Application::Get().GetWindow());
	}

	void SetMouseScreenPosition(int x, int y)
	{
		Game::Mouse::SetPosition(x, y);
	}
}

namespace Game
{
	bool Mouse::IsButtonPressed(CodeType button)
	{
		int vKey = 0;

		switch(button)
		{
			case Button0: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
				break;
			case Button1: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
				break;
			case Button2: vKey = VK_MBUTTON;
				break;
			case Button3: vKey = VK_XBUTTON1;
				break;
			case Button4: vKey = VK_XBUTTON2;
				break;
			default: vKey = 0;
				break;
		}

		return (GetAsyncKeyState(vKey) & 0x8000) != 0;
	}

	bool Mouse::IsButtonPressed(CodeType button, const Window &relative)
	{
		return glfwGetMouseButton(static_cast<GLFWwindow*>(relative.GetNativeWindow()), button) == GLFW_PRESS;
	}

	Vector2i Mouse::GetPosition()
	{
		POINT point;
		GetCursorPos(&point);

		return Vector2i(point.x, point.y);
	}

	Vector2i Mouse::GetPosition(const Window &relative)
	{
		const auto window = static_cast<GLFWwindow*>(relative.GetNativeWindow());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return {static_cast<int>(x), static_cast<int>(y)};
	}

	void Mouse::SetPosition(const Vector2i &position)
	{
		SetCursorPos(position.X, position.Y);
	}

	void Mouse::SetPosition(const Vector2i &position, const Window &relative)
	{
		const auto window = static_cast<GLFWwindow*>(relative.GetNativeWindow());
		double x          = position.X;
		double y          = position.Y;

		glfwSetCursorPos(window, x, y);
	}

	void Mouse::SetPosition(int x, int y, const Window &relative)
	{
		glfwSetCursorPos(static_cast<GLFWwindow*>(relative.GetNativeWindow()), x, y);
	}

	void Mouse::SetPosition(int x, int y)
	{
		SetCursorPos(x, y);
	}

	void Mouse::RegisterLua(sol::state &lua)
	{
		auto buttonTable = lua.create_table_with(
		                                         "Button0",
		                                         Button0,
		                                         "Button1",
		                                         Button1,
		                                         "Button2",
		                                         Button2,
		                                         "Button3",
		                                         Button3,
		                                         "Button4",
		                                         Button4,
		                                         "Button5",
		                                         Button5,
		                                         "Button6",
		                                         Button6,
		                                         "Button7",
		                                         Button7,
		                                         "Left",
		                                         ButtonLeft,
		                                         "Middle",
		                                         ButtonMiddle,
		                                         "Right",
		                                         ButtonRight
		                                        );

		lua.create_named_table(
		                       "Mouse",
		                       "Buttons",
		                       buttonTable,
		                       "IsButtonPressed",
		                       IsMouseButtonPressed,
		                       "GetPosition",
		                       GetMousePosition,
		                       "SetPosition",
		                       SetMousePosition,
		                       "GetScreenPosition",
		                       GetMouseScreenPosition,
		                       "SetMouseScreenPosition",
		                       SetMouseScreenPosition
		                      );
	}
}
