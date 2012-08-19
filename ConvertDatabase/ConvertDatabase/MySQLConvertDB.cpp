#pragma once
#include "StdAfx.h"
#include "MySQLConvertDB.h"
#include "BlackListDAO.h"
#include "UserDAO.h"
#include "ServiceDAO.h"
#include "StringConverter.h"
#include "Strsafe.h"
#include "FileHelper.h"

CMySQLConvertDB::CMySQLConvertDB(void)
{
	InitConnectString();
}

CMySQLConvertDB::~CMySQLConvertDB(void)
{
	if(m_pSQLDataAccessHelper != NULL)
	{
		m_pSQLDataAccessHelper->Disconnect();
		delete m_pSQLDataAccessHelper;
	}
}

void CMySQLConvertDB::InitConnectString()
{
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strUsername, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strUsername);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strPasssword, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strPasssword);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strServerAddress, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strServerAddress);
	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strDatabaseName, MAX_LENGTH, "%s", "inetdb");
}

BOOL CMySQLConvertDB::CheckPathInDataMySQL(const TCHAR* strPathDirectory)
{
	CString strPath(strPathDirectory);
	int iIndex = strPath.ReverseFind(L'\\');
	strPath.Delete(iIndex +1, strPath.GetLength() - iIndex);
	
	if(strPath.MakeLower().Compare(m_dataDirMySQLPath.MakeLower()) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CMySQLConvertDB::CopyDataToDataMySQL(const TCHAR* strPathDirectory)
{
	TCHAR strDataBaseName[MAX_PATH] = {0};
	CStringConverter stringConverter;

	CString strPath(strPathDirectory);
	int iIndex = strPath.ReverseFind(L'\\');
	strPath.Delete(0, iIndex + 1);

	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strDatabaseName, MAX_LENGTH, "%s", stringConverter.UnicodeToUTF8(strPath.GetBuffer()));

	if(!CheckPathInDataMySQL(strPathDirectory))
	{
		CFileHelper fileHelper;
		return fileHelper.CopyFolder(strPathDirectory, m_dataDirMySQLPath);
		
	}
	return TRUE;
}

BOOL CMySQLConvertDB::DeleteDataCopy(const TCHAR* strPathDirectory)
{
	CString strDirName(strPathDirectory);
	int iIndex = strDirName.ReverseFind(L'\\');
	strDirName.Delete(0, iIndex + 1);
	
	CString strTemp;
	strTemp.Format(_T("%s%s"), m_dataDirMySQLPath, strDirName);
	CFileHelper fileHelper;
	
	return fileHelper.DeleteFolder(strTemp);
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
		if(NULL != m_pSQLDataAccessHelper && m_pSQLDataAccessHelper->IsConnected())
		{
			m_pSQLDataAccessHelper->Disconnect();
		}
		return FALSE;
	}
	if(!ConvertUser())
	{
		if(NULL != m_pSQLDataAccessHelper && m_pSQLDataAccessHelper->IsConnected())
		{
			m_pSQLDataAccessHelper->Disconnect();
		}
		return FALSE;
	}
	if(!ConvertService())
	{
		if(NULL != m_pSQLDataAccessHelper && m_pSQLDataAccessHelper->IsConnected())
		{
			m_pSQLDataAccessHelper->Disconnect();
		}
		return FALSE;
	}

	if(NULL != m_pSQLDataAccessHelper && m_pSQLDataAccessHelper->IsConnected())
	{
		m_pSQLDataAccessHelper->Disconnect();
	}

	// Delete database in data directory
	if(!CheckPathInDataMySQL(strPath))
	{
		DeleteDataCopy(strPath);
	}
	
	return TRUE;
}
BOOL CMySQLConvertDB::Connect(const TCHAR* strPath)
{

	StringCchPrintfA(m_ConnectStringToMYSQLOneNet.m_strPasssword, MAX_LENGTH, "%s", m_ConnectStringToMYSQL.m_strPasssword);
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
		
		if(NULL == pMySQLQueryResult)
		{
			return FALSE;
		}
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
		m_pSQLDataAccessHelper->FreeQueryResult(pMySQLQueryResult);
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
	char* strQuery = "SELECT * FROM tbluser WHERE UserGroupId = 2 || UserGroupId = 3;";
	TRY
	{
		CUserDAO userDAO;
		if(!userDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			AfxMessageBox(_T("Không thể kết nối cơ sở dữ liệu của csm server"), MB_ICONSTOP);
			return FALSE;
		}
		
		CStringConverter stringConverter;

		
		MYSQL_RES *pMySQLQueryResult = m_pSQLDataAccessHelper->ExecuteQuery(strQuery);
		
		MYSQL_ROW mySQLResultRow;

		if(NULL == pMySQLQueryResult)
		{
			return FALSE;
		}
		while ((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult)))
		{
			CSM_USER csm_User;
			
			//UserName
			StringCchPrintf(csm_User.strUsername, MAX_USERNAME, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[1]));

			//FullName
			StringCchPrintf(csm_User.strName, MAX_NAME, _T("%s"), stringConverter.UTF8ToUnicode(mySQLResultRow[3]));
			

			//Birthday
			StringCchPrintf(csm_User.strBirthDay, MAX_STRINGDATE, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[4]));
			if(wcscmp(_T("1900-01-01 00:00:00"), csm_User.strBirthDay) == 0)
			{
				StringCchPrintf(csm_User.strBirthDay, MAX_STRINGDATE, _T("0000-00-00 0:0:0"));
			}

			//Address
			StringCchPrintf(csm_User.strAddress, MAX_ADDRESS, _T(""));
			StringCchPrintf(csm_User.strEmail, MAX_ADDRESS, _T(""));

			//Phone
			StringCchPrintf(csm_User.strPhone, MAX_PHONE, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[7]));
			

			//UserType
			StringCchPrintf(csm_User.strUserType, 6, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[8]));
			if(wcscmp(csm_User.strUserType, _T("3")) == 0)
			{
				StringCchPrintf(csm_User.strUserType, 6, _T("4"));
			}
			
			//RecordDay
			StringCchPrintf(csm_User.strRecordDate, MAX_STRINGDATE, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[10]));
			if(wcscmp(_T("1900-01-01 00:00:00"), csm_User.strRecordDate) == 0)
			{
				StringCchPrintf(csm_User.strRecordDate, MAX_STRINGDATE, _T("0000-00-00 0:0:0"));
			}

			//ExpiryDay
			StringCchPrintf(csm_User.strExpiryDate, MAX_STRINGDATE, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[11]));
			if(wcscmp(_T("1900-01-01 00:00:00"), csm_User.strExpiryDate) == 0)
			{
				StringCchPrintf(csm_User.strExpiryDate, MAX_STRINGDATE, _T("0000-00-00 0:0:0"));
			}

			//Active
			StringCchPrintf(csm_User.strActive, 6, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[12]));

			//TimeUsed
			StringCchPrintf(csm_User.strTimeUsed, MAX_INTTIME, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[14]));

			//TimeRemain
			StringCchPrintf(csm_User.strRemainTime, MAX_INTTIME, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[15]));

			//MoneyRemain
			StringCchPrintf(csm_User.strRemainMoney, MAX_MONEY, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[17]));
			
			//MoneyFree
			StringCchPrintf(csm_User.strFreeMoney, MAX_MONEY, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[18]));
			

			
			//Insert user to csm db
			if(!userDAO.AddUser(csm_User))
			{
				
				AfxMessageBox(_T("Chuyển đổi thất bại"));
				return FALSE;
			}
			
		}
		m_pSQLDataAccessHelper->FreeQueryResult(pMySQLQueryResult);
		
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		AfxMessageBox(errMsg, MB_ICONSTOP, 0);
	}
	END_CATCH_ALL
	return TRUE;
}
BOOL CMySQLConvertDB::ConvertService()
{
	char* strQuery = "SELECT * FROM tblservice";
	TRY
	{
		CServiceDAO serviceDAO;
		if(!serviceDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			AfxMessageBox(_T("Không thể kết nối cơ sở dữ liệu của csm server"), MB_ICONSTOP);
			return FALSE;
		}
		CStringConverter stringConverter;

		
		MYSQL_RES *pMySQLQueryResult = m_pSQLDataAccessHelper->ExecuteQuery(strQuery);
		
		MYSQL_ROW mySQLResultRow;

		if(NULL == pMySQLQueryResult)
		{
			return FALSE;
		}
		while ((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult)))
		{
			CSM_SERVICE csm_Service;
			
			//ServiceName 
			StringCchPrintf(csm_Service.strServiceName, MAX_SERVICENAME, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[1]));

			//Unit
			StringCchPrintf(csm_Service.strUnit, MAX_LENGTH, _T("%s"), stringConverter.UTF8ToUnicode( mySQLResultRow[2]));

			//Price
			csm_Service.iServicePrice = atoi(mySQLResultRow[3]);
			
			
			//Insert user to csm db
			if(!serviceDAO.AddService(csm_Service))
			{
				AfxMessageBox(_T("Chuyển đổi thất bại"));
				return FALSE;
			}
			
		}
		m_pSQLDataAccessHelper->FreeQueryResult(pMySQLQueryResult);
		
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		AfxMessageBox(errMsg, MB_ICONSTOP, 0);
	}
	END_CATCH_ALL
	return TRUE;
}