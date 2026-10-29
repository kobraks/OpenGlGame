#pragma once
#include "Engine/Core/Base.h"

#include <functional>
#include <vector>
#include <mutex>

namespace Engine {
	class RendererQueue {
	public:
		using CommandQueue = std::vector<std::function<void()>>;
		RendererQueue();

		template<typename Iterator>
		void Enqueue(Iterator begin, Iterator end) {
			std::scoped_lock lock(m_Mutex);

			for (auto i = begin; i != end; ++i) {
				m_WriteQueue->emplace_back(*i);
			}
		}

		template<typename ...Args>
		void Enqueue(std::function<void()> command, Args&& ... args) {
			Enqueue(command);
			Enqueue(std::forward<Args>(args)...);
		}

		void Enqueue(std::function<void()> command);
		void Execute();

		void Clear();
	private:
		void SwapBuffers();

		std::mutex m_Mutex;

		Scope<CommandQueue> m_Queue[2];

		CommandQueue* m_DrawQueue = nullptr;
		CommandQueue* m_WriteQueue = nullptr;
	};
}