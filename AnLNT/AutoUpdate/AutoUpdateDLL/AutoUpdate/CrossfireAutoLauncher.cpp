#include "StdAfx.h"
#include "CrossfireAutoLauncher.h"
#include "CrossfireCompare.h"
#include <tlhelp32.h>
#include "MyUtils.h"

CCrossfireAutoLauncher::CCrossfireAutoLauncher(LPCTSTR strSource)
:CLauncher(strSource)
{
	m_strLauncher = strSource + CString(L"\\Crossfire.exe");

	m_iStartButtonID = 0x000003E8;
	m_iOkButtonID = 0x00000002;

	m_bIsFailed = FALSE;

}

CCrossfireAutoLauncher::~CCrossfireAutoLauncher(void)
{
}

CString CCrossfireAutoLauncher::GetName()
{

	return L"CF";
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

	HANDLE hThread = ::CreateThread(NULL, 0, CCrossfireAutoLauncher::MonitorThreadFunction, this, 0, NULL);

	if (NULL == hThread) {
		return FALSE;
	}

	::WaitForSingleObject(hThread, INFINITE);
	::CloseHandle(hThread);

	if (TRUE == m_bIsFailed) {
		return FALSE;
	}

	return TRUE;
}

BOOL CCrossfireAutoLauncher::WaitForComplete()
{

	if (FALSE == CMyUtils::KillWindowProcess(L"Crossfire", L"Crossfire"))
	{
		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI CCrossfireAutoLauncher::MonitorThreadFunction( PVOID pvParam )
{

	CCrossfireAutoLauncher* pLaucher = (CCrossfireAutoLauncher*)pvParam;

	DWORD ullStartTime = GetTickCount ();

	while(FALSE == pLaucher->WaitForComplete() && FALSE == pLaucher->m_bIsFailed) {
		EnumWindows( CCrossfireAutoLauncher::EnumWindowsProc, (LPARAM)pvParam );

		DWORD ullEndTime = GetTickCount () - ullStartTime ;
		if (ullEndTime > pLaucher->m_dwLauncherTimeOut)
		{
			pLaucher->m_bIsFailed = TRUE;

			if (FALSE == CMyUtils::KillWindowProcess(L"#32770", L"Đột Kích - Trong tầm ngắm v28"))
			{
				CMyUtils::WriteErrorLog(L"To kill CF update process is failed");
			}
			return TRUE;
		}
	}

	return TRUE;
}

BOOL CALLBACK CCrossfireAutoLauncher::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{

	const int MAX_LENGTH = 31;

	TCHAR strClassName[MAX_LENGTH];
	TCHAR strWindowName[MAX_LENGTH];

	::GetClassName(hwnd, strClassName, MAX_LENGTH);
	::GetWindowText(hwnd, strWindowName, MAX_LENGTH);

	if (lstrcmpi(strClassName, L"#32770") == 0 && lstrcmpi(strWindowName, L"Đột Kích - Trong tầm ngắm v28") == 0) {

		BOOL bIsErrorDlgFound = FALSE;

		::EnumChildWindows(hwnd, CCrossfireAutoLauncher::EnumChildWindowsProc, (LPARAM)&bIsErrorDlgFound);

		if (TRUE == bIsErrorDlgFound) {
			
			CCrossfireAutoLauncher* pLaucher = (CCrossfireAutoLauncher*)lParam;
			pLaucher->m_bIsFailed = TRUE;

			pLaucher->m_hMainWindow = hwnd;
			pLaucher->ClickButton(pLaucher->m_hMainWindow, pLaucher->m_iOkButtonID);
		}
	}

	return TRUE;
}

BOOL CALLBACK CCrossfireAutoLauncher::EnumChildWindowsProc( HWND hwnd, LPARAM lParam )
{
	
	const int MAX_LENGTH = 26;

	TCHAR strWindowName[MAX_LENGTH];
	::GetWindowText(hwnd, strWindowName, MAX_LENGTH);

	if (lstrcmpi(strWindowName, L"Không thể tải version.ini") == 0) {

		BOOL *pbIsErrorDlgFound = (BOOL *)lParam;
		*pbIsErrorDlgFound = TRUE;
	}

	return TRUE;

}

CGameSourceCompare * CCrossfireAutoLauncher::GetComparer( LPCTSTR strNewSource, LPCTSTR strOldSource )
{
	if (m_Comparer != NULL)
	{
		delete m_Comparer;
	}

	m_Comparer = new CCrossfireCompare(strNewSource, strOldSource);
	if (m_Comparer != NULL)
	{
		return m_Comparer;
	}

	return NULL;
}
