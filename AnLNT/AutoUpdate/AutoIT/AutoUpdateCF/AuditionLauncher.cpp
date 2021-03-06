#include "StdAfx.h"
#include "AuditionLauncher.h"

CAuditionLauncher::CAuditionLauncher(LPCTSTR strSourcePath, LPCTSTR strLauncherPath)
: CCrossfireLauncher(strSourcePath, strLauncherPath)
{
	m_isMonitoring = TRUE;
}

CAuditionLauncher::~CAuditionLauncher(void)
{
}

BOOL CAuditionLauncher::Run()
{
	m_hMainWnd = StartLauncherWindow(L"TWizardForm", L"Setup - Audition");

	if (NULL == m_hMainWnd) {
		return FALSE;
	}

	if (FALSE == PressButton(m_hMainWnd, L"&Tiếp theo >")) {
		return FALSE;
	}

	if (FALSE == SetSourcePath(m_hMainWnd)) {
		return FALSE;
	}

	HANDLE hThread = ::CreateThread(NULL, 0, CAuditionLauncher::MonitorThreadFunction, this, 0, NULL);

	if (NULL == hThread) {
		return FALSE;
	}

	if (FALSE == PressButton(m_hMainWnd, L"&Cài đặt")) {
		return FALSE;
	}

	m_isMonitoring = FALSE;
	::WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode;
	if (FALSE == ::GetExitCodeThread(hThread, &dwExitCode)) {
		return FALSE;
	}

	CloseHandle(hThread);
	if (dwExitCode != 0) {
		return TRUE;
	}

	if (FALSE == PressButton(m_hMainWnd, L"&Kết thúc")) {
		return FALSE;
	}
	
	return TRUE;
}

DWORD WINAPI CAuditionLauncher::MonitorThreadFunction( PVOID pvParam )
{
	CAuditionLauncher* pLauncher = (CAuditionLauncher*)pvParam;

	while (TRUE == pLauncher->m_isMonitoring) {

		HWND hWnd = ::GetWindow(pLauncher->m_hMainWnd, GW_ENABLEDPOPUP);
		
		if (NULL != hWnd) {

			const int MAX_LENGTH = 7;
			TCHAR strWndName[MAX_LENGTH];

			::GetWindowText(hWnd, strWndName, MAX_LENGTH);

			if(lstrcmp(strWndName, L"Setup") == 0 ) {
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
				return 1;
			}
		}
		int iSleep = 2000;
		::Sleep(iSleep);
	}
	return 0;
}

CString CAuditionLauncher::GetName()
{
	return L"Audition";
}
