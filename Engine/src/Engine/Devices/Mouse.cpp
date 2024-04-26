#include "pch.h"
#include "Engine/Devices/Mouse.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

// #include <Windows.h>
#include <GLFW/glfw3.h>

#include <sol/state.hpp>

namespace {
	static bool IsButtonPressed(int button) {
		if (button < Engine::MouseButton::Button0) {
			button = Engine::MouseButton::Button0;
			LOG_SCRIPT_WARN("Given button doesn't exists replaced with Button0");
		}

		if (button < Engine::MouseButton::Button7) {
			button = Engine::MouseButton::Button7;
			LOG_SCRIPT_WARN("Given button doesn't exists replaced with Button7");
		}

		return Engine::Mouse::IsButtonPressed(button, Engine::Application::Get().GetWindow());
	}

	static std::pair<int32_t, int32_t> GetMousePosition() {
		const auto pos = Engine::Mouse::GetPosition(Engine::Application::Get().GetWindow());
		return std::make_pair(pos.X, pos.Y);
	}

	/*static std::pair<int32_t, int32_t> GetMouseScreenPosition() {
		const auto pos = Engine::Mouse::GetPosition();
		return std::make_pair(pos.X, pos.Y);
	}*/

	static void SetMousePosition(int x, int y) {
		Engine::Mouse::SetPosition(x, y, Engine::Application::Get().GetWindow());
	}

	/*static void SetMoseScreenPosition(int x, int y) {
		Engine::Mouse::SetPosition(x, y);
	}*/
}

namespace Engine {
	/*bool Mouse::IsButtonPressed(MouseButtonCode button) {
		int vKey = 0;

		switch(button) {
			case MouseButton::Button0: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
			break;
			case MouseButton::Button1: vKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
			break;
			case MouseButton::Button2: vKey = VK_MBUTTON;
			break;
			case MouseButton::Button3: vKey = VK_XBUTTON1;
			break;
			case MouseButton::Button4: vKey = VK_XBUTTON2;
			break;
			default: vKey = 0;
			break;
		}

		return (GetAsyncKeyState(vKey) & 0x8000) != 0;
	}*/

	bool Mouse::IsButtonPressed(MouseButtonCode button, const Window &relative) {
		return glfwGetMouseButton(static_cast<GLFWwindow *>(relative.GetNativeWindow()), button) == GLFW_PRESS;
	}

	/*Vector2i Mouse::GetPosition() {
		POINT point;
		GetCursorPos(&point);

		return Vector2i(point.x, point.y);
	}*/

	Vector2i Mouse::GetPosition(const Window &relative) {
		const auto window = static_cast<GLFWwindow*>(relative.GetNativeWindow());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { static_cast<int32_t>(x), static_cast<int32_t>(y) };
	}

	/*void Mouse::SetPosition(const Vector2i &pos) {
		SetCursorPos(pos.X, pos.Y);
	}*/

	void Mouse::SetPosition(const Vector2i &pos, const Window &relative) {
		const auto window = static_cast<GLFWwindow *>(relative.GetNativeWindow());

		const double x = static_cast<double>(pos.X);
		const double y = static_cast<double>(pos.Y);

		glfwSetCursorPos(window, x, y);
	}

	/*void Mouse::SetPosition(int32_t x, int32_t y) {
		SetCursorPos(x, y);
	}*/

	void Mouse::SetPosition(int32_t x, int32_t y, const Window &relative) {
		const auto window = static_cast<GLFWwindow *>(relative.GetNativeWindow());

		glfwSetCursorPos(window, static_cast<double>(x), static_cast<double>(y));
	}

	void Mouse::RegisterLua(sol::state &lua) {
		//TODO
	}

}