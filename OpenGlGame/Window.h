#pragma once
#include <String>
#include <functional>
#include <GLFW/glfw3.h>
#include <string_view>

#include "Event.h"
#include "Types.h"
#include "Vector2.h"
#include "Context.h"


namespace Game
{
	struct WindowProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProperties(const std::string title, uint32_t width, uint32_t height) : Title(title),
			Width(width),
			Height(height) { }
	};

	class Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event &)>;

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			Vector2u Position;

			bool VSync;
			EventCallbackFunction EventCallback;
		};


		GLFWwindow *m_Window;
		Scope<Context> m_Context;

		WindowData m_Data;
	public:
		explicit Window(const WindowProperties &properties);
		virtual ~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		std::string_view GetTitle() const { return m_Data.Title; }

		Vector2<uint32_t> GetSize() const { return Vector2u{GetWidth(), GetHeight()}; }

		void SetEventCallback(const EventCallbackFunction &callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const { return m_Data.VSync; };

		void* GetNativeWindow() const { return m_Window; };

		OpenGlFunctions GetFunctions() const;

		void SetPosition(const uint32_t x, const uint32_t y);
		void SetPosition(const Vector2u& position) { return SetPosition(position.X, position.Y); }

		Vector2u GetPosition() const { return m_Data.Position; }

		void SetTitle(const std::string& title);

		void Visible(const bool visible);

		bool IsVisible() const;

		void AttentionRequest();
	protected:
		void Init(const WindowProperties &props);
		void Shutdown();
	};
}
