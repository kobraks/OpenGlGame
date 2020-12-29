#include "pch.h"
#include "Keyboard.h"

#include <Windows.h>
#include "GLFW/glfw3.h"

namespace Game
{
	constexpr static int32_t ToWinKey(KeyCode key)
	{
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

		case Key::NumpadAdd: return VK_OEM_PLUS;
		case Key::NumpadSubtract: return VK_OEM_MINUS;
		}
	}
	
	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		return GetAsyncKeyState(ToWinKey(key));
	}

}