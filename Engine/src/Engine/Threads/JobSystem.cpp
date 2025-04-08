#include "pch.h"
#include "JobSystem.h"

namespace Engine {
	Scope<ThreadPool> s_ThreadPool;

	void JobSystem::Init(uint32_t threadCount) {
		s_ThreadPool = MakeScope<ThreadPool>(threadCount);
	}

	void JobSystem::Shutdown() {
		s_ThreadPool = nullptr;
	}

	ThreadPool& JobSystem::Get() {
		return *s_ThreadPool;
	}

	void JobSystem::Stop() {
		s_ThreadPool->Stop();
	}

	void JobSystem::Flush() {
		s_ThreadPool->Flush();
	}

	bool JobSystem::Flush(const Time& timeout) {
		return s_ThreadPool->Flush(timeout);
	}

	bool JobSystem::IsBusy() {
		return s_ThreadPool->IsBusy();
	}

	uint64_t JobSystem::CountTasksByTag(TaskTag tag) {
		return s_ThreadPool->CountTasksByTag(tag);
	}
}
