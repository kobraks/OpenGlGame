#include "pch.h"
#include "ThreadPool.h"

#include <algorithm>

namespace Game
{
	ThreadPool::ThreadPool(const size_t threadCount) : m_ThreadCount(threadCount)
	{
		m_Threads.resize(threadCount);

		std::fill(m_Threads.begin(), m_Threads.end(), nullptr);
	}

	ThreadPool::~ThreadPool()
	{
		for(auto &thread : m_Threads) delete thread;
	}

	void ThreadPool::AddTask(Thread::Task &&task)
	{
		if(!m_Threads[m_CurrentAssigment]) AddThread();

		m_Threads[m_CurrentAssigment]->AddTask(std::forward<Thread::Task>(task));
		m_CurrentAssigment = (m_CurrentAssigment + 1) % m_ThreadCount;
	}

	void ThreadPool::AddThread()
	{
		m_Threads[m_CurrentAssigment] = new Thread(m_CurrentAssigment);
	}
}
