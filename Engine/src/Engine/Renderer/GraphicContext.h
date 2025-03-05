#pragma once

#include <Engine/Core/Base.h>

namespace Engine {
	class Window;

	struct OpenGLVersion {
		int32_t Major = 0;
		int32_t Minor = 0;
	};

	class GraphicContext {
	public:
		GraphicContext(const GraphicContext &) = delete;
		GraphicContext(GraphicContext&&) = delete;

		GraphicContext& operator=(const GraphicContext &) = delete;
		GraphicContext& operator=(GraphicContext &&) = delete;

		static Scope<GraphicContext> Create(const Window* window);

		[[nodiscard]] OpenGLVersion GetVersion() const { return m_Version; }

		void SwapBuffers();

		bool IsCurrent() const;

		void MakeCurrent();
		void Detach();

	private:
		GraphicContext(void *windowHandler);

		OpenGLVersion m_Version;
		void *m_Window = nullptr;
		std::thread::id m_Thread;
	};
}