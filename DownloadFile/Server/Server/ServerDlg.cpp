// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "ThreadFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerDlg dialog




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uiPort = 11251;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_PATH, m_edtPath);
	CServerApp* cApp = static_cast<CServerApp*>(AfxGetApp());
	DDX_Text(pDX, IDC_EDT_PATH, cApp->m_strRootPath);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CServerDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_OK, &CServerDlg::OnBnClickedBtnOk)
END_MESSAGE_MAP()


BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	InitListener();
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
	HANDLE hThread = ::CreateThread(NULL, 0,  SocketAcceptThreadFunction, &m_sListener, NULL, &dwThreadID);

	if (0 == hThread) {
		AfxMessageBox(_T("Unable to listen imcoming connection"), MB_ICONSTOP);
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

			m_edtPath.SetWindowText(strPATH);
			//m_edtOutput->SetWindowText(strPATH);
		}

		::CoTaskMemFree(pItemIDList);
	}

}

void CServerDlg::OnBnClickedBtnOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	AfxMessageBox(_T("Xác Nhận Thành Công"), MB_ICONINFORMATION);
}
