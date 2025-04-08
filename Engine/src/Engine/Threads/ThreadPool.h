#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Threads/Task.h"

#include <future>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Engine {
	class ThreadPool {
	public:
		using TaskQueue = std::vector<Task>;

		ThreadPool(uint32_t threadCount = std::thread::hardware_concurrency());
		~ThreadPool();

		template<class F, class... Args>
		auto Submit(TaskPriority priority, TaskTag tag, const Time& delay, std::optional<uint32_t> threadAffinity, F&& function, Args&& ... args) -> std::future<std::invoke_result_t<F, Args...>>;

		void Stop();

		void Flush();
		bool Flush(const Time& timeout);

		bool IsBusy() const;

		uint64_t CountTasksByTag(TaskTag tag) const;

	private:
		static void Enqueue(TaskQueue& queue, Task&& task);

		static std::pair<Task, bool> FindTask(TaskQueue& queue);
		static void SortQueue(std::vector<Task>& queue);

		void WorkerLoop(uint32_t threadIndex);

		std::vector<std::thread> m_Threads;
		std::vector<TaskQueue> m_PerThreadQueue;

		TaskQueue m_GlobalTaskQueue;

		mutable std::mutex m_Mutex;
		std::condition_variable m_Condition;

		std::atomic<bool> m_Run = true;
		std::atomic<uint64_t> m_TasksInFlight = 0;

	};

	template <class Func, class ... Args>
	auto ThreadPool::Submit(TaskPriority priority, TaskTag tag, const Time& delay,
		std::optional<uint32_t> threadAffinity, Func&& function,
		Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		using ReturnType = std::invoke_result_t<Func, Args...>;

		auto taskPtr = MakeRef<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));

		Task task;
		task.Job = [taskPtr]() { (*taskPtr)(); };
		task.Priority = priority;
		task.Tag = tag;
		task.ThreadAffinity = threadAffinity;
		task.ReadyTime = delay;

		{
			std::lock_guard lock(m_Mutex);

			if (threadAffinity) {
				if (threadAffinity < m_PerThreadQueue.size()) {
					Enqueue(m_PerThreadQueue[threadAffinity.value()], std::move(task));
				} else {
					LOG_WARN("Invalid thread affinity! Falling back to global queue.");
					Enqueue(m_GlobalTaskQueue, std::move(task));
				}
			} else {
				Enqueue(m_GlobalTaskQueue, std::move(task));
			}
		}

		m_Condition.notify_all();
		return taskPtr->get_future();
	}
}
