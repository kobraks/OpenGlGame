#pragma once
#include <unordered_map>

#include "OpenGlFunctions.h"
#include "Types.h"

namespace Game
{
	class Window;

	class Context
	{
		const std::thread::id m_ThreadId;
		void *m_WindowHandler = nullptr;
		OpenGlFunctions m_Functions;
	
	public:
		Context(const Context &) = delete;
		Context(Context &&)      = delete;

		Context& operator=(const Context &) = delete;
		Context& operator=(Context &&)      = delete;

		static Scope<Context> Create(const Window& window);

		bool operator==(const Context &context) const;
		bool operator!=(const Context &context) const;

		void SwapBuffers();
		OpenGlFunctions GetFunctions() const { return m_Functions; }
		void MakeCurrent();

	private:
		Context(void *windowHandler);
		Context(void *windowHandler, void *mainWindow);
	};
}
