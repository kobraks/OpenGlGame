#pragma once

#include "Engine/Core/Base.h"
#include "Engine/OpenGL/OpenGlFunctions.h"

#include <unordered_map>

namespace Game
{
	class Window;

	class Context
	{
		static inline std::unordered_map<std::thread::id, Context*> s_Contests; 
		static inline thread_local Context* s_Context = nullptr;
		
		void *m_WindowHandler = nullptr;
		Scope<OpenGlFunctions> m_Functions = nullptr;
		
		std::thread::id m_ThreadId;
	public:
		Context(const Context &) = delete;
		Context(Context &&)      = delete;
		~Context();

		Context& operator=(const Context &) = delete;
		Context& operator=(Context &&)      = delete;

		static Scope<Context> Create(const Window& window);

		bool operator==(const Context &context) const;
		bool operator!=(const Context &context) const;

		void SwapBuffers();
		[[nodiscard]] OpenGlFunctions GetFunctions() const { return *m_Functions; }
		void MakeCurrent();

		[[nodiscard]] bool IsCurrent() const;

		static Context* GetCurrentContext();
		static Context* GetContext();
	private:
		Context(void *windowHandler);
	};
}
