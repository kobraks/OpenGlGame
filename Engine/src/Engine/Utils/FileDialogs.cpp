#include "pch.h"
#include "Engine/Utils/FileDialogs.h"

#include "Engine/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define FILE_STRING_SIZE 260
#define DIR_STRING_SIZE 256

std::string_view GetErrorMessage(DWORD er) {
	switch (er) {
	case CDERR_DIALOGFAILURE:
		return "The dialog box could not be created. The common dialog box function's call to the DialogBox function failed. For example, this error occurs if the common dialog box call specifies an invalid window handle. ";
	case CDERR_FINDRESFAILURE:
		return "The common dialog box function failed to find a specified resource. ";
	case CDERR_INITIALIZATION:
		return "The common dialog box function failed during initialization. This error often occurs when sufficient memory is not available. ";
	case CDERR_LOADRESFAILURE:
		return "The common dialog box function failed to load a specified resource. ";
	case CDERR_LOADSTRFAILURE:
		return "The common dialog box function failed to load a specified string. ";
	case CDERR_LOCKRESFAILURE:
		return "The common dialog box function failed to lock a specified resource. ";
	case CDERR_MEMALLOCFAILURE:
		return "The common dialog box function was unable to allocate memory for internal structures. ";
	case CDERR_MEMLOCKFAILURE:
		return "The common dialog box function was unable to lock the memory associated with a handle. ";
	case CDERR_NOHINSTANCE:
		return "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding instance handle. ";
	case CDERR_NOHOOK:
		return "The ENABLEHOOK flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a pointer to a corresponding hook procedure. ";
	case CDERR_NOTEMPLATE:
		return "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding template. ";
	case CDERR_REGISTERMSGFAIL:
		return "The RegisterWindowMessage function returned an error code when it was called by the common dialog box function. ";
	case CDERR_STRUCTSIZE:
		return "The lStructSize member of the initialization structure for the corresponding common dialog box is invalid. ";
	case FNERR_BUFFERTOOSMALL:
		return "The buffer pointed to by the lpstrFile member of the OPENFILENAME structure is too small for the file name specified by the user. The first two bytes of the lpstrFile buffer contain an integer value specifying the size required to receive the full name, in characters. ";
	case FNERR_INVALIDFILENAME:
		return "A file name is invalid. ";
	case FNERR_SUBCLASSFAILURE:
		return "An attempt to subclass a list box failed because sufficient memory was not available. ";
	default:
		return "Unknown";
	}
}

void PrintError() {
	const DWORD errorCode = CommDlgExtendedError();

	if (errorCode != 0) {
		LOG_ENGINE_ERROR("Unable to open dialog box, error code: '{:#x}': '{}'", errorCode, GetErrorMessage(errorCode));
	}
}

namespace Engine {
	std::string FileDialogs::OpenFile(std::string_view filter) {
		OPENFILENAMEA ofn;
		CHAR szFile[FILE_STRING_SIZE] = { 0 };
		CHAR currentDir[DIR_STRING_SIZE] = { 0 };
		
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(Application::Get().GetWindow().GetNativeHandle<GLFWwindow>());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.data();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		
		if (GetCurrentDirectoryA(DIR_STRING_SIZE, currentDir))
			ofn.lpstrInitialDir = currentDir;
		
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		PrintError();

		return {};
	}

	std::string FileDialogs::SaveFile(const char* filter) {
		OPENFILENAMEA ofn;
		CHAR szFile[FILE_STRING_SIZE] = { 0 };
		CHAR currentDir[DIR_STRING_SIZE] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(Application::Get().GetWindow().GetNativeHandle<GLFWwindow>());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(DIR_STRING_SIZE, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		ofn.lpstrFilter = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return {};
	}


}
