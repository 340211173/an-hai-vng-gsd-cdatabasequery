#include "StdAfx.h"
#include "FifaLauncher.h"


CFifaLauncher::CFifaLauncher( LPCTSTR strSourcePath, LPCTSTR strLauncherPath )
	:CCrossfireLauncher(strSourcePath, strLauncherPath)
{

}

CFifaLauncher::~CFifaLauncher(void)
{
}

BOOL CFifaLauncher::Run()
{

	HWND hMainWnd = StartLauncherWindow(L"TWizardForm", L"Setup - FIFA Online 2");
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

	return TRUE;
}
