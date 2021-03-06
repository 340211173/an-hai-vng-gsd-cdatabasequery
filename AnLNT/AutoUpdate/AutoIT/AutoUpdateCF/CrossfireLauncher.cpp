#include "StdAfx.h"
#include "CrossfireLauncher.h"



CCrossfireLauncher::CCrossfireLauncher( LPCTSTR strSourcePath, LPCTSTR strLauncherPath )
	:CAutoLauncher(strSourcePath, strLauncherPath)
{

}

CCrossfireLauncher::~CCrossfireLauncher(void)
{
}

BOOL CCrossfireLauncher::Run()
{

	HWND hMainWnd = StartLauncherWindow(L"TWizardForm", L"Setup - Đột Kích");

	if (NULL == hMainWnd) {
		return FALSE;
	}

	if (FALSE == PressButton(hMainWnd, L"&Tiếp theo >")) {
		return FALSE;
	}

	if (FALSE == SetSourcePath(hMainWnd)) {
		return FALSE;
	}

	if (FALSE == PressButton(hMainWnd, L"&Cài đặt")) {
		return FALSE;
	}

	if (FALSE == PressButton(hMainWnd, L"&Kết thúc")) {
		return FALSE;
	}

	if (FALSE == CloseWindow(L"#32770", L"CrossFire")) {
		return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK CCrossfireLauncher::FindSourcePathEditEnumProc( HWND hWnd, LPARAM lParam )
{

	const int MAX_LENGTH = 128;
	TCHAR strClassName[MAX_LENGTH];

	::GetClassName(hWnd, strClassName, MAX_LENGTH);

	if (lstrcmpi(strClassName, L"TEdit") == 0) {
	
		CCrossfireLauncher *pLauncher = (CCrossfireLauncher*)lParam;

		::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)pLauncher->m_strSourcePath);
		pLauncher->m_bResult = TRUE;

		return FALSE;
	}

	return TRUE;
}

BOOL CCrossfireLauncher::SetSourcePath( HWND hMainWnd )
{

	m_bResult = FALSE;

	::EnumChildWindows(hMainWnd, CCrossfireLauncher::FindSourcePathEditEnumProc, (LPARAM)this);

	if (m_bResult == TRUE) {
		return PressButton(hMainWnd, L"&Tiếp theo >");
	}

	return FALSE;
}

CString CCrossfireLauncher::GetName()
{
	return L"Crossfire";
}
