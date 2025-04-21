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

		void Pause();
		void Resume();

		bool IsPaused() const { return m_Paused; }

		void Stop();

		void Flush();
		bool Flush(const Time& timeout);

		void Flush(TaskTag tag);
		bool Flush(TaskTag tag, const Time& timeout);

		bool IsBusy() const;

		uint64_t CountTasksByTag(TaskTag tag) const;

		uint64_t GetCancelledCount() const { return m_TaskCancelled; }

		template<class Func, class... Args>
		auto Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> threadAffinity, Func&& function, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>;

		template<class Func, class... Args>
		auto Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> threadAffinity, CancellationToken token, Func&& function, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>;

		template<class Iterator, class Func, class... Args>
		void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, std::optional<uint32_t> threadAffinity, Func&& function, Args&&... args);

		template<class Iterator, class Func, class... Args>
		void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, std::optional<uint32_t> threadAffinity, CancellationToken token, Func&& function, Args&&... args);
	private:
		bool HasPendigTag(TaskTag tag) const;

		static void Enqueue(TaskQueue& queue, Task&& task);
		static Task Dequeue(TaskQueue& queue);

		static std::pair<Task, bool> FindTask(TaskQueue& queue);

		void WorkerLoop(uint32_t threadIndex);

		std::vector<std::thread> m_Threads;
		std::vector<TaskQueue> m_PerThreadQueue;
		std::unordered_map<TaskTag, uint32_t> m_InFlightTag;

		TaskQueue m_GlobalTaskQueue;

		mutable std::mutex m_Mutex;
		mutable std::mutex m_InFlightMutex;

		std::condition_variable m_Condition;
		std::condition_variable m_PauseCondition;

		std::atomic<bool> m_Run = true;
		std::atomic<bool> m_Paused = false;

		std::atomic<uint32_t> m_TasksInFlight = 0;
		std::atomic<uint64_t> m_TaskCancelled = 0;
	};

	template <class Func, class ... Args>
	auto ThreadPool::Submit(TaskPriority priority, TaskTag tag,
		std::optional<uint32_t> threadAffinity, Func&& function,
		Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		return Submit(priority, tag, threadAffinity, CancellationToken{}, std::forward<Func>(function), std::forward<Args>(args)...);
		
	}

	template <class Func, class ... Args>
	auto ThreadPool::Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> threadAffinity,
		CancellationToken token, Func&& function, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		using ReturnType = std::invoke_result_t<Func, Args...>;

		auto taskPtr = MakeRef<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));

		Task task;
		task.Job = [taskPtr]() { (*taskPtr)(); };
		task.Priority = priority;
		task.Tag = tag;
		task.ThreadAffinity = threadAffinity;
		task.Token = token;

		{
			std::lock_guard lock(m_Mutex);

			if (threadAffinity) {
				if (threadAffinity < m_PerThreadQueue.size()) {
					Enqueue(m_PerThreadQueue[threadAffinity.value()], std::move(task));
				}
				else {
					LOG_WARN("Invalid thread affinity! Falling back to global queue.");
					Enqueue(m_GlobalTaskQueue, std::move(task));
				}
			}
			else {
				Enqueue(m_GlobalTaskQueue, std::move(task));
			}
		}

		if (!m_Paused) {
			m_Condition.notify_all();
		}

		return taskPtr->get_future();

	}

	template <class Iterator, class Func, class ... Args>
	void ThreadPool::SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag,
		std::optional<uint32_t> threadAffinity, Func&& function, Args&&... args) {
		return SubmitBatch(begin, end, priority, tag, threadAffinity, CancellationToken{}, std::forward<Func>(function), std::forward<Args>(args)...);
	}

	template <class Iterator, class Func, class ... Args>
	void ThreadPool::SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag,
		std::optional<uint32_t> threadAffinity, CancellationToken token, Func&& function, Args&&... args) {
		std::lock_guard lock(m_Mutex);

		for (auto it = begin; it != end; ++it) {
			Task task;

			task.Priority = priority;
			task.Tag = tag;
			task.ThreadAffinity = threadAffinity;
			task.Token = token;

			auto item = *it;
			task.Job = [item, fn = std::forward<Func>(function), ...args = std::forward<Args>(args)]() mutable {
				fn(item, args);
				};

			if (threadAffinity) {
				if (*threadAffinity < m_PerThreadQueue.size()) {
					Enqueue(m_PerThreadQueue[*threadAffinity], std::move(task));
				}
				else {
					LOG_WARN("Invalid thread affinity! Falling back to global queue.");
					Enqueue(m_GlobalTaskQueue, std::move(task));
				}
			}
			else {
				Enqueue(m_GlobalTaskQueue, std::move(task));
			}
		}

		if (!m_Paused) {
			m_Condition.notify_all();
		}

	}
}
