#pragma once

#include <Engine/Core/Base.h>

#include <atomic>

namespace Engine {
	class Window;

	struct GraphicInfo {
		std::string Vendor;
		std::string Renderer;
		std::string Version;
	};

	struct OpenGLVersion {
		int32_t Major = 0;
		int32_t Minor = 0;
	};

	class GraphicContext {
	public:
		~GraphicContext();

		GraphicContext(const GraphicContext &) = delete;
		GraphicContext(GraphicContext&&) = delete;

		GraphicContext& operator=(const GraphicContext &) = delete;
		GraphicContext& operator=(GraphicContext &&) = delete;

		static Scope<GraphicContext> Create(const Window* window);
		static Scope<GraphicContext> CreateShared(const GraphicContext* sharedWith);

		[[nodiscard]] OpenGLVersion GetVersion() const { return m_Version; }
		[[nodiscard]] const GraphicInfo& GetInfo() const { return m_Info; }

		void SwapBuffers(); //NO-OP for shared contexts

		bool IsCurrent() const;
		bool IsValid() const;

		void MakeCurrent();
		void Detach();

		bool IsShared() const { return m_MainWindowHandle != nullptr; }
		bool OwnsWindow() const { return m_OwnsWindow; }

		bool IsThreadBound() const;
	private:
		GraphicContext(void *windowHandle);
		void Init(void* windowHandle);

		OpenGLVersion m_Version;
		GraphicInfo m_Info;

		bool m_OwnsWindow = false;

		void *m_WindowHandle = nullptr;
		void *m_MainWindowHandle = nullptr;

		std::thread::id m_Thread{};

		mutable std::atomic_flag m_ContextBusy = ATOMIC_FLAG_INIT;
	};
}