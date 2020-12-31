#include "pch.h"
#include "Thread.h"

#include <queue>

namespace Game
{
	Thread::Thread(const size_t id) : m_Id(id)
	{
		m_Thread = new std::thread(&Thread::Execute, this);
	}

	Thread::~Thread()
	{
		if(Joinable())
		{
			Stop();
			Join();
		}

		delete m_Thread;
	}

	void Thread::Join()
	{
		m_Thread->join();
	}

	bool Thread::Joinable() const
	{
		return m_Thread->joinable();
	}

	void Thread::Stop()
	{
		m_Run = false;
		m_Variable.notify_all();
	}

	void Thread::AddTask(Task &&task)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);
		m_Tasks.emplace_back(std::forward<Task>(task));
		m_Variable.notify_all();
	}
	
	void Thread::Execute()
	{
		std::unique_lock<std::mutex> guard(m_Mutex);
		std::queue<size_t> deleteQueue;

		while(m_Run.load())
		{
			if(m_Tasks.empty())
				m_Variable.wait(guard);

			for (size_t i = 0; i < m_Tasks.size(); ++i)
			{
				auto task = m_Tasks[i];
				guard.unlock();
				if (task(m_Id))
					deleteQueue.push(i);
				guard.lock();
			}

			while(!deleteQueue.empty())
			{
				m_Tasks.erase(m_Tasks.begin() + deleteQueue.front());
				deleteQueue.pop();
			}
		}
	}
}
