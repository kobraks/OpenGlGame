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

		void Submit(std::function<void()> command);
		void Execute();
	private:
		void SwapBuffers();

		std::mutex m_Mutex;

		Ref<CommandQueue> m_Queue[2];

		Ref<CommandQueue> m_ReadQueue = nullptr;
		Ref<CommandQueue> m_WriteQueue = nullptr;
	};
}