#pragma once
#include <vector>

#include "Thread.h"
#include "Types.h"

namespace Game
{
	class ThreadPool
	{
		std::vector<Scope<Thread>> m_Threads;

		const size_t m_ThreadCount;
		size_t m_CurrentAssigment = 0;

		static ThreadPool* s_Instance;
	
	public:
		explicit ThreadPool(const size_t threadCount);
		~ThreadPool();

		void IssueTask(Thread::Task&& task);

		static ThreadPool& GetInstance();
	};
}
