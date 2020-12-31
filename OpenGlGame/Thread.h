#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace Game
{
	class Thread
	{
	public:
		using Task = std::function<bool(size_t)>;

	private:
		const std::atomic<size_t> m_Id;

		std::thread* m_Thread;
		std::mutex m_Mutex;
		std::condition_variable m_Variable;
		std::vector<Task> m_Tasks;
		std::atomic<bool> m_Run = true;

	public:
		Thread(const size_t id);
		~Thread();
		
		size_t GetId() const { return m_Id; }

		void Join();
		bool Joinable() const;
		void Stop();
		void AddTask(Task&& task);

	private:
		void Execute();
	};
}
