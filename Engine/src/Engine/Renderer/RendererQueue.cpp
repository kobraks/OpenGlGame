#include "pch.h"
#include "RendererQueue.h"

namespace Engine {
	RendererQueue::RendererQueue() {
		m_Queue[0] = MakeScope<CommandQueue>();
		m_Queue[1] = MakeScope<CommandQueue>();

		m_WriteQueue = m_Queue[0].get();
		m_DrawQueue = m_Queue[1].get();
	}

	void RendererQueue::Enqueue(std::function<void()> command) {
		std::scoped_lock lock(m_Mutex);

		m_WriteQueue->emplace_back(std::move(command));
	}

	void RendererQueue::Execute() {
		SwapBuffers();

		auto& commands = *m_DrawQueue;
		for (auto& command : commands) {
			command();
		}

		commands.clear();
	}

	void RendererQueue::SwapBuffers() {
		std::scoped_lock lock(m_Mutex);
		std::swap(m_DrawQueue, m_WriteQueue);
	}

	void RendererQueue::Clear() {
		std::scoped_lock lock(m_Mutex);
		m_WriteQueue->clear();
	}
}