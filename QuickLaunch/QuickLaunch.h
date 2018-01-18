#pragma once

#include "resource.h"

#include <CommCtrl.h>
#include <Shobjidl.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <string>
#include <vector>
#include <fstream>
#include <fcntl.h> //_O_WTEXT
#include <io.h>    //_setmode()
#include <locale>
#include <codecvt> //possible C++11?
#include <algorithm>

#pragma comment (lib,"Shell32.lib")
#pragma comment (lib,"Shlwapi.lib")

using namespace std;

#define PRO_FILE L"C:\\Program Files"
#define PRO_FILE_X86 L"C:\\Program Files (x86)"
#define MAX_FILE_NAME 260
#define MAX_PATH 10240

struct PROGRAM_INFO {
	unsigned short priority;
	WCHAR szName[MAX_FILE_NAME];
	WCHAR szPath[MAX_PATH];
};

vector<PROGRAM_INFO> ListApp;
bool state = true;


#define MAX_PATH_LEN 1000
void LoadData(LPCWSTR path)
{
	BOOL isExist;
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path);
	StrCat(buffer, _T("\\*"));

	WIN32_FIND_DATA fd;
	BOOL bFound = TRUE;
	HANDLE hFile;
	TCHAR *filePath, *folderPath;

	hFile = FindFirstFileW(buffer, &fd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		bFound = FALSE;
		return;
	}
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
		{
			if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) && ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM))
			{
				int len = wcslen(fd.cFileName);
				if (fd.cFileName[len - 4] == L'.' && fd.cFileName[len - 3] == L'e' && fd.cFileName[len - 2] == L'x' && fd.cFileName[len - 1] == L'e')
				{
					filePath = new TCHAR[wcslen(path) + len + 2];
					StrCpy(filePath, path);
					StrCat(filePath, _T("\\"));
					StrCat(filePath, fd.cFileName);


					PROGRAM_INFO newFile;
					StrNCpy(newFile.szName, fd.cFileName, len - 3);
					StrCpy(newFile.szPath, filePath);

					ListApp.push_back(newFile);

					delete[] filePath;
				}
			}
			else if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
			{
				folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
				StrCpy(folderPath, path);
				StrCat(folderPath, _T("\\"));
				StrCat(folderPath, fd.cFileName);
				LoadData(folderPath);

				delete[] folderPath;
			}
		}
		bFound = FindNextFileW(hFile, &fd);
	}
}

void saveData()
{
	HANDLE hFile;
	DWORD dwBytesWritten;

	TCHAR buffer[MAX_PATH];
	hFile = CreateFile(L"Database.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		for (int i = 0; i < ListApp.size(); i++) {
			StrCpy(buffer, ListApp[i].szName);
			StrCat(buffer, L"\t");
			StrCat(buffer, ListApp[i].szPath);
			StrCat(buffer, L"\n");
			WriteFile(hFile, buffer, wcslen(buffer) * sizeof(TCHAR), &dwBytesWritten, NULL);
		}

		CloseHandle(hFile);
	}
}

bool compareFirstLetter(const PROGRAM_INFO &a, const PROGRAM_INFO &b)
{
	return a.szName[0] < b.szName[0];
}

bool stringCompareBeginLetters(TCHAR *buff, int len, TCHAR *name) 
{
	int i = 0;
	while(i < len) {
		if (i == len)
			return true;

		if (buff[i] != name[i])
			return false;
		i++;
	}
}