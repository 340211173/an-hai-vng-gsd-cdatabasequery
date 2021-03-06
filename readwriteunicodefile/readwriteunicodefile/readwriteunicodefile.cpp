// readwriteunicodefile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "readwriteunicodefile.h"
#include "fstream"
#include <iostream>
#include "Strsafe.h"


#include <string>


using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
void WriteToFile();
void ReadToFile();
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		//WriteToFile();
		ReadToFile();
	}

	return nRetCode;
}
void WriteToFile()
{
	TCHAR strPath[MAX_PATH] = {0};

	GetCurrentDirectory(MAX_PATH, strPath);
	//StringCchCat(strPath, MAX_PATH, _T("\\danhsach.txt"));

	
	::GetModuleFileName(NULL, strPath, MAX_PATH);
	
	::PathRemoveFileSpec(strPath);
	::StrCat(strPath, _T("\\App.ini"));
	
	LPTSTR pszSectionB = _T("Hôm nay là thứ 5:"); // section name with bracket 
	LPTSTR pszSection = _T("\r\n\r\tNgày mai?"); // section name without bracket
	LPTSTR pszKey = _T("String");
	
	
	if(true)
	{
	
		// UTF16-LE BOM(FFFE)
		WORD wBOM = 0xFEFF;
		DWORD NumberOfBytesWritten;

		HANDLE hFile = ::CreateFile(strPath, GENERIC_WRITE, 0, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		::WriteFile(hFile, &wBOM, sizeof(WORD), &NumberOfBytesWritten, NULL);
		::WriteFile(hFile, pszSectionB, (_tcslen(pszSectionB)+1)*(sizeof(TCHAR) ), &NumberOfBytesWritten, NULL);
		::WriteFile(hFile, pszSection, ( _tcslen(pszSection))*(sizeof(TCHAR)), &NumberOfBytesWritten, NULL);	
		::CloseHandle(hFile);
	}
	
	//WritePrivateProfileString(pszSection, pszKey, _T("dfk"), strPath);
	
}

void ReadToFile()
{
	TCHAR strPath[MAX_PATH] = {0};

	GetCurrentDirectory(MAX_PATH, strPath);
	//StringCchCat(strPath, MAX_PATH, _T("\\danhsach.txt"));

	::GetModuleFileName(NULL, strPath, MAX_PATH);
	
	::PathRemoveFileSpec(strPath);
	::StrCat(strPath, _T("\\App.ini"));

	
	__int64 i;
	wifstream ifs(_wfopen(strPath,L"rb"));
	
	/*FILE* ifs;
	_wfopen_s(&ifs, strPath,L"rb");
	wstring line;
	getline(wifstream(ifs), line);*/

	wstring line_contents;
	
	//CString line;
	while(getline(ifs, line_contents))
	{
		//wcout << line_contents << L'\n';
		//TCHAR* s = NULL;
		CString s(line_contents.c_str());
		s.Remove(L'\r');
		s.Remove(L'\n');
		s.Remove(L'\t');
		//sscanf(<static_cast>(char*)line_contents.c_str(), "%I64u", &i );
		//StringCchPrintf(s.GetBuffer(), "%I64u", &i );
		
		s.Trim();
		s.Delete(0,1);
		line_contents.erase(1, 0);
		
		__int64 er = _wtoi64(s.GetBuffer());
	}
	
	//FILE *infile;
 //   wchar_t line_buffer[MAX_PATH]; /* BUFSIZ is defined if you include stdio.h */
 //
 //   //infile = _wfopen(strPath,L"rb");
	//infile = _wfopen(strPath,L"rt+,ccs=UNICODE");
 //   if (!infile) {
 //       wprintf(L"\nfile '%s' not found\n", strPath);
 //       return;
 //   }
 //   wprintf(L"\n%s\n\n", strPath);
 //
 //   /*Reading data from file into temporary buffer*/
	//
	//wmemset(line_buffer, 0, 260);
 //   while (fread(line_buffer,100,13,infile)) {
 //       /* note that the newline is in the buffer */
 //       if ( wcscmp ( L"ب" , line_buffer ) == 0 ){
 //            wprintf(L"YES!\n");
 //       }else{
 //            wprintf(L"NO!\n", line_buffer);
 //       }
 //   }
	// fclose(infile);


	/*CStringArray aLineArray;
	CString sLine;
	int iLineCount = 0;      

	CFileException ex;
	CStdioFile StdFile;

	if(!StdFile.Open(strPath, CFile::modeNoTruncate | CFile::modeRead | CFile::typeText, &ex))
	  {
		cout << (LPCSTR) "error" << endl;
	  }else
	  {
		while (StdFile.ReadString(sLine))
		{
			aLineArray.Add(sLine);
			iLineCount++;
		 }    
		StdFile.Close();
	  }
	    
	  CString temp;
	  int si = aLineArray.GetSize();*/
}