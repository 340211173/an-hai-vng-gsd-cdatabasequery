
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "ConnectSocket.h"
#include "DuyetFile.h"
#include "FileServices.h"
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment (lib, "Crypto/lib/Cryptography.lib")
#include "Crypto/Encryption.h"

// CServerDlg dialog




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uiPort = 11250;
	m_sListener = INVALID_SOCKET;
	m_hListenThread = NULL;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CServerDlg::OnBnClickedBtnBrowse)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	InitListener();

	m_edtPath = (CEdit*)GetDlgItem(IDC_EDT_PATH);

	m_strRootFolder = _T("D:\\Public");
	m_edtPath->SetWindowText(m_strRootFolder);

	return TRUE; 
}


void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CServerDlg::InitListener() {

	m_sListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sListener) {
		AfxMessageBox(_T("Unable to create socket"));
		return FALSE;
	}

	SOCKADDR_IN serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(m_uiPort);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(m_sListener, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) {
		AfxMessageBox(_T("Unable to bind"));
		return FALSE;
	}

	if (SOCKET_ERROR == listen(m_sListener, 5)) {
		AfxMessageBox(_T("Unable to listen"));
		return FALSE;
	}

	DWORD dwThreadID = 0;
	m_hListenThread = ::CreateThread(NULL, 0, SocketListeningThreadFunction, this, NULL, &dwThreadID);

	if (0 == m_hListenThread) {
		AfxMessageBox(_T("Unable to listen imcoming connection"), MB_ICONSTOP);
	} 
	return TRUE;
}
void CServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_sListener != INVALID_SOCKET) {
		closesocket(m_sListener);
		m_sListener = INVALID_SOCKET;
	}

	if (m_hListenThread != NULL) {
		CloseHandle(m_hListenThread);
	}
}

DWORD WINAPI CServerDlg::SocketListeningThreadFunction(LPVOID lpParam ) {

	CServerDlg *pServerDlg = (CServerDlg *)lpParam;

	while (pServerDlg->m_sListener != INVALID_SOCKET) {

		SOCKET sConnectSocket = INVALID_SOCKET;
		sConnectSocket = accept(pServerDlg->m_sListener, NULL, NULL);
		

		if (INVALID_SOCKET != sConnectSocket) {
			
			CConnectSocket *pConnectSocket = new CConnectSocket(sConnectSocket, pServerDlg);
			HANDLE hThread = CreateThread(NULL, 0,  FolderDownloadingThreadFunction, pConnectSocket, CREATE_SUSPENDED, NULL);

			if (NULL != hThread) {

				pConnectSocket->SetThreadHandle(hThread);
				ResumeThread(hThread);

			} else {
				delete pConnectSocket;
			}
		}
	}
	::OutputDebugStringA("thoat ra thread listen\n");
	return 0;
}

DWORD WINAPI CServerDlg::FolderDownloadingThreadFunction(LPVOID lpParam ) {

	CConnectSocket *pConnectSocket = (CConnectSocket *)lpParam;
	CServerDlg *pServerDlg = pConnectSocket->GetDlg();

	int iMessageType;
	int uiLength;

	if (pConnectSocket->GetMessageHeader(&iMessageType, &uiLength) == FALSE) {
		return pConnectSocket->Destroy();
	}

	switch (iMessageType) {

		case FILE_LIST_REQUEST:

			pServerDlg ->ProcessFileListRequest(pConnectSocket, uiLength);
			break;

		case DOWNLOAD_FILE:

			pServerDlg ->ProcessDownloadFile(pConnectSocket, uiLength);
			break;
	}

	return pConnectSocket->Destroy();
}



BOOL CServerDlg::ProcessFileListRequest( CConnectSocket* pConnectSocket, int uiLength )
{

	const TCHAR * strFolderName = pConnectSocket->RecevieFileName(uiLength);
	if (strFolderName == NULL) {
		return FALSE;
	}

	CFileServices fileServices;

	TCHAR strFullFolderPath[MAX_PATH];
	fileServices.CreateFullPath(strFullFolderPath, MAX_PATH, m_strRootFolder.GetBuffer(), strFolderName);

	TCHAR strFilesListFilePath[MAX_PATH];
	fileServices.CreateFilesListFilePath(strFilesListFilePath, MAX_PATH, m_strRootFolder.GetBuffer());
	
	CDuyetFile duyetFile;
	duyetFile.ApproveFolderToList(strFullFolderPath, strFilesListFilePath);

	TCHAR strEncryptedFilesListFilePath[MAX_PATH];
	StringCchPrintf(strEncryptedFilesListFilePath, MAX_PATH, _T("%s%s"), strFilesListFilePath, _T("Encrypted.ini"));

	CEncryption encryption;
	encryption.ExecuteFile(strFilesListFilePath, strEncryptedFilesListFilePath);

	HANDLE hFile = ::CreateFile(strEncryptedFilesListFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	BOOL isSendingFileSuccessful = pConnectSocket->SendFile(hFile);

	if (FALSE == isSendingFileSuccessful) {
		return FALSE;
	}

	::DeleteFile(strFilesListFilePath);
	::DeleteFile(strEncryptedFilesListFilePath);
	return TRUE;
}

BOOL CServerDlg::ProcessDownloadFile( CConnectSocket* pConnectSocket, int uiLength )
{
	const TCHAR * strFileName = pConnectSocket->RecevieFileName(uiLength);
	if (strFileName == NULL) {
		return FALSE;
	}

	TCHAR strFilePath[MAX_PATH];
	CFileServices fileServices;
	fileServices.CreateFullPath(strFilePath, MAX_PATH, m_strRootFolder.GetBuffer(), strFileName);

	HANDLE hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	BOOL isSendingFileSuccessful = pConnectSocket->SendFile(hFile);
	
	if (FALSE == isSendingFileSuccessful) {
		return FALSE;
	}
	return TRUE;
}


void CServerDlg::OnBnClickedBtnBrowse()
{
	BROWSEINFO browseInfo;
	ZeroMemory(&browseInfo, sizeof(BROWSEINFO));

	TCHAR strTitle[] = _T("Choosing a folder to save your file");
	browseInfo.lpszTitle = strTitle;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&browseInfo);

	if (NULL != pItemIDList) {

		TCHAR strPATH[MAX_PATH] = {0};

		if (FALSE != ::SHGetPathFromIDList(pItemIDList, strPATH)) {

			m_strRootFolder = strPATH;
			m_edtPath->SetWindowText(m_strRootFolder);

		}

		::CoTaskMemFree(pItemIDList);
	}
}
