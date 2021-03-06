
#pragma once
#include "StdAfx.h"
#include "strsafe.h"
#pragma warning(disable :4995 )
#include "UserDAO.h"
#include "StringConverter.h"
#include "DialogCoincideUser.h"



CUserDAO::CUserDAO(void)
{
}

CUserDAO::~CUserDAO(void)
{
	if (m_pSQLDataAccessHelper != NULL) {
		delete m_pSQLDataAccessHelper ;
	}
}
BOOL CUserDAO::ConnectToDB(char *pcUserName, char *pcPassword, char *pcServerAddress, char *pcDatabase)
{
	m_pSQLDataAccessHelper = new CMySQLDataAccessHelper(pcUserName, pcPassword, pcServerAddress, pcDatabase);
	return m_pSQLDataAccessHelper->IsConnected();
	
}
BOOL CUserDAO::DeleteUser(const char* strUserName)
{
	CStringA cstrQuery;
	cstrQuery.Format("DELETE FROM usertb where UserName = '%s';", strUserName);
	return m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery);
}
BOOL CUserDAO::AddUser(const CSM_USER &csm_User)
{
	CStringConverter stringConverter;

	char strName[(MAX_NAME +1)*2] = {0};
	char strUsername[(MAX_USERNAME +1)*2] = {0};
	char strUserType[8] = {0};
	char strBirthDay[(MAX_STRINGDATE +1)*2] = {0};
	char strRecordDate[(MAX_STRINGDATE +1)*2] = {0};
	char strExpiryDate[(MAX_STRINGDATE +1)*2] = {0};
	char strAddress[(MAX_ADDRESS+1)*2] = {0};
	char strPhone[(MAX_PHONE+1)*2] = {0};
	char strEmail[(MAX_EMAIL+1)*2] = {0};
	
	char strRemainTime[(MAX_INTTIME+1)*2] = {0};
	char strTimeUsed[(MAX_INTTIME+1)*2] = {0};
	char strFreeTime[(MAX_INTTIME+1)*2] = {0};
	char strActive[6] = {0};
	char strRemainMoney[(MAX_MONEY +1)*2] = {0};


	

	char *pcBuffer = stringConverter.ConvertIntoUTF8(csm_User.strName);
	if(NULL != pcBuffer)
		strcpy_s(strName, (MAX_NAME +1)*2, pcBuffer);
	

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strUsername);
	if(NULL != pcBuffer)
		strcpy_s(strUsername, (MAX_USERNAME +1)*2, pcBuffer);
	
	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strUserType);
	if(NULL != pcBuffer)
		strcpy_s(strUserType, 8, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strBirthDay);
	if(NULL != pcBuffer)
		strcpy_s(strBirthDay, (MAX_STRINGDATE +1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strRecordDate);
	if(NULL != pcBuffer)
		strcpy_s(strRecordDate, (MAX_STRINGDATE +1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strExpiryDate);
	if(NULL != pcBuffer)
		strcpy_s(strExpiryDate, (MAX_STRINGDATE +1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strAddress);
	if(NULL != pcBuffer)
		strcpy_s(strAddress, (MAX_ADDRESS+1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strPhone);
	if(NULL != pcBuffer)
		strcpy_s(strPhone, (MAX_PHONE+1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strEmail);
	if(NULL != pcBuffer)
		strcpy_s(strEmail, (MAX_EMAIL+1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strRemainTime);
	if(NULL != pcBuffer)
		strcpy_s(strRemainTime, (MAX_INTTIME+1)*2, pcBuffer);
	

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strTimeUsed);
	if(NULL != pcBuffer)
		strcpy_s(strTimeUsed, (MAX_INTTIME+1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strFreeTime);
	if(NULL != pcBuffer)
		strcpy_s(strFreeTime, (MAX_INTTIME+1)*2, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strActive);
	if(NULL != pcBuffer)
		strcpy_s(strActive, 6, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(csm_User.strRemainMoney);
	if(NULL != pcBuffer)
		strcpy_s(strRemainMoney, (MAX_MONEY +1)*2, pcBuffer);

	CStringA cstrQuery = "INSERT INTO usertb ";
	cstrQuery.Append("(FirstName,LastName,MiddleName,UserName,Password,ID,Address,Phone,");
	cstrQuery.Append("Email,City,State,Zipcode,Debit,CreditLimit,Active,RecordDate,ExpiryDate,");
	cstrQuery.Append("UserType,Memo,Birthdate,SSN1,SSN2,SSN3,TimePaid,TimeUsed,MoneyPaid,MoneyUsed,");
	cstrQuery.Append("RemainTime,FreeTime,TimeTransfer,RemainMoney,FreeMoney,MoneyTransfer,UsageTimeId,");
	cstrQuery.Append("PromotionTime,PromotionMoney,MachineGroupId,MAC,changepcdetailid) ");

	cstrQuery.AppendFormat("VALUES (N'%s','','','%s',OLD_PASSWORD('csm'),'','%s','%s',", strName, strUsername,strAddress, strPhone);
	cstrQuery.AppendFormat("'%s','','','',0,0,%s,'%s','%s',", strEmail, strActive, strRecordDate, strExpiryDate);
	cstrQuery.AppendFormat("%s,'','%s','','','',0,%s,0,0,",strUserType, strBirthDay, strTimeUsed);
	cstrQuery.AppendFormat("%s, 0, 0,%s,0,0,1,",strRemainTime, strRemainMoney);
	cstrQuery.Append("0,0,0,'',0)");
	
	BOOL bResult = m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery);
	if(!bResult)
	{
		CStringA cstrSelectQuery;
		cstrSelectQuery.Format("SELECT count(UserName) FROM usertb WHERE UserName = '%s';", strUsername);
		MYSQL_RES *pMySQLQueryResult =  m_pSQLDataAccessHelper->ExecuteQuery(cstrSelectQuery);
		if(NULL == pMySQLQueryResult)
		{
			return bResult;
		}
		MYSQL_ROW mySQLResultRow;
		if((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult)))
		{
			int iCount = atoi(mySQLResultRow[0]);
			if(iCount > 0)
			{
				CString strMessage;
				strMessage.Format(_T("User '%s' đã tồn tại trong CSM Server\n"), csm_User.strUsername);
				strMessage.AppendFormat(_T("Thông tin của '%s' trong CSM Server:\n"), csm_User.strUsername);
				strMessage.AppendFormat(_T("\tHọ tên: %s\n"), csm_User.strName);
				strMessage.AppendFormat(_T("\tSố tiền dư: %s\n"), csm_User.strRemainMoney);

				if(wcscmp(_T("0000-00-00 0:0:0"), csm_User.strExpiryDate) != 0)
				{
					strMessage.AppendFormat(_T("\tNgày hết hạn: %s\n"), csm_User.strExpiryDate);
				}
				else strMessage.Append(_T("\tNgày hết hạn:\n"));
				strMessage.Append(_T("Bạn có muốn cập nhật user này không?"));
				if( AfxMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION, 0) == IDYES)
				{
					DeleteUser(strUsername);
					return m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery);
				}
				else return bResult;

				

			/*	if(true)
				{
					DeleteUser(strUsername);
					return m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery);
				}
				else
				{
					
					CSM_USER csm_UserTemp;
					StringCchCopy(csm_UserTemp.strUsername, MAX_USERNAME, csm_User.strUsername);
					StringCchCopy(csm_UserTemp.strName, MAX_NAME, csm_User.strName);
					StringCchCopy(csm_UserTemp.strExpiryDate, MAX_STRINGDATE, csm_User.strExpiryDate);
					StringCchCopy(csm_UserTemp.strRemainMoney, MAX_MONEY, csm_User.strRemainMoney);
					BOOL bOptionSave;
					CDialogCoincideUser* dlg = new CDialogCoincideUser(0, &csm_UserTemp, &bOptionSave);
					INT_PTR nResponse = dlg->DoModal();

					if (nResponse == IDOK)
					{
						DeleteUser(strUsername);
					return m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery);
					}
					else if (nResponse == IDCANCEL)
					{
						
					}
				}*/
			}
		}
	}
	return bResult;
}

INT CUserDAO::GetMoneyFromTime(const int& iTime, const int& iUserType)
{

	int iUnitPrice = GetUnitPriceFromUserType(iUserType);
	return (iUnitPrice * iTime)/60;
}
INT CUserDAO::GetUnitPriceFromUserType(const int& iUserType)
{
	CStringA cstrQuery = "SELECT Price FROM pricelisttb p where Type=2 and Active=0 limit 1;";
	MYSQL_RES *pMySQLQueryResult = m_pSQLDataAccessHelper->ExecuteQuery(cstrQuery);
	MYSQL_ROW mySQLResultRow = mysql_fetch_row(pMySQLQueryResult);
	return atoi(mySQLResultRow[0]);
}