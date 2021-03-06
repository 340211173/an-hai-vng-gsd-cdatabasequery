#include "StdAfx.h"
#include "FolderCompare.h"
#include "FileServices.h"
#include "MyPath.h"
#include "md5.h"
#include "MyUtils.h"


CFolderCompare::CFolderCompare( LPCTSTR strNewSource, LPCTSTR strOldSource )
{

	m_strNewSource = strNewSource;
	m_strOldSource = strOldSource;

	m_hResultFile = INVALID_HANDLE_VALUE;
	m_hTempFile = INVALID_HANDLE_VALUE;

}

CFolderCompare::~CFolderCompare(void)
{

}

BOOL CFolderCompare::ProcessFile( LPCTSTR strFileName )
{

	if (FALSE == CheckFileExist(strFileName)) {
		return WriteFile(strFileName, m_hResultFile);
	}

	CMyPath myPath;
	CString strOldFileName = myPath.TransferSourcePath(m_strNewSource, m_strOldSource, strFileName);

	if (FALSE == CheckSum(strFileName, strOldFileName)) {
		return WriteFile(strFileName, m_hTempFile);
	}

	return TRUE;
}

BOOL CFolderCompare::Compare( LPCTSTR strResultFilePath )
{
	CFileServices fileServices;

	if (FALSE == fileServices.IsDirectory(m_strNewSource) || FALSE == fileServices.IsDirectory(m_strOldSource)) {
		return FALSE;
	}

	if (FALSE == CreateResultFile(strResultFilePath)) {
		return FALSE;
	}

	if (FALSE == Process(m_strNewSource)) {
		return FALSE;
	}

	if (m_hResultFile != INVALID_HANDLE_VALUE && m_hTempFile != INVALID_HANDLE_VALUE) {
		if (FALSE == CreateFinalResult()) {
			return FALSE;
		}
	}
	Commit();
	return TRUE;
}

BOOL CFolderCompare::CheckFileExist( LPCTSTR strFileName )
{
	CMyPath myPath;
	CString strPath = myPath.TransferSourcePath(m_strNewSource, m_strOldSource, strFileName);

	WIN32_FIND_DATA FindFileData;

	HANDLE hFind = ::FindFirstFile(strPath, &FindFileData);

	if (INVALID_HANDLE_VALUE == hFind) {
		return FALSE;
	}

	::FindClose(hFind);
	return TRUE;
}

BOOL CFolderCompare::CheckSum( LPCTSTR strFirstFile, LPCTSTR strSecondFile )
{
	CMD5 md5;

	CString firstHashCode = md5.MDFile(strFirstFile);
	CString secondHashCode = md5.MDFile(strSecondFile);

	return firstHashCode == secondHashCode;
}

BOOL CFolderCompare::CreateResultFile( LPCTSTR strFileName )
{
	m_hResultFile = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hResultFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	
	DWORD dwBytesWritten = 0;

	TCHAR strBuffer[] = L"New Files:\r\n";
	DWORD dwNumberOfBytesToWrite = (lstrlen(strBuffer)) * sizeof(TCHAR);

	if (FALSE == ::WriteFile(m_hResultFile, strBuffer, dwNumberOfBytesToWrite, &dwBytesWritten, 0)) {
		return FALSE;
	}
	
	if(FALSE == CreateTempResultFile()) {
		return FALSE;
	}

	return TRUE;
}

BOOL CFolderCompare::CreateTempResultFile()
{
	DWORD dwBytesWritten = 0;
	TCHAR strTempFileName[MAX_PATH];  
	TCHAR strTempPath[MAX_PATH];

	if (0 == ::GetTempPath(MAX_PATH, strTempPath)) {
		return FALSE;
	}

	if (0 == ::GetTempFileName(strTempPath, L"vng", 0,  strTempFileName)) {
		return FALSE;
	}

	m_hTempFile = ::CreateFile(strTempFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);

	if (m_hTempFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	TCHAR strTempBuffer[] = L"\r\nModified Files:\r\n";
	DWORD dwNumberOfBytesToWrite = (lstrlen(strTempBuffer)) * sizeof(TCHAR);

	if (FALSE == ::WriteFile(m_hTempFile, strTempBuffer, dwNumberOfBytesToWrite, &dwBytesWritten, 0)) {
		return FALSE;
	}
	return TRUE;
}


BOOL CFolderCompare::WriteFile( LPCTSTR strFileName, HANDLE hFile )
{
	CString strText =  CString(L"\r\t") + strFileName + CString(L"\r\n");

	DWORD dwBytesWritten = 0;
	DWORD dwNumberOfBytesToWrite =  strText.GetLength()* sizeof(TCHAR);

	if (FALSE == ::WriteFile(hFile, strText.GetBuffer(), dwNumberOfBytesToWrite, &dwBytesWritten, 0)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CFolderCompare::CreateFinalResult()
{
	DWORD dwPtr = ::SetFilePointer(m_hTempFile, 0, NULL, FILE_BEGIN);

	if (dwPtr == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
		return FALSE;
	}

	const int BUFFER_LENGTH = 4096;
	char strBuffer[BUFFER_LENGTH] = {0};
	DWORD  dwBytesRead = 0;

	do {

		if (FALSE == ::ReadFile(m_hTempFile, strBuffer, BUFFER_LENGTH, &dwBytesRead, NULL)) {
			return FALSE;
		}

		if (dwBytesRead > 0) {

			DWORD dwBytesWritten = 0;

			if (FALSE == ::WriteFile(m_hResultFile, strBuffer, dwBytesRead, &dwBytesWritten, 0)) {
				return FALSE;
			}
		}

	} while (dwBytesRead != 0);

	return TRUE;
}

void CFolderCompare::Commit()
{
	if(m_hResultFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hResultFile);
		m_hResultFile = INVALID_HANDLE_VALUE;
	}

	if (m_hTempFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hTempFile);
		m_hTempFile = INVALID_HANDLE_VALUE;
	}
}
