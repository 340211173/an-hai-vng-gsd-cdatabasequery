#pragma once
#include "StdAfx.h"
#include "MySQLConvertDB.h"
#include "BlackListDAO.h"
#include "UserDAO.h"
#include "ServiceDAO.h"
#include "StringConverter.h"
#include "Strsafe.h"

CMySQLConvertDB::CMySQLConvertDB(void)
{
	InitConnectString();
}

CMySQLConvertDB::~CMySQLConvertDB(void)
{
	if(m_pSQLDataAccessHelper != NULL)
		delete m_pSQLDataAccessHelper;
}

void CMySQLConvertDB::InitConnectString()
{

	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strUsername, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strUsername);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strPasssword, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strPasssword);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strServerAddress, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strServerAddress);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strDatabaseName, MAX_LENGTH, "%s", "inetdb");
}

BOOL CMySQLConvertDB::Convert(const TCHAR* strPath)
{
	if(!Connect(strPath))
	{
		AfxMessageBox(_T("Không thể kết nối cơ sở dữ liệu"), 0, 0);
		return FALSE;
	}
	
	if(!ConvertBlackList())
	{
		return FALSE;
	}
	/*if(!ConvertUser())
	{
		return FALSE;
	}
	if(!ConvertService())
	{
		return FALSE;
	}*/
	return TRUE;
}
BOOL CMySQLConvertDB::Connect(const TCHAR* strPath)
{
	m_pSQLDataAccessHelper = new CMySQLDataAccessHelper(m_ConnectStringToMYSQLOneNet.m_strUsername,
														m_ConnectStringToMYSQLOneNet.m_strPasssword,
														m_ConnectStringToMYSQLOneNet.m_strServerAddress,
														m_ConnectStringToMYSQLOneNet.m_strDatabaseName
														);
	return m_pSQLDataAccessHelper->IsConnected();
	
}

BOOL CMySQLConvertDB::ConvertBlackList()
{
	TRY
	{
		
		

		CBlackListDAO blackListDAO;
		if(!blackListDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			AfxMessageBox(_T("Không thể kết nối cơ sở dữ liệu của csm server"), MB_ICONSTOP);
			return FALSE;
		}
		CStringConverter stringConverter;

		MYSQL_RES *pMySQLQueryResult = m_pSQLDataAccessHelper->ExecuteQuery("SELECT URL, CreateDate FROM tblblacklistweb");
		MYSQL_ROW mySQLResultRow;
		

		while((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult)))
		{
			BLACKLIST blackList;
			
			StringCchPrintf(blackList.strURL, MAX_PATH, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[0]));
			StringCchPrintf(blackList.strRecordDate, MAX_STRINGDATE, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[1]));
			blackList.iActive = 1;
			blackList.iAddedBy = 1;
			if(!blackListDAO.AddBlackList(blackList))
			{
				return FALSE;
			}
		}
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		AfxMessageBox(errMsg, MB_ICONSTOP, 0);
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}
BOOL CMySQLConvertDB::ConvertUser()
{
	return FALSE;
}
BOOL CMySQLConvertDB::ConvertService()
{
	return FALSE;
}