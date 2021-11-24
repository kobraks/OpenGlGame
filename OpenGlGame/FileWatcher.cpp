#include "pch.h"
#include "FileWatcher.h"

#include <filesystem>
#include <queue>

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
		if(m_Thread->joinable())
			m_Thread->join();
	}

	void FileWatcher::Watch(const std::filesystem::path &path, OnChangeFunctionType onChange)
	{
		const std::string sPath = path.string();

		if(!exists(path))
			throw Exception("\"{}\", Does not exists.", sPath);

		std::unique_lock guard(GetInstance().m_Mutex);
		if(is_regular_file(path))
			GetInstance().Add(path, onChange, false);
		else if(is_directory(path))
			GetInstance().AddFolderNoMutex(path, onChange);
		else
			throw Exception("Unknown file type \"{}\"", sPath);
	}

	void FileWatcher::SetDelay(DelayType delay)
	{
		GetInstance().m_Delay.store(delay);
	}

	void FileWatcher::Add(const std::filesystem::path &path, OnChangeFunctionType onChange, bool isDirectory)
	{
		std::string sPath = path.string();

		if(m_Files.contains(sPath))
			return;

		m_Files.emplace(std::make_pair(sPath, File{last_write_time(path), onChange, isDirectory}));
	}

	void FileWatcher::AddFolderNoMutex(const std::filesystem::path &path, OnChangeFunctionType onChange)
	{
		Add(path, onChange, true);

		for(const auto &file : path)
		{
			if(is_directory(file))
				AddFolderNoMutex(file, onChange);
			else
				Add(file, onChange, false);
		}
	}

	void FileWatcher::ProcessEntry(const std::string &fileName, File &file)
	{
		try
		{
			if(file.IsFolder)
				ProcessDirectory(fileName, file);
			else
			{
				const auto time = std::filesystem::last_write_time(fileName);
				if(file.Time != time)
				{
					file.OnChange(fileName, FileStatus::Modified);
					file.Time = time;
				}
			}
		}
		catch(...) {}
	}

	void FileWatcher::ProcessDirectory(const std::string &directoryName, const File &file)
	{
		std::filesystem::path path = directoryName;

		for(const auto f : path)
		{
			if(!m_Files.contains(f.string()))
			{
				file.OnChange(f.string(), FileStatus::Created);
				m_Files.emplace(std::make_pair(f.string(), File{last_write_time(f), file.OnChange, is_directory(f)}));
			}
		}
	}

	void FileWatcher::Run()
	{
		while(m_Run.load())
		{
			std::unique_lock guard(m_Mutex);
			m_Condition.wait_for(guard, m_Delay.load());
			if(!m_Run.load())
				return;

			auto it = m_Files.begin();
			std::queue<std::string> deletionQueue;

			while(it != m_Files.end())
			{
				if(!std::filesystem::exists(it->first))
				{
					it->second.OnChange(it->first, FileStatus::Erased);
					deletionQueue.emplace(it->first);
				}
				else
					ProcessEntry(it->first, it->second);
			}
		}
	}
}
