#include "StdAfx.h"
#include "GameSourceCompare.h"
#include "Fifa2AutoLauncher.h"
#include <tlhelp32.h>
#include "Fifa2Compare.h"
#include "MyUtils.h"

CFifa2AutoLauncher::CFifa2AutoLauncher(LPCTSTR strSource)
	:CLauncher(strSource)
{
	m_strLauncher = strSource + CString(L"\\GameLoader.exe");

	m_iAcceptCheckboxID = 0x00000426;
	m_iZone1RadioButtonID = 0x000007D1;
	m_iStartButtonID = 0x000003E9;

	m_iFailedOkButtonID = 0x00000002;

	m_strWindowName = L"GameLoader";
	m_strClassName = L"#32770";

	m_state = UNKNOWN;

	m_bIsNeedMonitor = TRUE;
	m_bIsFailed = FALSE;
}

CFifa2AutoLauncher::~CFifa2AutoLauncher(void)
{

}

BOOL CFifa2AutoLauncher::Run()
{
	StartLauncherWindow();

	if (NULL == m_hMainWindow) {
		return FALSE;
	}

	if (DISCONNECT == m_state) {
		ClickButton(m_hMainWindow, m_iFailedOkButtonID);
		return FALSE;
	}

	if (FALSE == CheckButton(m_iAcceptCheckboxID)) {
		return FALSE;
	}

	if (FALSE == CheckButton(m_iZone1RadioButtonID)) {
		return FALSE;
	}

	HWND hStartButton = ::GetDlgItem(m_hMainWindow, m_iStartButtonID);
	if (NULL == hStartButton) {
		return FALSE;
	}

	HANDLE hThread = ::CreateThread(NULL, 0, MonitorThreadFunction, this, 0, NULL);

	if (NULL == hThread) {
		return FALSE;
	}

	while (hStartButton != NULL) {
	
		if (FALSE == ClickButton(m_iStartButtonID)) {
			return FALSE;
		}

		hStartButton = ::GetDlgItem(m_hMainWindow, m_iStartButtonID);
	}

	if (FALSE == WaitForComplete()) {
		return FALSE;
	}

	::WaitForSingleObject(hThread, INFINITE);
	::CloseHandle(hThread);

	if (TRUE == m_bIsFailed)
	{
		return FALSE;
	}
	return TRUE;
}

CString CFifa2AutoLauncher::GetName()
{
	return L"FF";
}

BOOL CFifa2AutoLauncher::WaitForComplete()
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == hProcessSnap) {
		return FALSE;
	}

	while (true) {

		if (TRUE == m_bIsFailed )
		{
			return TRUE;
		}

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if( !::Process32First( hProcessSnap, &pe32 ) ) {
			::CloseHandle( hProcessSnap );  
			return FALSE ;
		}

		do {
			if (lstrcmp(pe32.szExeFile, L"FF2Client.exe") == 0) {

				HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);

				if (hProcess == NULL) {
					_tprintf(L"Open process is failed. Error: %d\n", GetLastError());
				}

				if (FALSE == ::TerminateProcess(hProcess, 0)) {
					_tprintf(L"Termiate process crossfire.dat that bai. Error: %d\n", GetLastError());
				} 

				::CloseHandle(hProcess);
				::CloseHandle( hProcessSnap );

				m_bIsNeedMonitor = FALSE;
				return TRUE;
			}

		} while (::Process32Next( hProcessSnap, &pe32 ));
	}
	
	return FALSE;
}

HWND CFifa2AutoLauncher::StartLauncherWindow()
{

	if(FALSE == StartLauncherProcess()) {
		return NULL;
	}

	m_hMainWindow = NULL;
	int iCount = 0;

	while (iCount < m_iTimeoutCount && UNKNOWN == m_state) {

		EnumWindows(CFifa2AutoLauncher::EnumWindowsProc, (LPARAM)this);
		::Sleep(m_iTimeoutSeconds);

	}

	return m_hMainWindow;
}

BOOL CALLBACK CFifa2AutoLauncher::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{

	CFifa2AutoLauncher *pLaucher = (CFifa2AutoLauncher*)lParam;

	const int MAX_LENGTH = 12;

	TCHAR strClassName[MAX_LENGTH];
	TCHAR strWindowName[MAX_LENGTH];

	::GetClassName(hwnd, strClassName, MAX_LENGTH);
	::GetWindowText(hwnd, strWindowName, MAX_LENGTH);

	if (lstrcmpi(strClassName, pLaucher->m_strClassName) == 0 
		&& lstrcmpi(strWindowName, pLaucher->m_strWindowName) == 0
		&& TRUE == IsWindowVisible(hwnd)) {

		BOOL bIsErrorDlg = FALSE;

		EnumChildWindows(hwnd, CFifa2AutoLauncher::EnumChildWindowsProc, (LPARAM)&bIsErrorDlg);

		if (TRUE == bIsErrorDlg) {

			pLaucher->m_state = DISCONNECT;

		} else {

			pLaucher->m_state = CONNECT;
		}

		pLaucher->m_hMainWindow = hwnd;

		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK CFifa2AutoLauncher::EnumChildWindowsProc( HWND hwnd, LPARAM lParam )
{
	const int MAX_LENGTH = 47;
	TCHAR strWindowName[MAX_LENGTH];

	::GetWindowText(hwnd, strWindowName, MAX_LENGTH);

	if (lstrcmpi(strWindowName, L"There is a problem with the network connection") == 0) {

		BOOL *pbIsErrorDlg = (BOOL*)lParam;
		*pbIsErrorDlg = TRUE;

		return FALSE;
	}

	return TRUE;
}

CGameSourceCompare * CFifa2AutoLauncher::GetComparer( LPCTSTR strNewSource, LPCTSTR strOldSource )
{
	if (m_Comparer != NULL)
	{
		delete m_Comparer;
	}

	m_Comparer = new CFifa2Compare(strNewSource, strOldSource);

	if (m_Comparer != NULL)
	{
		return m_Comparer;
	}

	return NULL;
}

DWORD WINAPI CFifa2AutoLauncher::MonitorThreadFunction( PVOID pvParam )
{
	CFifa2AutoLauncher* pLaucher = (CFifa2AutoLauncher*)pvParam;

	DWORD dwStartTime = GetTickCount ();

	while(TRUE == pLaucher->m_bIsNeedMonitor)
	{
		DWORD dwEndTime = GetTickCount () - dwStartTime ;

		if (dwEndTime > pLaucher->m_dwLauncherTimeOut)
		{
			pLaucher->m_bIsFailed = TRUE;

			CMyUtils::WriteErrorLog(L"FF Launcher timeout");

			if (FALSE == CMyUtils::KillWindowProcess(pLaucher->m_hMainWindow))
			{
				CMyUtils::WriteErrorLog(L"To kill FF update process is failed");
			}

			return TRUE;
		}
	}
	return TRUE;
}
