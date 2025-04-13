#include "pch.h"
#include "ThreadPool.h"

#include <algorithm>

namespace Engine {
	ThreadPool::ThreadPool(uint32_t threadCount) {
		m_PerThreadQueue.resize(threadCount);
		m_Threads.reserve(threadCount);

		for (uint32_t i = 0; i < threadCount; ++i) {
			m_Threads.emplace_back([this, i]() { WorkerLoop(i); });
		}
	}

	ThreadPool::~ThreadPool() {
		Flush();
		Stop();
	}

	void ThreadPool::Stop() {
		m_Run = false;

		m_Condition.notify_all();

		for (auto& thread : m_Threads) {
			if (thread.joinable())
				thread.join();
		}
	}

	void ThreadPool::Flush() {
		std::unique_lock lock(m_Mutex);

		m_Condition.wait(lock, [&]() {
			const bool globalEmpty = m_GlobalTaskQueue.empty();
			const bool inFlightDone = (m_TasksInFlight == 0);
			const bool perThreadEmpty = std::ranges::all_of(m_PerThreadQueue, [](const TaskQueue& queue) {
				return queue.empty();
				});

			return globalEmpty && inFlightDone && perThreadEmpty;
			});
	}

	bool ThreadPool::Flush(const Time& timeout) {
		std::unique_lock lock(m_Mutex);

		return m_Condition.wait_for(lock, timeout.ToDuration(), [&]() {
			const bool globalEmpty = m_GlobalTaskQueue.empty();
			const bool inFlightDone = (m_TasksInFlight == 0);
			const bool perThreadEmpty = std::ranges::all_of(m_PerThreadQueue, [](const TaskQueue& queue) {
				return queue.empty();
				});

			return globalEmpty && inFlightDone && perThreadEmpty;
			});
	}

	void ThreadPool::Flush(TaskTag tag) {
		std::unique_lock lock(m_Mutex);

		m_Condition.wait(lock, [&]() {
			std::lock_guard guard(m_InFlightMutex);
			return !HasPendigTag(tag) && m_InFlightTag[tag] == 0;
			});
	}

	bool ThreadPool::Flush(TaskTag tag, const Time& timeout) {
		std::unique_lock lock(m_Mutex);

		return m_Condition.wait_for(lock, timeout.ToDuration(), [&]() {
			std::lock_guard guard(m_InFlightMutex);
			return !HasPendigTag(tag) && m_InFlightTag[tag] == 0;
			});
	}

	bool ThreadPool::IsBusy() const {
		std::unique_lock lock(m_Mutex);

		if (!m_GlobalTaskQueue.empty() || m_TasksInFlight > 0)
			return true;

		return std::ranges::any_of(m_PerThreadQueue, [](const TaskQueue& queue) { return !queue.empty(); });
	}

	uint32_t ThreadPool::CountTasksByTag(TaskTag tag) const {
		std::lock_guard lock(m_Mutex);

		const auto tagPre = [tag](const Task& task) { return task.Tag == tag; };
		const auto countQueue = [tagPre](const TaskQueue& queue) { return std::ranges::count_if(queue, tagPre); };

		uint64_t count = countQueue(m_GlobalTaskQueue);;

		for (const auto& queue : m_PerThreadQueue) {
			count += countQueue(queue);
		}

		return static_cast<uint32_t>(count);
	}

	bool ThreadPool::HasPendigTag(TaskTag tag) const {
		auto tagMatch = [tag](const Task& task) {
			return task.Tag == tag;
			};

		if (std::ranges::any_of(m_GlobalTaskQueue, tagMatch))
			return true;

		for (const auto& queue : m_PerThreadQueue) {
			if (std::ranges::any_of(queue, tagMatch))
				return true;
		}

		return false;
	}

	void ThreadPool::Enqueue(TaskQueue& queue, Task&& task) {
		queue.emplace_back(std::forward<Task>(task));
		std::ranges::push_heap(queue, std::greater<Task>{});
	}

	Task ThreadPool::Dequeue(TaskQueue& queue) {
		std::ranges::pop_heap(queue, std::greater<Task>{});
		Task task = std::move(queue.back());
		queue.pop_back();
		return task;

	}

	std::pair<Task, bool> ThreadPool::FindTask(TaskQueue& queue) {
		Task task;
		bool found = false;

		if (!queue.empty()) {
			task = std::move(Dequeue(queue));
			found = true;
		}

		return std::make_pair(std::move(task), found);
	}

	void ThreadPool::WorkerLoop(uint32_t threadIndex) {
		auto& queue = m_PerThreadQueue[threadIndex];
		while (m_Run) {
			Task task;
			bool found = false;

			{
				std::unique_lock lock(m_Mutex);

				m_Condition.wait(lock, [&]() { return !m_Run || !queue.empty() || !m_GlobalTaskQueue.empty(); });

				if (!m_Run) break;

				std::tie(task, found) = FindTask(queue);
				if (!found)
					std::tie(task, found) = FindTask(m_GlobalTaskQueue);
			}

			if (found) {
				{
					std::lock_guard lock(m_InFlightMutex);
					m_InFlightTag[task.Tag]++;
				}

				++m_TasksInFlight;
				task.Job();

				--m_TasksInFlight;

				{
					std::lock_guard lock(m_InFlightMutex);
					m_InFlightTag[task.Tag]--;
				}

				m_Condition.notify_all();
			}
		}
	}
}
