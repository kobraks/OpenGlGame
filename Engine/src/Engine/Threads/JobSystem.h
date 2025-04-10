#pragma once
#include "Engine/Threads/ThreadPool.h"

namespace Engine {
	class JobSystem {
	public:
		static void Init(uint32_t threadCount = std::thread::hardware_concurrency());
		static void Shutdown();

		static ThreadPool& Get();

		static void Stop();

		static void Flush();
		static bool Flush(const Time& timeout);

		static void Flush(TaskTag tag);
		static bool Flush(TaskTag tag, const Time& timeout);

		static bool IsBusy();

		static uint64_t CountTasksByTag(TaskTag tag);

		template<typename Func, typename... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, Func&& function, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>;

		template<typename Iternator, typename Func, typename... Args>
		static auto SubmitBatch(Iternator begin, Iternator end, TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, Func&& function, Args&&... args);
	};

	template <typename Func, typename ... Args>
	auto JobSystem::Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, Func&& function,
		Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		return Get().Submit(priority, tag, affinity, std::forward<Func>(function), std::forward<Args>(args)...);
	}

	template <typename Iternator, typename Func, typename ... Args>
	auto JobSystem::SubmitBatch(Iternator begin, Iternator end, TaskPriority priority, TaskTag tag,
		std::optional<uint32_t> affinity, Func&& function, Args&&... args) {
		return Get().SubmitBatch(begin, end, priority, tag, affinity, std::forward<Func>(function), std::forward<Args>(args)...);
	}
}
