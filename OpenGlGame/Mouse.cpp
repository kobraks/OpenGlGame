#include "pch.h"
#include "Mouse.h"

#include <Windows.h>

#include "Window.h"
#include "GLFW/glfw3.h"

namespace Game
{
	bool Mouse::IsButtonPressed(CodeType button)
	{
		int vKey = 0;
		
		switch(button)
		{
		case Button::Button0: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON; break;
		case Button::Button1: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON; break;
		case Button::Button2: vKey = VK_MBUTTON; break;
		case Button::Button3: vKey = VK_XBUTTON1; break;
		case Button::Button4: vKey = VK_XBUTTON2; break;
		default: vKey = 0; break;
		}
		
		return (GetAsyncKeyState(vKey) & 0x8000) != 0;
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
		double x = position.X;
		double y = position.Y;

		glfwSetCursorPos(window, x, y);
		
	}
}
