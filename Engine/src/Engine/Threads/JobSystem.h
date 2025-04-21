#pragma once
#include "Engine/Threads/ThreadPool.h"

namespace Engine {
	class JobSystem {
	public:
		static void Init(uint32_t threadCount = std::thread::hardware_concurrency());
		static void Shutdown();

		static ThreadPool& Get();

		static void Pause();
		static void Resume();

		static bool IsPaused();

		static void Stop();

		static void Flush();
		static bool Flush(const Time& timeout);

		static void Flush(TaskTag tag);
		static bool Flush(TaskTag tag, const Time& timeout);

		static bool IsBusy();

		static uint64_t CountTasksByTag(TaskTag tag);
		static uint64_t GetCancelledCount() { return Get().GetCancelledCount(); }

		template<typename Func, typename... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, Func&& function, Args&&... args) {
			return Get().Submit(priority, tag, affinity ? affinity : GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Func, typename... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, Func&& function, Args&&... args) {
			return Get().Submit(priority, tag, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Func, typename... Args>
		static auto Submit(TaskTag tag, Func&& function, Args&&... args) {
			return Get().Submit(TaskPriority::Normal, tag, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Func, typename... Args>
		static auto Submit(Func&& function, Args&&... args) {
			return Get().Submit(TaskPriority::Normal, TaskTag::None, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<class Func, class... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, CancellationToken token, Func&& function, Args&&... args) {
			return Get().Submit(priority, tag, affinity ? affinity : GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<class Func, class... Args>
		static auto Submit(TaskPriority priority, TaskTag tag, CancellationToken token, Func&& function, Args&&... args) {
			return Get().Submit(priority, tag, GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<class Func, class... Args>
		static auto Submit(TaskTag tag, CancellationToken token, Func&& function, Args&&... args) {
			return Get().Submit(TaskPriority::Normal, tag, GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<class Func, class... Args>
		static auto Submit(CancellationToken token, Func&& function, Args&&... args) {
			return Get().Submit(TaskPriority::Normal, TaskTag::None, GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, CancellationToken token, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, priority, tag, affinity ? affinity : GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, CancellationToken token, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, priority, tag, GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskTag tag, CancellationToken token, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, TaskPriority::Normal, tag, GetCurrentAffinity(), token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, CancellationToken token, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, TaskPriority::Normal, TaskTag::None, GetCurrentAffinity() , token, std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, std::optional<uint32_t> affinity, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, priority, tag, affinity ? affinity : GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskPriority priority, TaskTag tag, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, priority, tag, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, TaskTag tag, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, TaskPriority::Normal, tag, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		template<typename Iterator, typename Func, typename... Args>
		static void SubmitBatch(Iterator begin, Iterator end, Func&& function, Args&&... args) {
			return Get().SubmitBatch(begin, end, TaskPriority::Normal, TaskTag::None, GetCurrentAffinity(), std::forward<Func>(function), std::forward<Args>(args)...);
		}

		static bool HasThreadAffinity();
	private:
		static std::optional<uint32_t> GetCurrentAffinity();
	};

	struct ScopedJobPause {
		ScopedJobPause() { JobSystem::Pause(); }
		~ScopedJobPause() { JobSystem::Resume(); }
	};

	struct ScopedThreadAffinity {
		std::optional<uint32_t> Previous;

		ScopedThreadAffinity(uint32_t threadIndex);
		~ScopedThreadAffinity();
	};

	struct ScopedCancelable {
		CancellationSource Source;
		CancellationToken Token;

		ScopedCancelable() : Token(Source.GetToken()) {}

		void Cancel() { Source.Cancel(); }

		bool IsCancelled() const { return Token.IsCancelled(); }

		operator bool() const { return !Token.IsCancelled(); }

		~ScopedCancelable() {
			Source.Cancel();
		}
	};

}
