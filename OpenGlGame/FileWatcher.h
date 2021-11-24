#pragma once

#include "Types.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace Game
{
	enum class FileStatus
	{
		Created,
		Modified,
		Erased
	};

	class FileWatcher
	{
	public:
		using OnChangeFunctionType = std::function<void(std::string, FileStatus)>;
		using DelayType = std::chrono::duration<int, std::milli>;

	private:
		struct File
		{
			std::filesystem::file_time_type Time;
			OnChangeFunctionType OnChange;
			bool IsFolder;
		};

		std::condition_variable m_Condition;
		std::unordered_map<std::string, File> m_Files;
		std::mutex m_Mutex;
		std::shared_ptr<std::thread> m_Thread = nullptr;

		std::atomic<DelayType> m_Delay;
		std::atomic<bool> m_Run = true;

	private:
		FileWatcher();

		static FileWatcher& GetInstance();

	public:
		~FileWatcher();

		static void Watch(const std::filesystem::path &path, OnChangeFunctionType onChange);
		static void SetDelay(DelayType delay);

	private:
		void Add(const std::filesystem::path &path, OnChangeFunctionType onChange, bool isDirectory);
		void AddFolderNoMutex(const std::filesystem::path &path, OnChangeFunctionType onChange);

		void ProcessEntry(const std::string &fileName, File &file);
		void ProcessDirectory(const std::string &directoryName, const File &file);
		void Run();
	};
}
