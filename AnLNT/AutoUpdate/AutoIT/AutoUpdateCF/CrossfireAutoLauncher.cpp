#include "StdAfx.h"
#include "CrossfireAutoLauncher.h"

#include <tlhelp32.h>

CCrossfireAutoLauncher::CCrossfireAutoLauncher(LPCTSTR strSource)
:CLauncher(strSource)
{
	m_strLauncher = strSource + CString(L"\\Crossfire.exe");

	m_iStartButtonID = 0x000003E8;
}

CCrossfireAutoLauncher::~CCrossfireAutoLauncher(void)
{
}

CString CCrossfireAutoLauncher::GetName()
{

	return L"Crossfire";
}

BOOL CCrossfireAutoLauncher::Run()
{

	HWND hMainWnd = StartLauncherWindow(L"#32770", L"Crossfire");
	if (NULL == hMainWnd) {
		return FALSE;
	}

	if (FALSE == ClickButton(hMainWnd, m_iStartButtonID)) {
		return FALSE;
	}

	int iWaitSeconds = 5000;

	while (FALSE == WaitForComplete()) {
		::Sleep(iWaitSeconds);
	}

	return TRUE;
}

BOOL CALLBACK CCrossfireAutoLauncher::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{

	TCHAR strClassName[MAX_PATH];
	TCHAR strWindowName[MAX_PATH];

	::GetClassName(hwnd, strClassName, MAX_PATH);
	::GetWindowText(hwnd, strWindowName, MAX_PATH);

	_tprintf(L"Class name: %s. Window Name: %s\n", strClassName, strWindowName);
	return TRUE;
}

BOOL CCrossfireAutoLauncher::WaitForComplete()
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == hProcessSnap) {
		return FALSE;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if( !::Process32First( hProcessSnap, &pe32 ) ) {
		::CloseHandle( hProcessSnap );  
		return FALSE ;
	}

	do {
		if (lstrcmp(pe32.szExeFile, L"crossfire.dat") == 0) {

			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);

			if (hProcess == NULL) {
				_tprintf(L"Open process is failed. Error: %d\n", GetLastError());
			}
			
			if (FALSE == ::TerminateProcess(hProcess, 0)) {
				_tprintf(L"Termiate process is failed. Error: %d\n", GetLastError());
			} 

			::CloseHandle(hProcess);

			return TRUE;
		}

	} while (::Process32Next( hProcessSnap, &pe32 ));

	::CloseHandle( hProcessSnap );
	return FALSE;
}
