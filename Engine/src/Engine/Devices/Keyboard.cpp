#include "pch.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

#include "Engine/Utils/LuaUtils.h"

// #include <Windows.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <limits>

#include <sol/state.hpp>

#define CREATE_ENUM_BIND(e, v) #v, Engine::##e##::##v

namespace {
	sol::table RegisterLuaKeys(sol::state& lua)
	{
		auto keys = lua.create_table_with();

		keys.set(CREATE_ENUM_BIND(Key, Space));
		keys.set(CREATE_ENUM_BIND(Key, Quote));
		keys.set(CREATE_ENUM_BIND(Key, Comma));
		keys.set(CREATE_ENUM_BIND(Key, Hyphen));
		keys.set(CREATE_ENUM_BIND(Key, Period));
		keys.set(CREATE_ENUM_BIND(Key, Slash));
		keys.set(CREATE_ENUM_BIND(Key, Num0));
		keys.set(CREATE_ENUM_BIND(Key, Num1));
		keys.set(CREATE_ENUM_BIND(Key, Num2));
		keys.set(CREATE_ENUM_BIND(Key, Num3));
		keys.set(CREATE_ENUM_BIND(Key, Num4));
		keys.set(CREATE_ENUM_BIND(Key, Num5));
		keys.set(CREATE_ENUM_BIND(Key, Num6));
		keys.set(CREATE_ENUM_BIND(Key, Num7));
		keys.set(CREATE_ENUM_BIND(Key, Num8));
		keys.set(CREATE_ENUM_BIND(Key, Num9));
		keys.set(CREATE_ENUM_BIND(Key, Semicolon));
		keys.set(CREATE_ENUM_BIND(Key, Equal));
		keys.set(CREATE_ENUM_BIND(Key, A));
		keys.set(CREATE_ENUM_BIND(Key, B));
		keys.set(CREATE_ENUM_BIND(Key, C));
		keys.set(CREATE_ENUM_BIND(Key, D));
		keys.set(CREATE_ENUM_BIND(Key, E));
		keys.set(CREATE_ENUM_BIND(Key, F));
		keys.set(CREATE_ENUM_BIND(Key, G));
		keys.set(CREATE_ENUM_BIND(Key, H));
		keys.set(CREATE_ENUM_BIND(Key, I));
		keys.set(CREATE_ENUM_BIND(Key, J));
		keys.set(CREATE_ENUM_BIND(Key, K));
		keys.set(CREATE_ENUM_BIND(Key, L));
		keys.set(CREATE_ENUM_BIND(Key, M));
		keys.set(CREATE_ENUM_BIND(Key, N));
		keys.set(CREATE_ENUM_BIND(Key, O));
		keys.set(CREATE_ENUM_BIND(Key, P));
		keys.set(CREATE_ENUM_BIND(Key, Q));
		keys.set(CREATE_ENUM_BIND(Key, R));
		keys.set(CREATE_ENUM_BIND(Key, S));
		keys.set(CREATE_ENUM_BIND(Key, T));
		keys.set(CREATE_ENUM_BIND(Key, U));
		keys.set(CREATE_ENUM_BIND(Key, V));
		keys.set(CREATE_ENUM_BIND(Key, W));
		keys.set(CREATE_ENUM_BIND(Key, X));
		keys.set(CREATE_ENUM_BIND(Key, Y));
		keys.set(CREATE_ENUM_BIND(Key, Z));
		keys.set(CREATE_ENUM_BIND(Key, LeftBracket));
		keys.set(CREATE_ENUM_BIND(Key, Backslash));
		keys.set(CREATE_ENUM_BIND(Key, RightBracket));
		keys.set(CREATE_ENUM_BIND(Key, Tilde));
		keys.set(CREATE_ENUM_BIND(Key, World1));
		keys.set(CREATE_ENUM_BIND(Key, World2));
		keys.set(CREATE_ENUM_BIND(Key, Escape));
		keys.set(CREATE_ENUM_BIND(Key, Enter));
		keys.set(CREATE_ENUM_BIND(Key, Tab));
		keys.set(CREATE_ENUM_BIND(Key, Backspace));
		keys.set(CREATE_ENUM_BIND(Key, Insert));
		keys.set(CREATE_ENUM_BIND(Key, Delete));
		keys.set(CREATE_ENUM_BIND(Key, Right));
		keys.set(CREATE_ENUM_BIND(Key, Left));
		keys.set(CREATE_ENUM_BIND(Key, Down));
		keys.set(CREATE_ENUM_BIND(Key, Up));
		keys.set(CREATE_ENUM_BIND(Key, PageUp));
		keys.set(CREATE_ENUM_BIND(Key, PageDown));
		keys.set(CREATE_ENUM_BIND(Key, Home));
		keys.set(CREATE_ENUM_BIND(Key, End));
		keys.set(CREATE_ENUM_BIND(Key, CapsLock));
		keys.set(CREATE_ENUM_BIND(Key, ScrollLock));
		keys.set(CREATE_ENUM_BIND(Key, NumLock));
		keys.set(CREATE_ENUM_BIND(Key, PrintScreen));
		keys.set(CREATE_ENUM_BIND(Key, Pause));
		keys.set(CREATE_ENUM_BIND(Key, F1));
		keys.set(CREATE_ENUM_BIND(Key, F2));
		keys.set(CREATE_ENUM_BIND(Key, F3));
		keys.set(CREATE_ENUM_BIND(Key, F4));
		keys.set(CREATE_ENUM_BIND(Key, F5));
		keys.set(CREATE_ENUM_BIND(Key, F6));
		keys.set(CREATE_ENUM_BIND(Key, F7));
		keys.set(CREATE_ENUM_BIND(Key, F8));
		keys.set(CREATE_ENUM_BIND(Key, F9));
		keys.set(CREATE_ENUM_BIND(Key, F10));
		keys.set(CREATE_ENUM_BIND(Key, F11));
		keys.set(CREATE_ENUM_BIND(Key, F12));
		keys.set(CREATE_ENUM_BIND(Key, F13));
		keys.set(CREATE_ENUM_BIND(Key, F14));
		keys.set(CREATE_ENUM_BIND(Key, F15));
		keys.set(CREATE_ENUM_BIND(Key, F16));
		keys.set(CREATE_ENUM_BIND(Key, F17));
		keys.set(CREATE_ENUM_BIND(Key, F18));
		keys.set(CREATE_ENUM_BIND(Key, F19));
		keys.set(CREATE_ENUM_BIND(Key, F20));
		keys.set(CREATE_ENUM_BIND(Key, F21));
		keys.set(CREATE_ENUM_BIND(Key, F22));
		keys.set(CREATE_ENUM_BIND(Key, F23));
		keys.set(CREATE_ENUM_BIND(Key, F24));
		keys.set(CREATE_ENUM_BIND(Key, Numpad0));
		keys.set(CREATE_ENUM_BIND(Key, Numpad1));
		keys.set(CREATE_ENUM_BIND(Key, Numpad2));
		keys.set(CREATE_ENUM_BIND(Key, Numpad3));
		keys.set(CREATE_ENUM_BIND(Key, Numpad4));
		keys.set(CREATE_ENUM_BIND(Key, Numpad5));
		keys.set(CREATE_ENUM_BIND(Key, Numpad6));
		keys.set(CREATE_ENUM_BIND(Key, Numpad7));
		keys.set(CREATE_ENUM_BIND(Key, Numpad8));
		keys.set(CREATE_ENUM_BIND(Key, Numpad9));
		keys.set(CREATE_ENUM_BIND(Key, NumpadDecimal));
		keys.set(CREATE_ENUM_BIND(Key, NumpadDivide));
		keys.set(CREATE_ENUM_BIND(Key, NumpadMultiply));
		keys.set(CREATE_ENUM_BIND(Key, NumpadSubtract));
		keys.set(CREATE_ENUM_BIND(Key, NumpadAdd));
		keys.set(CREATE_ENUM_BIND(Key, LeftShift));
		keys.set(CREATE_ENUM_BIND(Key, LeftControl));
		keys.set(CREATE_ENUM_BIND(Key, LeftAlt));
		keys.set(CREATE_ENUM_BIND(Key, LeftSuper));
		keys.set(CREATE_ENUM_BIND(Key, RightShift));
		keys.set(CREATE_ENUM_BIND(Key, RightControl));
		keys.set(CREATE_ENUM_BIND(Key, RightAlt));
		keys.set(CREATE_ENUM_BIND(Key, RightSuper));
		keys.set(CREATE_ENUM_BIND(Key, Menu));

		return keys;
	}

	bool IsKeyPressed(int32_t key) {
		key = std::clamp(key, static_cast<int>(std::numeric_limits<Engine::KeyCode>::min()), static_cast<int>(std::numeric_limits<Engine::KeyCode>::max()));

		return Engine::Keyboard::IsKeyPressed(static_cast<Engine::KeyCode>(key));
	}
}

namespace Engine {
	/*bool Keyboard::IsKeyPressed(KeyCode key) {
		return (GetAsyncKeyState(ToWinKey(key)) & 0x8000) != 0;
	}*/

	bool Keyboard::IsKeyPressed(KeyCode key, const Window &window) {
		return glfwGetKey(window.GetNativeHandle<GLFWwindow>(), key) == GLFW_PRESS;
	}

	bool Keyboard::IsKeyPressed(KeyCode key) {
		return IsKeyPressed(key, Application::Get().GetWindow());
	}


	void Keyboard::RegisterLua(sol::state &lua) {
		auto keyboard = lua.create_named_table("Keyboard");

		auto keyMetaTable = RegisterLuaKeys(lua);
		auto keyboardMetaTable = lua.create_table_with();

		keyboardMetaTable["IsKeyPressed"] = ::IsKeyPressed;
		keyboardMetaTable["Key"] = keyMetaTable;

		SetAsReadOnlyTable(keyboardMetaTable["Key"], keyMetaTable, Deny);
		SetAsReadOnlyTable(keyboard, keyboardMetaTable, Deny);

		RegisterLuaKeys(lua);
	}

}