#include "pch.h"
#include "Keyboard.h"

#include <Windows.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <limits>

#include "Window.h"
#include "Application.h"

namespace
{
	void RegisterLuaKeys(sol::state &lua)
	{
		auto keys = lua["Keyboard"]["Key"];
	
		keys["Space"]          = Game::Key::Space;
		keys["Quote"]          = Game::Key::Quote;
		keys["Comma"]          = Game::Key::Comma;
		keys["Hyphen"]         = Game::Key::Hyphen;
		keys["Period"]         = Game::Key::Period;
		keys["Slash"]          = Game::Key::Slash;
		keys["Num0"]           = Game::Key::Num0;
		keys["Num1"]           = Game::Key::Num1;
		keys["Num2"]           = Game::Key::Num2;
		keys["Num3"]           = Game::Key::Num3;
		keys["Num4"]           = Game::Key::Num4;
		keys["Num5"]           = Game::Key::Num5;
		keys["Num6"]           = Game::Key::Num6;
		keys["Num7"]           = Game::Key::Num7;
		keys["Num8"]           = Game::Key::Num8;
		keys["Num9"]           = Game::Key::Num9;
		keys["Semicolon"]      = Game::Key::Semicolon;
		keys["Equal"]          = Game::Key::Equal;
		keys["A"]              = Game::Key::A;
		keys["B"]              = Game::Key::B;
		keys["C"]              = Game::Key::C;
		keys["D"]              = Game::Key::D;
		keys["E"]              = Game::Key::E;
		keys["F"]              = Game::Key::F;
		keys["G"]              = Game::Key::G;
		keys["H"]              = Game::Key::H;
		keys["I"]              = Game::Key::I;
		keys["J"]              = Game::Key::J;
		keys["K"]              = Game::Key::K;
		keys["L"]              = Game::Key::L;
		keys["M"]              = Game::Key::M;
		keys["N"]              = Game::Key::N;
		keys["O"]              = Game::Key::O;
		keys["P"]              = Game::Key::P;
		keys["Q"]              = Game::Key::Q;
		keys["R"]              = Game::Key::R;
		keys["S"]              = Game::Key::S;
		keys["T"]              = Game::Key::T;
		keys["U"]              = Game::Key::U;
		keys["V"]              = Game::Key::V;
		keys["W"]              = Game::Key::W;
		keys["X"]              = Game::Key::X;
		keys["Y"]              = Game::Key::Y;
		keys["Z"]              = Game::Key::Z;
		keys["LeftBracket"]    = Game::Key::LeftBracket;
		keys["Backslash"]      = Game::Key::Backslash;
		keys["RightBracket"]   = Game::Key::RightBracket;
		keys["Tilde"]          = Game::Key::Tilde;
		keys["World1"]         = Game::Key::World1;
		keys["World2"]         = Game::Key::World2;
		keys["Escape"]         = Game::Key::Escape;
		keys["Enter"]          = Game::Key::Enter;
		keys["Tab"]            = Game::Key::Tab;
		keys["Backspace"]      = Game::Key::Backspace;
		keys["Insert"]         = Game::Key::Insert;
		keys["Delete"]         = Game::Key::Delete;
		keys["Right"]          = Game::Key::Right;
		keys["Left"]           = Game::Key::Left;
		keys["Down"]           = Game::Key::Down;
		keys["Up"]             = Game::Key::Up;
		keys["PageUp"]         = Game::Key::PageUp;
		keys["PageDown"]       = Game::Key::PageDown;
		keys["Home"]           = Game::Key::Home;
		keys["End"]            = Game::Key::End;
		keys["CapsLock"]       = Game::Key::CapsLock;
		keys["ScrollLock"]     = Game::Key::ScrollLock;
		keys["NumLock"]        = Game::Key::NumLock;
		keys["PrintScreen"]    = Game::Key::PrintScreen;
		keys["Pause"]          = Game::Key::Pause;
		keys["F1"]             = Game::Key::F1;
		keys["F2"]             = Game::Key::F2;
		keys["F3"]             = Game::Key::F3;
		keys["F4"]             = Game::Key::F4;
		keys["F5"]             = Game::Key::F5;
		keys["F6"]             = Game::Key::F6;
		keys["F7"]             = Game::Key::F7;
		keys["F8"]             = Game::Key::F8;
		keys["F9"]             = Game::Key::F9;
		keys["F10"]            = Game::Key::F10;
		keys["F11"]            = Game::Key::F11;
		keys["F12"]            = Game::Key::F12;
		keys["F13"]            = Game::Key::F13;
		keys["F14"]            = Game::Key::F14;
		keys["F15"]            = Game::Key::F15;
		keys["F16"]            = Game::Key::F16;
		keys["F17"]            = Game::Key::F17;
		keys["F18"]            = Game::Key::F18;
		keys["F19"]            = Game::Key::F19;
		keys["F20"]            = Game::Key::F20;
		keys["F21"]            = Game::Key::F21;
		keys["F22"]            = Game::Key::F22;
		keys["F23"]            = Game::Key::F23;
		keys["F24"]            = Game::Key::F24;
		keys["Numpad0"]        = Game::Key::Numpad0;
		keys["Numpad1"]        = Game::Key::Numpad1;
		keys["Numpad2"]        = Game::Key::Numpad2;
		keys["Numpad3"]        = Game::Key::Numpad3;
		keys["Numpad4"]        = Game::Key::Numpad4;
		keys["Numpad5"]        = Game::Key::Numpad5;
		keys["Numpad6"]        = Game::Key::Numpad6;
		keys["Numpad7"]        = Game::Key::Numpad7;
		keys["Numpad8"]        = Game::Key::Numpad8;
		keys["Numpad9"]        = Game::Key::Numpad9;
		keys["NumpadDecimal"]  = Game::Key::NumpadDecimal;
		keys["NumpadDivide"]   = Game::Key::NumpadDivide;
		keys["NumpadMultiply"] = Game::Key::NumpadMultiply;
		keys["NumpadSubtract"] = Game::Key::NumpadSubtract;
		keys["NumpadAdd"]      = Game::Key::NumpadAdd;
		keys["NumpadEnter"]    = Game::Key::NumpadEnter;
		keys["NumpadEqual"]    = Game::Key::NumpadEqual;
		keys["LeftShift"]      = Game::Key::LeftShift;
		keys["LeftControl"]    = Game::Key::LeftControl;
		keys["LeftAlt"]        = Game::Key::LeftAlt;
		keys["LeftSuper"]      = Game::Key::LeftSuper;
		keys["RightShift"]     = Game::Key::RightShift;
		keys["RightControl"]   = Game::Key::RightControl;
		keys["RightAlt"]       = Game::Key::RightAlt;
		keys["RightSuper"]     = Game::Key::RightSuper;
		keys["Menu"]           = Game::Key::Menu;
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
		lua.create_named_table("Keyboard", "IsKeyPressed", ::IsKeyPressed);
		lua["Keyboard"]["Key"] = lua.create_table_with();

		RegisterLuaKeys(lua);
	}
}
