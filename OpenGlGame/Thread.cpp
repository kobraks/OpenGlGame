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
		Shutdown();

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
		m_Condition.notify_one();
	}

	void Thread::AddTask(Task &&task)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);
		m_Tasks.emplace(std::forward<Task>(task));
		m_Condition.notify_one();
	}

	void Thread::Shutdown()
	{
		if(Joinable())
		{
			Stop();
			Join();
		}
	}

	void Thread::Execute()
	{
		while(m_Run.load())
		{
			Task task;
			bool result = true;

			{
				std::unique_lock<std::mutex> guard(m_Mutex);

				m_Condition.wait(guard, [this] { return !m_Tasks.empty() || !m_Run.load(); });
				if (m_Tasks.empty())
					continue;
				
				if (!result) m_Tasks.pop();
				task = m_Tasks.front();
			}

			result = task();
		}
	}
}
