#include "pch.h"
#include "Engine/Devices/Mouse.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"


#include <GLFW/glfw3.h>

#include <sol/state.hpp>

#define ENUM_TO_STRING_ENUM(e, v) #e, static_cast<int>(e::##v)

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

	static void SetMousePosition(int x, int y) {
		Engine::Mouse::SetPosition(x, y, Engine::Application::Get().GetWindow());
	}
}

namespace Engine {
	bool Mouse::IsButtonPressed(MouseCode button, const Window &relative) {
		return glfwGetMouseButton(static_cast<GLFWwindow *>(relative.GetNativeWindow()), button) == GLFW_PRESS;
	}

	bool Mouse::IsButtonPressed(MouseCode button) {
		return IsButtonPressed(button, Application::Get().GetWindow());
	}

	Vector2i Mouse::GetPosition() {
		return GetPosition(Application::Get().GetWindow());
	}

	void Mouse::SetPosition(const Vector2i &pos) {
		SetPosition(pos, Application::Get().GetWindow());
	}

	void Mouse::SetPosition(int32_t x, int32_t y) {
		SetPosition(x, y, Application::Get().GetWindow());
	}

	Vector2i Mouse::GetPosition(const Window &relative) {
		const auto window = static_cast<GLFWwindow*>(relative.GetNativeWindow());

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { static_cast<int32_t>(x), static_cast<int32_t>(y) };
	}

	void Mouse::SetPosition(const Vector2i &pos, const Window &relative) {
		const auto window = static_cast<GLFWwindow *>(relative.GetNativeWindow());

		const double x = static_cast<double>(pos.X);
		const double y = static_cast<double>(pos.Y);

		glfwSetCursorPos(window, x, y);
	}

	void Mouse::SetPosition(int32_t x, int32_t y, const Window &relative) {
		const auto window = static_cast<GLFWwindow *>(relative.GetNativeWindow());

		glfwSetCursorPos(window, static_cast<double>(x), static_cast<double>(y));
	}

	void Mouse::RegisterLua(sol::state &lua) {
		auto buttonEnum = lua.create_table_with();
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button0));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button1));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button2));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button3));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button4));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button5));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button6));
		buttonEnum.set(ENUM_TO_STRING_ENUM(MouseButton, Button7));
		buttonEnum.set("Left", static_cast<int>(MouseButton::ButtonLeft));
		buttonEnum.set("Middle", static_cast<int>(MouseButton::ButtonMiddle));
		buttonEnum.set("Right", static_cast<int>(MouseButton::ButtonRight));

		auto mouseMetaTable = lua.create_table_with();
		mouseMetaTable.set("IsButtonPressed", ::IsButtonPressed);
		mouseMetaTable.set("GetPosition", ::GetMousePosition);
		mouseMetaTable.set("SetPosition", ::SetMousePosition);
		mouseMetaTable.set("Buttons", buttonEnum);

		//TODO
	}

}