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

		static bool IsBusy();

		static uint64_t CountTasksByTag(TaskTag tag);

		template<typename Func, typename... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, const Time& delay, std::optional<uint32_t> affinity, Func&& function, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>;
	};

	template <typename Func, typename ... Args>
	auto JobSystem::Submit(TaskPriority priority, TaskTag tag, const Time& delay, std::optional<uint32_t> affinity, Func&& function,
		Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
		return Get().Submit(priority, tag, delay, affinity, std::forward<Func>(function), std::forward<Args>(args)...);
	}
}
