#pragma once
#include <vector>

#include "Thread.h"

namespace Game
{
	class ThreadPool
	{
		std::vector<Thread*> m_Threads;

		const size_t m_ThreadCount;
		size_t m_CurrentAssigment = 0;

	public:
		explicit ThreadPool(const size_t threadCount);
		~ThreadPool();

		void AddTask(Thread::Task&& task);

	private:
		void AddThread();
	};
}
