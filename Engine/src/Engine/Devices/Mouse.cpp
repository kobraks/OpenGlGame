#include "pch.h"
#include "Engine/Devices/Mouse.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

#include "Engine/Utils/LuaUtils.h"

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

		return Engine::Mouse::IsButtonPressed(static_cast<Engine::MouseCode>(button), Engine::Application::Get().GetWindow());
	}

	static std::pair<float, float> GetMousePosition() {
		const auto pos = Engine::Mouse::GetPosition(Engine::Application::Get().GetWindow());
		return std::make_pair(pos.X, pos.Y);
	}

	static void SetMousePosition(float x, float y) {
		Engine::Mouse::SetPosition(x, y, Engine::Application::Get().GetWindow());
	}
}

namespace Engine {
	Vector2f Mouse::s_Delta{ 0.0f, 0.0f };
	Vector2f Mouse::s_LastPosition{ 0.0f, 0.0f };

	bool Mouse::IsButtonPressed(MouseCode button, const Window &relative) {
		return glfwGetMouseButton(relative.GetNativeHandle<GLFWwindow>(), button) == GLFW_PRESS;
	}

	bool Mouse::IsButtonPressed(MouseCode button) {
		return IsButtonPressed(button, Application::Get().GetWindow());
	}

	Vector2f Mouse::GetPosition() {
		return GetPosition(Application::Get().GetWindow());
	}

	void Mouse::SetPosition(const Vector2f &pos) {
		SetPosition(pos, Application::Get().GetWindow());
	}

	void Mouse::SetPosition(float x, float y) {
		SetPosition(x, y, Application::Get().GetWindow());
	}

	float Mouse::GetX() {
		return GetPosition().X;
	}

	float Mouse::GetY() {
		return GetPosition().Y;
	}

	void Mouse::UpdateDelta() {
		Vector2f current = GetPosition();
		s_Delta.X = current.X - s_LastPosition.X;
		s_Delta.Y = current.Y - s_LastPosition.Y;
		s_LastPosition = current;
	}

	void Mouse::LockCursor(bool enabled) {
		auto& window = Application::Get().GetWindow();
		window.SetCursorMode(enabled ? CursorMode::Disabled : CursorMode::Normal);
	}

	void Mouse::ShowCursor(bool show) {
		auto& window = Application::Get().GetWindow();
		window.SetCursorMode(show ? CursorMode::Normal : CursorMode::Hidden);
	}

	bool Mouse::IsCursorVisible() {
		const auto& window = Application::Get().GetWindow();
		return window.GetCursorMode() == CursorMode::Normal;
	}

	void Mouse::ToggleCursorLock() {
		LockCursor(!IsCursorLocked());
	}

	bool Mouse::IsCursorLocked() {
		const auto& window = Application::Get().GetWindow();
		return window.GetCursorMode() == CursorMode::Disabled;
	}

	Vector2f Mouse::GetPosition(const Window &relative) {
		double x, y;
		glfwGetCursorPos(relative.GetNativeHandle<GLFWwindow>(), &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	void Mouse::SetPosition(const Vector2f &pos, const Window &relative) {
		const double x = static_cast<double>(pos.X);
		const double y = static_cast<double>(pos.Y);

		glfwSetCursorPos(relative.GetNativeHandle<GLFWwindow>(), x, y);
	}

	void Mouse::SetPosition(float x, float y, const Window &relative) {
		glfwSetCursorPos(relative.GetNativeHandle<GLFWwindow>(), static_cast<double>(x), static_cast<double>(y));
	}

	void Mouse::RegisterLua(sol::state &lua) {
		auto mouseTable = lua.create_named_table("Mouse");

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

		mouseTable.set_function("LockCursor", &Mouse::LockCursor);
		mouseTable.set_function("ToggleCursorLock", &Mouse::ToggleCursorLock);
		mouseTable.set_function("IsCursorLocked", &Mouse::IsCursorLocked);
		mouseTable.set_function("IsCursorVisible", &Mouse::IsCursorVisible);
		mouseTable.set_function("ShowCursor", &Mouse::ShowCursor);

		SetAsReadOnlyTable(mouseMetaTable["Buttons"], buttonEnum, Deny);
		SetAsReadOnlyTable(mouseTable, mouseMetaTable, Deny);
	}

}