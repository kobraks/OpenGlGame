#include "pch.h"
#include "RendererQueue.h"

namespace Engine {
	RendererQueue::RendererQueue() {
		m_Queue[0] = MakeRef<CommandQueue>();
		m_Queue[1] = MakeRef<CommandQueue>();

		m_WriteQueue = m_Queue[0];
		m_ReadQueue = m_Queue[1];
	}

	void RendererQueue::Submit(std::function<void()> command) {
		std::scoped_lock lock(m_Mutex);

		m_WriteQueue->emplace_back(std::move(command));
	}

	void RendererQueue::Execute() {
		SwapBuffers();

		auto& commands = *m_ReadQueue;
		for (auto& command : commands) {
			command();
		}

		commands.clear();
	}

	void RendererQueue::SwapBuffers() {
		std::scoped_lock lock(m_Mutex);
		std::swap(m_ReadQueue, m_WriteQueue);
	}
}