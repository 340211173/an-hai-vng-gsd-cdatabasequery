#include "StdAfx.h"
#include "LMHTAutoLauncher.h"
#include "GlobalClass.h"
#include "Include/pugixml.hpp"
#include "MyUtils.h"
#include "WindowFinder.h"
#include <iostream>
#include "LMHTCompare.h"
using namespace pugi;
using namespace std;

CLMHTAutoLauncher::CLMHTAutoLauncher( LPCTSTR strSource )
:CLauncherEx(strSource)
{
	m_strLauncher = strSource + CString(L"\\Lien Minh Huyen Thoai.exe");
	m_bIsInitSuccessful = Init();
}

CLMHTAutoLauncher::~CLMHTAutoLauncher(void)
{
}

CString CLMHTAutoLauncher::GetName()
{
	return L"LOL";
}

BOOL CLMHTAutoLauncher::Run()
{
	if (FALSE == m_bIsInitSuccessful)
	{
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"Failure to start LMHT Launcher");
		return FALSE;
	}

	if (NULL == StartLauncherWindow(m_strBeforeLoginGarenaWndClass, m_strBeforeLoginGarenaWndTitle))
	{
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"CLMHTLaucher: Unable to start launcher process");
		return FALSE;
	}

	HWND hErrorDlg = CLauncherEx::WaitForWindow(m_strLastErrorWndClass, m_strLastErrorWndTitle, m_dwSleep);
	if (NULL != hErrorDlg)
	{
		CMyUtils::KillWindowProcess(hErrorDlg);
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"CLMHTLaucher: No network connection");
		return FALSE;
	}

	HWND hGerenaMessageWnd = NULL;
	int iTryCount = 3;
	int iTry = 0;

	while(iTry < iTryCount && NULL == hGerenaMessageWnd)
	{
		DWORD dwWait = 1*60*1000;

		CWindowFinder windowFinder;
		vector<HWND> listMainWnd = windowFinder.GetListWindow(m_strBeforeLoginGarenaWndClass, m_strBeforeLoginGarenaWndTitle);

		CMyUtils::SimulateLeftMouseClick(listMainWnd,  m_xLoginGarenaButton, m_yLoginGarenaButton);
		hGerenaMessageWnd = CLauncherEx::WaitForWindow(m_strAfterLoginGarenaWndClass, m_strAfterLoginGarenaWndTitle, dwWait);

		if (NULL == hGerenaMessageWnd)
		{
			CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_WARN, L"CLMHTLaucher: Failure to click Garena login button");
		}
		iTry++;
	}

	m_hMainWindow = hGerenaMessageWnd;

	if (NULL == m_hMainWindow)
	{
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"CLMHTLaucher: Timeout for wating for log in Garena");
		return FALSE;
	}

	DWORD dwThreadID = ::GetWindowThreadProcessId(m_hMainWindow, NULL);
	if (0 == dwThreadID)
	{
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"CLMHTLaucher: Failure to get the main thread ID");
		return FALSE;
	}

	::Sleep(m_dwSleep);
	EnumThreadWindows(dwThreadID, EnumChildWindowsProc, (LPARAM)this);

	iTryCount = 3;
	iTry = 0;
	HWND hLMHTLauncher = NULL;
	DWORD dwWaitLuncher = m_dwLauncherTimeOut/iTryCount;

	while(iTry < iTryCount && NULL == hLMHTLauncher)
	{
		hLMHTLauncher = CLauncherEx::WaitForWindow(m_strLMHTLauncherWndClass, m_strLMHTLauncherWndTitle, dwWaitLuncher);
		if (NULL == hLMHTLauncher)
		{
			EnumThreadWindows(dwThreadID, EnumChildWindowsProc, (LPARAM)this);
			CGlobalClass::GetInstance()->GetLogWriter()->WriteLogFormat(LOG_TYPE_WARN, L"CLMHTLaucher: Finding launcher window timeout %d", iTry);
		}
		iTry++;
	}

	if (NULL == hLMHTLauncher)
	{
		CMyUtils::KillWindowProcess(m_hMainWindow);
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"CLMHTLaucher: Launcher timeout");
		return FALSE;
	}

	PostMessage(hLMHTLauncher, WM_CLOSE, 0, 0);
	CMyUtils::KillWindowProcess(m_hMainWindow);

	if (TRUE == CLauncherEx::IsWindowShowed(hLMHTLauncher))
	{
		CMyUtils::KillWindowProcess(hLMHTLauncher);
	}

	return TRUE;
}

BOOL CLMHTAutoLauncher::Init()
{
	if (!m_ConfigLoadingResult)
	{
		CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, L"Failure to load launcher config");
		return FALSE;
	}
	m_xLoginGarenaButton = m_launcherConfig.child("lmht").child("login_garena").attribute("x").as_int();
	m_yLoginGarenaButton = m_launcherConfig.child("lmht").child("login_garena").attribute("y").as_int();
	m_xLoginLMHTButton = m_launcherConfig.child("lmht").child("login_lmht").attribute("x").as_int();
	m_yLoginLMHTButton = m_launcherConfig.child("lmht").child("login_lmht").attribute("y").as_int();

	string strTempUTF8 = m_launcherConfig.child("lmht").child("before_login_garena").attribute("class").value();
	TCHAR *pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strBeforeLoginGarenaWndClass = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("before_login_garena").attribute("title").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strBeforeLoginGarenaWndTitle = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("after_login_garena").attribute("class").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strAfterLoginGarenaWndClass = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("after_login_garena").attribute("title").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strAfterLoginGarenaWndTitle = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("lmht_login").attribute("class").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLMHTLoginWndClass = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("lmht_login").attribute("title").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLMHTLoginWndTitle = pTempWidechar;
		delete[] pTempWidechar;
	}


	strTempUTF8 = m_launcherConfig.child("lmht").child("lmht_launcher").attribute("class").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLMHTLauncherWndClass = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("lmht_launcher").attribute("title").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLMHTLauncherWndTitle = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("last_error_dialog").attribute("class").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLastErrorWndClass = pTempWidechar;
		delete[] pTempWidechar;
	}

	strTempUTF8 = m_launcherConfig.child("lmht").child("last_error_dialog").attribute("title").value();
	pTempWidechar =  CMyUtils::ConvertUTF8ToWideChar(strTempUTF8.c_str());
	if (pTempWidechar != NULL)
	{
		m_strLastErrorWndTitle = pTempWidechar;
		delete[] pTempWidechar;
	}

	m_dwSleep = m_launcherConfig.child("lmht").child("sleep").attribute("time").as_uint();
	return TRUE;
}

BOOL CLMHTAutoLauncher::EnumChildWindowsProc( HWND hwnd, LPARAM lParam )
{
	const int MAX_LENGTH = 100;

	TCHAR strClassName[MAX_LENGTH] = {0};
	TCHAR strWindowName[MAX_LENGTH] = {0};

	::GetClassName(hwnd, strClassName, MAX_LENGTH);
	::GetWindowText(hwnd, strWindowName, MAX_LENGTH);

	CLMHTAutoLauncher *pLauncher = (CLMHTAutoLauncher*)lParam;
	if (lstrcmpi(strClassName, pLauncher->m_strLMHTLoginWndClass) == 0 && lstrcmpi(strWindowName, pLauncher->m_strLMHTLoginWndTitle) == 0)
	{
		CMyUtils::SimulateLeftMouseClick(hwnd, pLauncher->m_xLoginLMHTButton, pLauncher->m_yLoginLMHTButton);
	}

	return TRUE;
}

CGameSourceCompare * CLMHTAutoLauncher::GetComparer( LPCTSTR strNewSource, LPCTSTR strOldSource )
{
	if (m_Comparer != NULL)
	{
		delete m_Comparer;
	}

	m_Comparer = new CLMHTCompare(strNewSource, strOldSource);

	if (m_Comparer != NULL)
	{
		return m_Comparer;
	}

	return NULL;
}

HWND CLMHTAutoLauncher::StartLauncherWindow( LPCTSTR strClassName, LPCTSTR strWindowName )
{
	if(FALSE == StartLauncherProcess()) {
		return NULL;
	}

	DWORD dwWait = 5*60*1000;
	m_hMainWindow = CLMHTAutoLauncher::WaitForWindow(strClassName,  strWindowName, dwWait);

	return m_hMainWindow;
}

HWND CLMHTAutoLauncher::WaitForWindow( LPCTSTR strClassName, LPCTSTR strWindowName, DWORD dwTimeout )
{
	HWND hWnd = NULL;
	DWORD dwStartTime = GetTickCount ();

	while (NULL == hWnd)
	{
		DWORD dwSpentTime = GetTickCount () - dwStartTime;
		if (dwSpentTime > dwTimeout)
		{
			return NULL;
		}
		hWnd = ::FindWindow(strClassName, strWindowName);
	}
	return hWnd;
}
