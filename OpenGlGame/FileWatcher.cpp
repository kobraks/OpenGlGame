#include "pch.h"
#include "FileWatcher.h"

#include <filesystem>

#include "Exception.h"

namespace Game
{
	FileWatcher::FileWatcher()
	{
		m_Delay.store(std::chrono::milliseconds(5000));
		m_Thread = MakePointer<std::thread>(&FileWatcher::Run, this);
	}

	FileWatcher& FileWatcher::GetInstance()
	{
		static FileWatcher instance;

		return instance;
	}

	FileWatcher::~FileWatcher()
	{
		m_Run.store(false);
		m_Condition.notify_all();
		if (m_Thread->joinable())
			m_Thread->join();
	}

	void FileWatcher::AddFile(const std::string& filePath, OnChangeFunctionType onChange)
	{
		AddFile(std::filesystem::current_path() / filePath, onChange);
	}

	void FileWatcher::AddFile(const std::filesystem::path& file, OnChangeFunctionType onChange)
	{
		if (!exists(file))
			throw Exception("\"{}\" File does not exists.", file.string());

		std::string filePath = file.string();

		if (is_regular_file(file))
		{
			std::unique_lock(GetInstance().m_Mutex);
			auto& files = GetInstance().m_Files;

			if (files.contains(filePath))
				throw Exception("Already watching that file \"{}\"", filePath);

			files.emplace(std::make_pair(filePath, File{last_write_time(file), onChange}));
		}
		else
			throw Exception("\"{}\" given file is not an regular file", filePath);
	}

	void FileWatcher::AddFolder(const std::filesystem::path& path, OnChangeFunctionType onChange)
	{
		if (!exists(path))
			throw Exception("\"{}\" Folder does not exists,", path.string());

		if (is_directory(path))
		{
			for (const auto& file : path)
			{
				if (is_directory(file))
				{
					AddFolder(file, onChange);
				}
				else if (is_regular_file(file))
				{
					AddFile(file, onChange);
				}
			}
		}
		else
			throw Exception("\"{}\"path is not an directory", path.string());
	}

	void FileWatcher::AddFolder(const std::string& filePath, OnChangeFunctionType onChange)
	{
		AddFolder(std::filesystem::current_path() / filePath, onChange);
	}

	void FileWatcher::SetDelay(DelayType delay)
	{
		GetInstance().m_Delay.store(delay);
	}

	void FileWatcher::Run()
	{
		while (m_Run.load())
		{
			//std::this_thread::sleep_for(m_Delay.load());

			std::unique_lock guard(m_Mutex);
			m_Condition.wait_for(guard, m_Delay.load());

			auto it = m_Files.begin();
			bool deleted = false;
			while (it != m_Files.end())
			{
				if (!std::filesystem::exists(it->first))
				{
					it->second.OnChange(it->first, FileStatus::Erased);
					m_Files.erase(it);
					deleted = true;
				}
				else
				{
					if (!deleted)
					{
						const auto time = std::filesystem::last_write_time(it->first);
						if (it->second.Time != time)
						{
							it->second.OnChange(it->first, FileStatus::Modified);
							it->second.Time = time;
						}
					}

					++it;
					deleted = false;
				}
			}
		}
	}
}
