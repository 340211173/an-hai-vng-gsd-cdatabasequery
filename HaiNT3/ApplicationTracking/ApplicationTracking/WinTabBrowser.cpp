#pragma once
#include "StdAfx.h"
#include "WinTabBrowser.h"

CWinTabBrowser::CWinTabBrowser(BSTR strTitle, BSTR strURL)
{
	m_strTitle.Format(_T("%s"), strTitle);
	m_strURL.Format(_T("%s"), strURL);
}
	
CWinTabBrowser::CWinTabBrowser(CString pstrTitle, CString pstrURL)
{
	m_strTitle = pstrTitle;
	m_strURL = pstrURL;
}

CWinTabBrowser::~CWinTabBrowser(void)
{
	
}

LPCTSTR CWinTabBrowser::GetTitle()
{
	CString strTemp(m_strTitle);
	return strTemp.GetBuffer();
}
LPCTSTR CWinTabBrowser::GetURL()
{
	CString strTemp(m_strURL);
	return strTemp;
}

BOOL CWinTabBrowser::CompareWinTab(CWinTabBrowser winTab)
{
	
	CString strBuffer = winTab.GetTitle();

	if(m_strTitle.Compare(strBuffer.GetBuffer()) != 0)
	{
		return FALSE;
	}
	strBuffer = winTab.GetURL();
	if(CompareServerHost(strBuffer))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CWinTabBrowser::CompareServerHost(const TCHAR* pstrURL)
{
	if(wcscmp( GetServerHost(m_strURL.GetBuffer()), GetServerHost(pstrURL)) == 0)
	{
		return TRUE;
	}
	return FALSE;
}


CString CWinTabBrowser::GetServerHost(const TCHAR* pstrURL)
{

	CString strHost;//(pstrURL);
	strHost.Format(_T("%s"), pstrURL);
	int iIndex = strHost.Find(_T("https://"), 0);
	if(iIndex >= 0)
	{
		strHost.Delete(iIndex,8);
	}
	iIndex = strHost.Find(_T("http://"), 0);
	if(iIndex >= 0)
	{
		strHost.Delete(iIndex,7);
	}

	iIndex = strHost.Find(L"/");

	strHost.Delete(iIndex, strHost.GetLength() - iIndex);
	return strHost;
}

CString CWinTabBrowser::GetServerHostAndPath(const TCHAR* pstrURL)
{

	CString strHost;//(pstrURL);
	strHost.Format(_T("%s"), pstrURL);
	int iIndex = strHost.Find(_T("https://"), 0);
	if(iIndex >= 0)
	{
		strHost.Delete(iIndex,8);
	}
	iIndex = strHost.Find(_T("http://"), 0);
	if(iIndex >= 0)
	{
		strHost.Delete(iIndex,7);
	}

	iIndex = strHost.Find(L"/");
	iIndex = strHost.Find(L"/", iIndex +1);
	strHost.Delete(iIndex, strHost.GetLength() - iIndex);
	return strHost;
}

