#include "pch.h"
#include "ThreadPool.h"

#include <algorithm>

namespace Game
{
	ThreadPool::ThreadPool(const size_t threadCount) : m_ThreadCount(threadCount)
	{
		m_Threads.resize(threadCount);

		uint32_t id = 0; 
		std::generate(m_Threads.begin(), m_Threads.end(), [&id]{return MakeScope<Thread>(id++);});
	}

	ThreadPool::~ThreadPool()
	{
		for(auto &thread : m_Threads) thread->Shutdown();
	}

	void ThreadPool::IssueTask(Thread::Task &&task)
	{
		m_Threads[m_CurrentAssigment]->AddTask(std::forward<Thread::Task>(task));
		m_CurrentAssigment = (m_CurrentAssigment + 1) % m_ThreadCount;
	}
}
