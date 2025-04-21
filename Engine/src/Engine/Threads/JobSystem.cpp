#include "pch.h"
#include "JobSystem.h"

namespace Engine {
	Scope<ThreadPool> s_ThreadPool = nullptr;
	thread_local std::optional<uint32_t> s_CurrentThreadAffinity = std::nullopt;

	void JobSystem::Init(uint32_t threadCount) {
		s_ThreadPool = MakeScope<ThreadPool>(threadCount);
	}

	void JobSystem::Shutdown() {
		s_ThreadPool = nullptr;
	}

	ThreadPool& JobSystem::Get() {
		return *s_ThreadPool;
	}

	void JobSystem::Pause() {
		s_ThreadPool->Pause();
	}

	void JobSystem::Resume() {
		s_ThreadPool->Resume();
	}

	bool JobSystem::IsPaused() {
		return s_ThreadPool->IsPaused();
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

	void JobSystem::Flush(TaskTag tag) {
		s_ThreadPool->Flush(tag);
	}

	bool JobSystem::Flush(TaskTag tag, const Time& timeout) {
		return s_ThreadPool->Flush(tag, timeout);
	}

	bool JobSystem::IsBusy() {
		return s_ThreadPool->IsBusy();
	}

	uint64_t JobSystem::CountTasksByTag(TaskTag tag) {
		return s_ThreadPool->CountTasksByTag(tag);
	}

	bool JobSystem::HasThreadAffinity() {
		return s_CurrentThreadAffinity.has_value();
	}

	std::optional<uint32_t> JobSystem::GetCurrentAffinity() {
		return s_CurrentThreadAffinity;
	}

	ScopedThreadAffinity::ScopedThreadAffinity(uint32_t threadIndex) {
		Previous = s_CurrentThreadAffinity;
		s_CurrentThreadAffinity = threadIndex;
	}

	ScopedThreadAffinity::~ScopedThreadAffinity() {
		s_CurrentThreadAffinity = Previous;
	}
}
