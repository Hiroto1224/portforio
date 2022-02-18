#include <stdio.h>
#include <string>
#include <Windows.h>
#include <clocale>
#include <tchar.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <commdlg.h>
#include <crtdbg.h>
#include "../Header/FileFunction.h"
#include "../Header/StartingOver.h"

std::string FileFunction::OpenExplorer()
{
	static OPENFILENAME ofn;
	static TCHAR iniPath[MAX_PATH]; // 初期フォルダパス
	static TCHAR fileName[MAX_PATH]; // 選択したファイルの名前
	static std::string strPath;
	static std::string currentPath;
	static size_t currentPathSize;

	if(iniPath[0]==TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, iniPath);
		char path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, iniPath, -1, path, MAX_PATH, nullptr, nullptr);
		currentPath = path;
		currentPathSize = currentPath.size();
	}
	if(ofn.lStructSize == 0)
	{
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = StartingOver::Window::hWnd;
		ofn.lpstrInitialDir = iniPath;
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("ファイル選択");
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	}
	if(GetOpenFileName(&ofn))
	{
		char path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, fileName, -1, path, MAX_PATH, nullptr, nullptr);
		strPath = path;
		strPath = strPath.substr(currentPathSize + 1, strPath.size());
	}
	else
	{
		strPath = "";
	}

	return strPath;

}
