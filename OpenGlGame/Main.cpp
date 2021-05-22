#include "pch.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>

#include "Application.h"
#include "GameLayer.h"

using MINIDUMPWRITEDUMP = BOOL(WINAPI *)(
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

void CreateMinidump(struct _EXCEPTION_POINTERS *exception)
{
	//SORUCE: https://stackoverflow.com/questions/9020353/create-a-dump-file-for-an-application-whenever-it-crashes
	
	//Load liblary
	HMODULE lib = LoadLibrary(_T("dbghelp.dll"));
	//GetFunctionAddres
	MINIDUMPWRITEDUMP dump = (MINIDUMPWRITEDUMP)GetProcAddress(lib, "MiniDumpWriteDump");

	//OpenFile
	HANDLE file = CreateFile(_T("DMP_NAME"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//Create information struct
	_MINIDUMP_EXCEPTION_INFORMATION exInfo;
	exInfo.ThreadId = GetCurrentThreadId();
	exInfo.ExceptionPointers = exception;
	exInfo.ClientPointers = FALSE;

	//Create dmp file
	dump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &exInfo, NULL, NULL);
	CloseHandle(file);
}

LONG WINAPI HandleException(struct _EXCEPTION_POINTERS *exception)
{
	CreateMinidump(exception);
	return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char **argv)
{
	SetUnhandledExceptionFilter(HandleException);
	
	Game::Application game;
	game.ProcessArgs(argc, argv);

	int32_t exitCode = 0;

	try
	{
		game.Initialize();
		game.PushLayer(Game::MakePointer<Game::GameLayer>());
		exitCode = game.Run();
	}
	catch(std::exception &ex)
	{
		LOG_CRITICAL("Uncatched exception: {}", ex.what());
		system("pause");
		exitCode = EXIT_FAILURE;
	}

	LOG_INFO("Exited with code: {}", exitCode);

	return exitCode;
}
