#pragma once
#include <afxcmn.h>
#include <afxdb.h>

#define MAX_LENGTH 50
typedef struct	
{
	bool m_bIsConnected;
	char m_strUsername[MAX_LENGTH];
	char m_strPasssword[MAX_LENGTH];
	char m_strServerAddress[MAX_LENGTH];
	char m_strDatabaseName[MAX_LENGTH];
}CONNECTSTRINGTOMYSQL;

class CConvertDB
{
protected:
	CONNECTSTRINGTOMYSQL m_ConnectStringToMYSQL;
	CDatabase m_db;
public:
	CConvertDB(void);
	~CConvertDB(void);

	void Convert(const TCHAR* strPath);
	BOOL Connect(const TCHAR* strPath);

	BOOL ConvertBlackList();
	BOOL ConvertPriceListTb(); // from price to pricelisttb
	
};