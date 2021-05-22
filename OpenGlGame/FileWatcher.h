#pragma once

#include "Types.h"

#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

namespace Game
{
	enum class FileStatus
	{
		// Created,
		Modified,
		Erased
	};

	class FileWatcher
	{
	public:
		using OnChange = std::function<void(std::string, FileStatus)>;
		using DelayType = std::chrono::duration<int, std::milli>;
	
	private:
		struct File
		{
			std::filesystem::file_time_type Time;
			OnChange OnChange;
		};

		std::unordered_map<std::string, File> m_Files;
		std::mutex m_Mutex;
		Pointer<std::thread> m_Thread = nullptr;
		std::atomic<DelayType> m_Delay;
		std::atomic<bool> m_Run = true;
	
	private:
		FileWatcher();

		static FileWatcher& GetInstance();
	public:
		~FileWatcher();

		static void AddFile(const std::string &filePath, OnChange onChange);
		static void AddFolder(const std::string &filePath, OnChange onChange);

		static void SetDelay(DelayType delay);

	private:
		//Thread method
		void Run();
	};
}
