#include "StdAfx.h"
#include "DuyetFile.h"
#include <windows.h>
#include "Strsafe.h"
#include "fstream"
#include <string>
#include <iostream>
#include "DownloadFileSocket.h"
#include "FileServices.h"
#include "StrSafe.h"

CDuyetFile::CDuyetFile(void)
{
	wmemset(strRootPath, 0, MAX_PATH);
}

CDuyetFile::~CDuyetFile(void)
{
}
void CDuyetFile::ClearList()
{
	
}

void CDuyetFile::ApproveFolderToList(const TCHAR *strPath)
{
	ClearList();

	TCHAR strPathFileData[MAX_PATH] = {0};

	GetCurrentDirectory(MAX_PATH, strPathFileData);
	StringCchCat(strPathFileData, MAX_PATH, _T("\\danhsach.ini"));
	
	// UTF16-LE BOM(FFFE)
	WORD wBOM = 0xFEFF;
	DWORD NumberOfBytesWritten;

	HANDLE hFileOut = CreateFile(strPathFileData, FILE_WRITE_DATA, 0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFileOut)
	{
		AfxMessageBox(_T("Không thể tạo file"), MB_ICONSTOP);
		return;
	}
	//write unicode.
	::WriteFile(hFileOut, &wBOM, sizeof(WORD), &NumberOfBytesWritten, NULL);
	TCHAR strTemp[MAX_PATH] = {0};
	StringCchCopy(strTemp, MAX_PATH, strPath);
	CString temp = strPath;
	temp.Delete(wcslen(temp) - 2, 2);
	int iIndex = (int)wcslen(temp);
	for(; iIndex >= 0 ; iIndex--)
	{
		if(temp[iIndex] == L'\\')
		{
			break;
		}
	}
	temp.Delete(iIndex + 1,(int)wcslen(temp) - iIndex +1 );
	//FormatPath(temp);
	StringCchCopy(strRootPath, MAX_PATH, temp);

	if(ApproveFolder(strPath, &hFileOut))
	{
		
		AfxMessageBox(_T("Duyệt file hoàn tất"), MB_ICONINFORMATION);
	}
	else
		AfxMessageBox(_T("Duyệt file thất bại"), MB_ICONSTOP);
	CloseHandle(hFileOut);
}
const TCHAR* CDuyetFile::FormatPath(const TCHAR *strPath)
{
	int iLengthPath = wcslen(strRootPath);
	const TCHAR* strTemp = strPath +iLengthPath;
	return strTemp;
	
}
bool CDuyetFile::ApproveFolder(const TCHAR *strPath, const HANDLE* hFile)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;

	hFind = FindFirstFile(strPath, &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Đường dẫn không hợp lệ"));
		return false;
	}

	while(FindNextFile(hFind, &FindFileData) != 0)
	{
		if(wcscmp(FindFileData.cFileName, _T(".")) != 0 && wcscmp(FindFileData.cFileName, _T("..")) != 0)
		{
			INFOFILE infoFile;
		
			TCHAR strFilePath[MAX_PATH];
			_tcscpy_s(strFilePath, strPath);
			int length = wcslen(strFilePath);
			strFilePath[length -1] = L'\0';
			wcscat_s(strFilePath, FindFileData.cFileName);

			_LARGE_INTEGER lInt;

			lInt.HighPart = FindFileData.ftLastWriteTime.dwHighDateTime;
			lInt.LowPart = FindFileData.ftLastWriteTime.dwLowDateTime;
			infoFile.llLastModified = lInt.QuadPart;

			infoFile.cTypeFile = 0;
			wcscpy_s(infoFile.strFileName, strFilePath);
			CString strTemp = infoFile.strFileName;
			
			
			
			lInt.HighPart = FindFileData.nFileSizeHigh;
			lInt.LowPart = FindFileData.nFileSizeLow;
			infoFile.llSize = lInt.QuadPart;

			if(GetFileAttributes(strFilePath) & FILE_ATTRIBUTE_DIRECTORY)
			{
				infoFile.cTypeFile = 1;
			
				WriteElementToFileText(infoFile, *hFile);
				wcscat_s(strFilePath,_T("\\*"));
				ApproveFolder(strFilePath, hFile);
			}
			else
			{
				WriteElementToFileText(infoFile, *hFile);
			
			}
		}
	}

	FindClose(hFind);
	return true;
}


void CDuyetFile::WriteElementToFileText(const INFOFILE& infoFile, const HANDLE &hFile)
{
	DWORD dwPtr = SetFilePointer( hFile, 0, NULL, FILE_END);
	
	TCHAR strTemp[MAX_PATH] = {0};
	
	
	const TCHAR* strPath = FormatPath(infoFile.strFileName);
	
	//Write file name.
	StringCchPrintf(strTemp, MAX_PATH, _T("%s%s"),strPath, _T("\r\n") );
	WriteFile(hFile, strTemp, wcslen(strTemp) * sizeof(TCHAR), &dwPtr, NULL);

	//Write type file.
	wmemset(strTemp, 0, MAX_PATH);
	StringCchPrintf(strTemp, MAX_PATH, _T("\r\t%d\r\n"), infoFile.cTypeFile);
	WriteFile(hFile, strTemp, wcslen(strTemp) * sizeof(TCHAR), &dwPtr, NULL);

	//Write file size.
	wmemset(strTemp, 0, MAX_PATH);
	StringCchPrintf(strTemp, MAX_PATH, _T("\r\t%I64u\r\n"), infoFile.llSize);
	WriteFile(hFile, strTemp, wcslen(strTemp) * sizeof(TCHAR), &dwPtr, NULL);
	
	//Write last modified.
	wmemset(strTemp, 0, MAX_PATH);
	StringCchPrintf(strTemp, MAX_PATH, _T("\r\t%I64u\r\n"), infoFile.llLastModified);
	WriteFile(hFile, strTemp, wcslen(strTemp) * sizeof(TCHAR), &dwPtr, NULL);
}


bool CDuyetFile::CompareTwoInfoFile(const INFOFILE* infoFileDest, const INFOFILE* infoFileSource)
{
	if(wcscmp(infoFileDest->strFileName, infoFileSource->strFileName) != 0)
		return false;
	if(infoFileDest->cTypeFile != infoFileSource->cTypeFile)
		return false;
	if(infoFileDest->llSize != infoFileSource->llSize)
		return false;
	if(infoFileDest->llLastModified != infoFileSource->llLastModified)
		return false;
	return true;
}
bool CDuyetFile::CompareTwoInfoFIle(const INFOFILE* infoFile)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA file ;
	hFind = FindFirstFile(infoFile->strFileName, &file);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		return false;
	}
	FindClose(hFind);
	_LARGE_INTEGER lInt;

	lInt.HighPart = file.ftLastWriteTime.dwHighDateTime;
	lInt.LowPart = file.ftLastWriteTime.dwLowDateTime;
	if(infoFile->llLastModified != lInt.QuadPart)
	{
		return false;
	}
	lInt.HighPart = file.nFileSizeHigh;
	lInt.LowPart = file.nFileSizeLow;
	if(infoFile->llSize != lInt.QuadPart)
	{
		return false;
	}

	return true;
}
void CDuyetFile::ApproveFolderToDownload(CClientDlg *pClientDlg, const TCHAR *strRootPath, const TCHAR* strFullPathBuild)
{
	
	CFileServices fileServices;
	SOCKET sClientSocket = INVALID_SOCKET;
	CDownloadFileSocket *pDownloadFileSocket = NULL;
	HANDLE hThread = NULL;

	FILE* ifs;
	
	_wfopen_s(&ifs, strFullPathBuild,L"rb");
	wstring strLine;
	int iCount = 0;
	vector<INFOFILE> lstInfoFile;
	__int64 iSumSize = 0;
	INFOFILE infoFile;
	
	while(getline(wifstream(ifs), strLine))
	{
		iCount++;
		CString strCLine(strLine.c_str());
		strCLine.Remove(L'\r');
		strCLine.Remove(L'\n');
		strCLine.Remove(L'\t');
		if(strCLine.GetAt(0) > 255)
			strCLine.Delete(0);
		switch(iCount)
		{
		case 1:
			StringCchPrintf(infoFile.strFileName, MAX_PATH,_T("%s\\%s"), strRootPath, strCLine);
			break;
		case 2:
			//StringCchCopy(infoFile, MAX_PATH, strCLine);
			if(strCLine.Compare(_T("1")) == 0)
				infoFile.cTypeFile = 1;
			else
				infoFile.cTypeFile = 0;
			break;
		case 3:
			infoFile.llSize = _wtoi64(strCLine.GetBuffer());
			break;
		case 4:
			infoFile.llLastModified = _wtoi64(strCLine.GetBuffer());
			iCount = 0;
			break;
		}
		if(iCount == 0)
		{
			if(!CompareTwoInfoFIle(&infoFile))
			{			
				
				if(infoFile.cTypeFile == 1)
				{
					fileServices.CustomCreateDirectory(infoFile.strFileName);
				}
				else
				{
					lstInfoFile.push_back(infoFile);
					iSumSize += infoFile.llSize;
				}
			}
		}

	}
	
	TCHAR strFolderDownload[MAX_PATH];
	StringCchPrintf(strFolderDownload, MAX_PATH, _T("%s\\%s"), strRootPath, pClientDlg->GetFolderDownload());
	if(lstInfoFile.empty())
	{
		fclose(ifs);
		int iIndexDownload = pClientDlg->GetCountItemDownload();
		pClientDlg->InsertItemDownload(iIndexDownload, strFolderDownload, iSumSize, 100);
		return;
	}
	int iIndexDownload = pClientDlg->GetCountItemDownload();
	pClientDlg->InsertItemDownload(iIndexDownload, strFolderDownload, iSumSize, 0);
	
	while(!lstInfoFile.empty())
	{
		
		CString strFileName(lstInfoFile[lstInfoFile.size() -1].strFileName);
		int iLengRootPath = wcslen(strRootPath);
		strFileName.Delete(0, iLengRootPath +1);

		sClientSocket = pClientDlg->CreateConnectSocket();
		
		pDownloadFileSocket = new CDownloadFileSocket(sClientSocket, pClientDlg, strRootPath, strFileName);
		pDownloadFileSocket->SetIndexDownload(iIndexDownload);
		hThread = CreateThread(NULL, 0,  CClientDlg::FileDownloadingThreadFunction, pDownloadFileSocket, CREATE_SUSPENDED, NULL);
		
		if (NULL != hThread) {
				pDownloadFileSocket->SetThreadHandle(hThread);
				pDownloadFileSocket->SetLastWriteModified(&lstInfoFile[lstInfoFile.size() -1].llLastModified);
				ResumeThread(hThread);
		} else {
			delete pDownloadFileSocket;
			closesocket(sClientSocket);
		}
		lstInfoFile.pop_back();
	}

	fclose(ifs);
}