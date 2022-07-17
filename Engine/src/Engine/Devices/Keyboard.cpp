#include "pch.h"
#include "Engine/Devices/Keyboard.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"
#include "Engine/Lua/LuaUtils.h"

#include <Windows.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <limits>

namespace
{
#define CREATE_ENUM_BIND(e, v) #v, Game::##e##::##v
	
	sol::table RegisterLuaKeys(sol::state &lua)
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

	bool IsKeyPressed(int32_t key)
	{
		key = std::clamp(key, static_cast<int>(std::numeric_limits<Game::KeyCode>::min()), static_cast<int>(std::numeric_limits<Game::KeyCode>::max()));

		return Game::Keyboard::IsKeyPressed(static_cast<Game::KeyCode>(key), Game::Application::Get().GetWindow());
	}

	constexpr int32_t ToWinKey(Game::KeyCode key)
	{
		using namespace Game;
		switch(key)
		{
			default: return 0;
			case Key::A: return 'A';
			case Key::B: return 'B';
			case Key::C: return 'C';
			case Key::D: return 'D';
			case Key::E: return 'E';
			case Key::F: return 'F';
			case Key::G: return 'G';
			case Key::H: return 'H';
			case Key::I: return 'I';
			case Key::J: return 'J';
			case Key::K: return 'K';
			case Key::L: return 'L';
			case Key::M: return 'M';
			case Key::N: return 'N';
			case Key::O: return 'O';
			case Key::P: return 'P';
			case Key::Q: return 'Q';
			case Key::R: return 'R';
			case Key::S: return 'S';
			case Key::T: return 'T';
			case Key::U: return 'U';
			case Key::V: return 'V';
			case Key::W: return 'W';
			case Key::X: return 'X';
			case Key::Y: return 'Y';
			case Key::Z: return 'Z';
			case Key::Num0: return '0';
			case Key::Num1: return '1';
			case Key::Num2: return '2';
			case Key::Num3: return '3';
			case Key::Num4: return '4';
			case Key::Num5: return '5';
			case Key::Num6: return '6';
			case Key::Num7: return '7';
			case Key::Num8: return '8';
			case Key::Num9: return '9';
			case Key::Numpad0: return VK_NUMPAD0;
			case Key::Numpad1: return VK_NUMPAD1;
			case Key::Numpad2: return VK_NUMPAD2;
			case Key::Numpad3: return VK_NUMPAD3;
			case Key::Numpad4: return VK_NUMPAD4;
			case Key::Numpad5: return VK_NUMPAD5;
			case Key::Numpad6: return VK_NUMPAD6;
			case Key::Numpad7: return VK_NUMPAD7;
			case Key::Numpad8: return VK_NUMPAD8;
			case Key::Numpad9: return VK_NUMPAD9;
			case Key::F1: return VK_F1;
			case Key::F2: return VK_F2;
			case Key::F3: return VK_F3;
			case Key::F4: return VK_F4;
			case Key::F5: return VK_F5;
			case Key::F6: return VK_F6;
			case Key::F7: return VK_F7;
			case Key::F8: return VK_F8;
			case Key::F9: return VK_F9;
			case Key::F10: return VK_F10;
			case Key::F11: return VK_F11;
			case Key::F12: return VK_F12;
			case Key::F13: return VK_F13;
			case Key::F14: return VK_F14;
			case Key::F15: return VK_F15;
			case Key::F16: return VK_F16;
			case Key::F17: return VK_F17;
			case Key::F18: return VK_F18;
			case Key::F19: return VK_F19;
			case Key::F20: return VK_F20;
			case Key::F21: return VK_F21;
			case Key::F22: return VK_F22;
			case Key::F23: return VK_F23;
			case Key::F24: return VK_F24;

			case Key::Escape: return VK_ESCAPE;

			case Key::Space: return VK_SPACE;
			case Key::Enter: return VK_RETURN;
			case Key::NumpadEnter: return VK_RETURN;

			case Key::NumpadAdd: return VK_ADD;
			case Key::NumpadSubtract: return VK_SUBTRACT;

			case Key::Pause: return VK_PAUSE;
			case Key::Quote: return VK_OEM_7;
			case Key::Comma: return VK_OEM_COMMA;
			case Key::Hyphen: return VK_OEM_MINUS;
			case Key::Period: return VK_OEM_PERIOD;
			case Key::Slash: return VK_OEM_2;
			case Key::Equal: return VK_OEM_PLUS;
			case Key::Semicolon: return VK_OEM_1;

			case Key::LeftBracket: return VK_OEM_4;
			case Key::RightBracket: return VK_OEM_6;
			case Key::Backslash: return VK_OEM_5;
			case Key::Tilde: return VK_OEM_3;

			case Key::World1: return 0;
			case Key::World2: return 0;

			case Key::Tab: return VK_TAB;
			case Key::Backspace: return VK_BACK;
			case Key::Insert: return VK_INSERT;
			case Key::Delete: return VK_DELETE;
			case Key::Right: return VK_RIGHT;
			case Key::Left: return VK_LEFT;
			case Key::Down: return VK_DOWN;
			case Key::Up: return VK_UP;
			case Key::PageUp: return VK_PRIOR;
			case Key::PageDown: return VK_NEXT;
			case Key::Home: return VK_HOME;
			case Key::End: return VK_END;
			case Key::CapsLock: return VK_CAPITAL;
			case Key::ScrollLock: return VK_SCROLL;
			case Key::NumLock: return VK_NUMLOCK;
			case Key::PrintScreen: return VK_SNAPSHOT;

			case Key::LeftShift: return VK_LSHIFT;
			case Key::LeftControl: return VK_LCONTROL;
			case Key::LeftAlt: return VK_LMENU;
			case Key::LeftSuper: return VK_LWIN;

			case Key::RightShift: return VK_RSHIFT;
			case Key::RightControl: return VK_RCONTROL;
			case Key::RightAlt: return VK_RMENU;
			case Key::RightSuper: return VK_RWIN;

			case Key::Menu: return VK_MENU;
		}
	}
}

namespace Game
{
	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		return (GetAsyncKeyState(ToWinKey(key)) & 0x8000) != 0;
	}

	bool Keyboard::IsKeyPressed(KeyCode key, const Window &window)
	{
		return glfwGetKey(static_cast<GLFWwindow*>(window.GetNativeWindow()), key) == GLFW_PRESS;
	}

	void Keyboard::RegisterLua(sol::state &lua)
	{
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
