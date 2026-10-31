#include "pch.h"
#include "RendererQueue.h"

namespace Engine {
	void RendererQueue::Enqueue(Command command) {
		std::scoped_lock lock(m_Mutex);

		m_WriteQueue->emplace_back(std::move(command));
	}

	void RendererQueue::Execute() {
		{
			std::scoped_lock lock(m_Mutex);
			std::swap(m_WriteQueue, m_DrawQueue);
		}

		auto& commands = *m_DrawQueue;

		try {
			for (auto& command : commands) {
				command();
			}
		}
		catch (...) {
			commands.clear();
			throw;
		}
		

		commands.clear();
	}

	void RendererQueue::ClearPending() {
		std::scoped_lock lock(m_Mutex);
		m_WriteQueue->clear();
	}
}