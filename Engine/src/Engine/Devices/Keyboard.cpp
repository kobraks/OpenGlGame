#include "pch.h"

#include "Engine/Devices/Keyboard.h"
#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

// #include <Windows.h>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <limits>

#include <sol/state.hpp>

namespace Engine {
	/*bool Keyboard::IsKeyPressed(KeyCode key) {
		return (GetAsyncKeyState(ToWinKey(key)) & 0x8000) != 0;
	}*/


	bool Keyboard::IsKeyPressed(KeyCode key, const Window &window) {
		return glfwGetKey(static_cast<GLFWwindow *>(window.GetNativeWindow()), key) == GLFW_PRESS;
	}

	bool Keyboard::IsKeyPressed(KeyCode key) {
		return IsKeyPressed(key, Application::Get().GetWindow());
	}


	void Keyboard::RegisterLua(sol::state &lua) {
		//TODO
	}

}