#pragma once
#include "Engine/Core/Base.h"

#include <functional>
#include <vector>
#include <array>
#include <mutex>
#include <concepts>
#include <iterator>
#include <utility>

namespace Engine {
	class RendererQueue {
	public:
		using Command = std::move_only_function<void()>;
		using CommandQueue = std::vector<Command>;

		template<std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
		void Enqueue(Iterator begin, Sentinel end) {
			std::scoped_lock lock(m_Mutex);

			for (; begin != end; ++begin) {
				m_WriteQueue->emplace_back(*begin);
			}
		}

		template <typename... Commands> requires (sizeof...(Commands) > 1 && (std::constructible_from<Command, Commands&&> && ...))
			void Enqueue(Commands&&... commands) {
			std::scoped_lock lock(m_Mutex);

			(m_WriteQueue->emplace_back(std::forward<Commands>(commands)), ...);
		}


		void Enqueue(Command command);

		//Must only be called by the render theard
		void Execute();

		void ClearPending();
	private:
		std::mutex m_Mutex;

		std::array<CommandQueue, 2> m_Queue;

		CommandQueue* m_DrawQueue = &m_Queue[0];
		CommandQueue* m_WriteQueue = &m_Queue[1];
	};
}