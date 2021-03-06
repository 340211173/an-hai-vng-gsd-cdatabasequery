#include "StdAfx.h"
#include "Fifa2Launcher.h"


CFifaLauncher2::CFifaLauncher2( LPCTSTR strSourcePath, LPCTSTR strLauncherPath )
	:CCrossfireLauncher(strSourcePath, strLauncherPath)
{

}

CFifaLauncher2::~CFifaLauncher2(void)
{
}

BOOL CFifaLauncher2::Run()
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

CString CFifaLauncher2::GetName()
{
	return L"Fifaonline2";
}
