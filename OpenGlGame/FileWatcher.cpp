#include "pch.h"
#include "FileWatcher.h"

#include <filesystem>

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
		if(m_Thread->joinable())
			m_Thread->join();
	}

	void FileWatcher::AddFile(const std::string &filePath, OnChange onChange)
	{
		const auto path = std::filesystem::current_path() / filePath;

		if(!exists(path))
			throw std::runtime_error(fmt::format("\"{}\" Folder does not exists,", filePath));

		if(is_regular_file(path))
		{
			std::unique_lock guard(GetInstance().m_Mutex);
			auto &files = GetInstance().m_Files;

			if(files.contains(path.string()))
				throw std::runtime_error(fmt::format("Already watching that file \"{}\"", filePath));

			files.emplace(std::make_pair<std::string, File>(path.string(), {last_write_time(path), onChange}));
		}
		else
			throw std::runtime_error(fmt::format("\"{}\" given file is not an regular file", filePath));
	}

	void FileWatcher::AddFolder(const std::string &filePath, OnChange onChange)
	{
		const auto path = std::filesystem::current_path() / filePath;

		if(!exists(path))
			throw std::runtime_error(fmt::format("\"{}\" Folder does not exists,", filePath));

		if(is_directory(path))
		{
			for(auto file : path)
			{
				if(is_directory(file))
				{
					AddFolder(file.string(), onChange);
				}
				else if(is_regular_file(file))
				{
					AddFile(file.string(), onChange);
				}
			}
		}
		else
			throw std::runtime_error(fmt::format("\"{}\"path is not an directory", filePath));
	}

	void FileWatcher::SetDelay(DelayType delay)
	{
		GetInstance().m_Delay.store(delay);
	}

	void FileWatcher::Run()
	{
		while(m_Run.load())
		{
			std::this_thread::sleep_for(m_Delay.load());

			std::unique_lock guard(m_Mutex);

			auto it      = m_Files.begin();
			bool deleted = false;
			while(it != m_Files.end())
			{
				if(!std::filesystem::exists(it->first))
				{
					it->second.OnChange(it->first, FileStatus::Erased);
					m_Files.erase(it);
					deleted = true;
				}
				else
				{
					if(!deleted)
					{
						const auto time = std::filesystem::last_write_time(it->first);
						if(it->second.Time != time)
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
