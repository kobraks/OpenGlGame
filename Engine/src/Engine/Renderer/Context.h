#pragma once

#include <Engine/Core/Base.h>

namespace Engine {
	class Window;

	struct OpenGLVersion {
		int32_t Major;
		int32_t Minor;
	};

	class Context {
	public:
		Context(const Context &) = delete;
		Context(Context &&) = delete;

		Context& operator=(const Context &) = delete;
		Context& operator=(Context &&) = delete;

		static Scope<Context> Create(const Window* window);

		[[nodiscard]] OpenGLVersion GetVersion() const;

		void SwapBuffers();

		bool IsCurrent() const;

		void MakeCurrent();

	private:
		Context(void *windowHandler);

		OpenGLVersion m_Version;
		void *m_Window;
		std::thread::id m_Thread;
	};
}