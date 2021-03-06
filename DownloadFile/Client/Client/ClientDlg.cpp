// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "Messages.h"
#include "ThreadFunction.h"
#include "threaddata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uiServerPort = 11251;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LST_DOWNLOAD, m_LstDownload);

	DDX_Text(pDX, IDC_EDT_SERVERFILENAME, m_strServerFileName);
	DDV_MaxChars(pDX, m_strServerFileName, 260);


	DDX_Text(pDX, IDC_EDT_SAVEPATH, m_strSavePath);
	DDV_MaxChars(pDX, m_strSavePath, 260);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CClientDlg::OnBnClickedBtnConnect)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_, &CClientDlg::OnBnClickedBtn)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	m_LstDownload.InitProgressColumn(2);
	m_LstDownload.InsertColumn(0,_T("Tên tập tin"), LVCFMT_LEFT, 100, 0);
	m_LstDownload.InsertColumn(1,_T("Kích thước"),LVCFMT_LEFT,100, 1);
	m_LstDownload.InsertColumn(2,_T("Trạng thái"),LVCFMT_CENTER,100, 2);
	
	
	
	
	m_LstDownload.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_BORDERSELECT | LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINECOLD);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CClientDlg::GetIPAddressString(char *pcBuffer, size_t stMaxLength) {

	if (stMaxLength < 16) {
		return false;
	}

	BYTE nField0;
	BYTE nField1;
	BYTE nField2;
	BYTE nField3;
	
	CIPAddressCtrl *pIPAddressCtrlServerAddress = (CIPAddressCtrl*)this->GetDlgItem(IDC_IPADDRESS_SERVER);
	int nNonBlankFieldsCount = pIPAddressCtrlServerAddress->GetAddress(nField0, nField1, nField2, nField3);
	if (nNonBlankFieldsCount < 4) {
		throw _T("Not to be allowed to leave any fields be blank");
	}
	sprintf_s(pcBuffer, stMaxLength, "%d.%d.%d.%d", nField0, nField1, nField2, nField3);
	return TRUE;
}
void CClientDlg::OnBnClickedBtnConnect()
{
	UpdateData(TRUE);
	DownloadFile();
}
void CClientDlg::DownloadFile()
{
	try {

		SOCKET sClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
		if (INVALID_SOCKET == sClientSocket) {
			throw _T("Unable to create socket");
		}

		SOCKADDR_IN serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(m_uiServerPort);

		
		char strServerIPAddress[16];
		GetIPAddressString(strServerIPAddress, sizeof(strServerIPAddress));
		serverAddress.sin_addr.s_addr = inet_addr(strServerIPAddress);

		if (0 != connect(sClientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) {
			throw _T("Unable to connect server");
		}

		THREAD_DATA *pThreadData = new THREAD_DATA;

		pThreadData->iIndex = m_LstDownload.GetItemCount();
		pThreadData->lstDownloadFile = &m_LstDownload;
		_tcscpy_s(pThreadData->strFileName, MAX_PATH, m_strServerFileName);
		_tcscpy_s(pThreadData->strPath, MAX_PATH, m_strSavePath);
		pThreadData->sConnectSocket = sClientSocket;

		

		HANDLE hThread = CreateThread(NULL, 0,  SocketCommunicationThreadFunction1, /*pThreadData*/this, NULL, NULL);

		if (NULL == hThread) {
			AfxMessageBox(_T("Unable to download this file. Please try later"), MB_ICONSTOP);
		}

	} catch (const TCHAR* message) {
		AfxMessageBox(message);
	}

}
void CClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CClientDlg::OnBnClickedBtn()
{
	BROWSEINFO browseInfo;
	ZeroMemory(&browseInfo, sizeof(BROWSEINFO));

	TCHAR strTitle[] = _T("Choosing a folder to save your file");
	browseInfo.lpszTitle = strTitle;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder(&browseInfo);

	if (NULL != pItemIDList) {

		TCHAR strPATH[MAX_PATH] = {0};

		if (FALSE != ::SHGetPathFromIDList(pItemIDList, strPATH)) {

			m_strSavePath = strPATH;
			UpdateData(FALSE);

		}

		::CoTaskMemFree(pItemIDList);
	}

}

DWORD WINAPI CClientDlg::SocketCommunicationThreadFunction1(LPVOID lpParam ) {

	CClientDlg *pThis = (CClientDlg*)lpParam;
	//CString  strText = pThis->m_strServerFileName;
	pThis->ProcessThreadFunc();
	return 0;
}

void CClientDlg::ProcessThreadFunc()
{
	CString  strText = m_strServerFileName;
}