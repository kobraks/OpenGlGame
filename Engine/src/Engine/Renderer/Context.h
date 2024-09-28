#pragma once

#include <Engine/Core/Base.h>

namespace Engine {
	class Window;

	struct OpenGLVersion {
		int32_t Major = 0;
		int32_t Minor = 0;
	};

	class Context {
	public:
		Context(const Context &) = delete;
		Context(Context &&) = delete;

		Context& operator=(const Context &) = delete;
		Context& operator=(Context &&) = delete;

		static Scope<Context> Create(const Window* window);

		[[nodiscard]] OpenGLVersion GetVersion() const { return m_Version; }

		void SwapBuffers();

		bool IsCurrent() const;

		void MakeCurrent();
		void Detach();

	private:
		Context(void *windowHandler);

		OpenGLVersion m_Version;
		void *m_Window = nullptr;
		std::thread::id m_Thread;
	};
}