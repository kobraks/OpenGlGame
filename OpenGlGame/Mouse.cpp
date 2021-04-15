#include "pch.h"
#include "Mouse.h"

#include <Windows.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Application.h"
#include "LuaUtils.h"

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

#define ENUM_TO_STRING_ENUM(e, v) #v,  static_cast<int>(e::##v)

	void Mouse::RegisterLua(sol::state &lua)
	{
		auto buttonEnum = lua.create_table_with();
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button0));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button1));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button2));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button3));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button4));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button5));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button6));
		buttonEnum.set(ENUM_TO_STRING_ENUM(Button, Button7));
		buttonEnum.set("Left", static_cast<int>(ButtonLeft));
		buttonEnum.set("Middle", static_cast<int>(ButtonMiddle));
		buttonEnum.set("Right", static_cast<int>(ButtonRight));

		auto mouseMetaTable                      = lua.create_table_with();
		mouseMetaTable["IsButtonPressed"]        = IsMouseButtonPressed;
		mouseMetaTable["GetPosition"]            = GetMousePosition;
		mouseMetaTable["SetPosition"]            = SetMousePosition;
		mouseMetaTable["GetScreenPosition"]      = GetMouseScreenPosition;
		mouseMetaTable["SetMouseScreenPosition"] = SetMouseScreenPosition;
		mouseMetaTable["Buttons"]                = buttonEnum;

		SetAsReadOnlyTable(mouseMetaTable["Buttons"], buttonEnum, Deny);

		auto mouseTable = lua.create_named_table("Mouse");

		SetAsReadOnlyTable(mouseTable, mouseMetaTable, Deny);
	}
}
