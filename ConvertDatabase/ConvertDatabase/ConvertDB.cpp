#pragma once
#include "StdAfx.h"
#include "ConvertDB.h"
#include "BlackListDAO.h"
#include "UserDAO.h"
#include "ServiceDAO.h"
#include "Strsafe.h"
#include "StringConverter.h"
#include "MySQLDataAccessHelper.h"


CConvertDB::CConvertDB(void)
{
	StringCchPrintfA(m_ConnectStringToMYSQL.m_strUsername, MAX_LENGTH, "%s", "root");
	//StringCchPrintfA(m_ConnectStringToMYSQL.m_strPasssword, MAX_LENGTH, "%s", "hai");
	StringCchPrintfA(m_ConnectStringToMYSQL.m_strServerAddress, MAX_LENGTH, "%s", "localhost");
	StringCchPrintfA(m_ConnectStringToMYSQL.m_strDatabaseName, MAX_LENGTH, "%s", "ddm");
}

BOOL CConvertDB::ConnectToCSMDB(const TCHAR* strPassword)
{
	SetPasswordMySQL(strPassword);
	CMySQLDataAccessHelper *pSQLDataAccessHelper;
	pSQLDataAccessHelper = new CMySQLDataAccessHelper(m_ConnectStringToMYSQL.m_strUsername,
														m_ConnectStringToMYSQL.m_strPasssword,
														m_ConnectStringToMYSQL.m_strServerAddress,
														m_ConnectStringToMYSQL.m_strDatabaseName);
	BOOL bResultConnect = pSQLDataAccessHelper->IsConnected();
	if(bResultConnect)
	{
		MYSQL_RES *pMySQLQueryResult = pSQLDataAccessHelper->ExecuteQuery("SHOW VARIABLES LIKE 'datadir';");
		MYSQL_ROW mySQLResultRow;
		CStringConverter stringConverter;
		while((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult)))
		{
			m_dataDirMySQLPath.Format(stringConverter.UTF8ToUnicode( mySQLResultRow[1]));
		}
		pSQLDataAccessHelper->FreeQueryResult(pMySQLQueryResult);
	}
	if(NULL != pSQLDataAccessHelper)
	{
		delete pSQLDataAccessHelper;
	}
	return bResultConnect;
}


CConvertDB::~CConvertDB(void)
{
}

VOID CConvertDB::VariantToCstring(const CDBVariant &var, CString &sData)
{
	switch (var.m_dwType)
    {
        case DBVT_STRING:
            sData.Format(_T("%s"), var.m_pstring->GetBuffer(var.m_pstring->GetLength()));
            break;
        case DBVT_ASTRING:
            sData.Format(_T("%s"), var.m_pstringA->GetBuffer(var.m_pstringA->GetLength()));
            break;
        case DBVT_WSTRING:
            sData.Format(_T("%s"), var.m_pstringW->GetBuffer(var.m_pstringW->GetLength()));
            break;
		case DBVT_BOOL:
			sData.Format(_T("%d"), var.m_boolVal);
			break;		
		case DBVT_SHORT:
			sData.Format(_T("%d"), var.m_iVal);
			break;
		case DBVT_LONG:
			sData.Format(_T("%d"), var.m_lVal);
			break;
		case DBVT_UCHAR:
			sData.Format(_T("%d"), var.m_chVal);
			break;
		case DBVT_DOUBLE:
			sData.Format(_T("%f"), var.m_dblVal);
			break;
		case DBVT_DATE:
			sData.Format(_T("%d-%d-%d %d:%d:%d"), var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour, var.m_pdate->minute, var.m_pdate->second);
			break;
        default:
            sData = _T("");
   }
}


VOID CConvertDB::SetPasswordMySQL(const TCHAR* strPassword)
{
	StringCchPrintfA(m_ConnectStringToMYSQL.m_strPasssword, MAX_LENGTH, "%s", CW2A(strPassword));
}