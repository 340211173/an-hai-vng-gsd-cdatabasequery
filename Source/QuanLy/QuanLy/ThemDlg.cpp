// ThemDlg.cpp : implementation file
//

#pragma once
#include "stdafx.h"
#include "QuanLy.h"
#include "ThemDlg.h"
#include "MySqlPhongBanDAO.h"
#include "MySqlNhanVienDAO.h"
#include "MyException.h"
#pragma warning(disable: 4996)

// CThemDlg dialog

IMPLEMENT_DYNAMIC(CThemDlg, CDialog)

CThemDlg::CThemDlg(NHANVIEN* nhanVien /*=NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(CThemDlg::IDD, pParent)
{
	if(nhanVien != NULL)
	{
		m_strTenNhanVien =  nhanVien->strTenNhanVien;
		m_strPhongBan =  nhanVien->strTenPhongBan;
		m_strSoDienThoai = nhanVien->strSoDienThoai;
		m_strDiaChi = nhanVien->strDiaChi;
		m_iMaNhanVien = nhanVien->iMaNhanVien;
		
		if(nhanVien->cGioiTinh == 0)
		{
			m_strGioiTinh = _T("Nữ");
		}else
			m_strGioiTinh = _T("Nam");
		m_bIsEdit = true;
		m_iMaNhanVien = nhanVien->iMaNhanVien;
	}
	else
	{
		m_bIsEdit = false;
		m_iMaNhanVien = -1;
	}
}

CThemDlg::~CThemDlg()
{
}

void CThemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDT_HOTEN, m_strTenNhanVien);
	DDV_MaxChars(pDX, m_strTenNhanVien, MAX_NAME);

	DDX_CBString(pDX, IDC_CBB_PHONGBAN, m_strPhongBan);
	DDV_MaxChars(pDX, m_strPhongBan, MAX_NAME);

	DDX_CBString(pDX, IDC_CBB_GIOITINH, m_strGioiTinh);
	DDV_MaxChars(pDX, m_strGioiTinh, 3);

	DDX_Text(pDX, IDC_EDT_SODIENTHOAI, m_strSoDienThoai);
	DDV_MaxChars(pDX, m_strSoDienThoai, MAX_PHONE);

	DDX_Text(pDX, IDC_EDT_DIACHI, m_strDiaChi);
	DDV_MaxChars(pDX, m_strDiaChi, MAX_ADDRESS);
	
	DDX_Control(pDX, IDC_CBB_PHONGBAN, m_cbbPhongBan);
	DDX_Control(pDX, IDC_CBB_GIOITINH, m_cbbGioiTinh);

	DDX_Control(pDX, IDOK, m_btnOK);

}

BEGIN_MESSAGE_MAP(CThemDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CThemDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CThemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	int count = m_cbbGioiTinh.GetCount();
	GetAllListPhongBan();
	m_cbbGioiTinh.AddString(_T("Nữ"));
	m_cbbGioiTinh.AddString(_T("Nam"));
	m_cbbGioiTinh.SetCurSel(0);
	if(m_bIsEdit)
	{
		SetWindowText(_T("Thay đổi thông tin nhân viên"));
		if(wcscmp(m_strGioiTinh, _T("Nữ")) == 0)
		{
			m_cbbGioiTinh.SetCurSel(1);
		}
		else m_cbbGioiTinh.SetCurSel(0);
		m_btnOK.SetWindowTextW(_T("Thay đổi"));
		
	}
	
	return TRUE;
}
// CThemDlg message handlers

void CThemDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	CQuanLyApp* pApp = static_cast <CQuanLyApp*> ( AfxGetApp());

	try
	{
		CMySqlNhanVienDAO nhanVienDAO(pApp->ConnectString.m_strUsername,
									  pApp->ConnectString.m_strPasssword, 
									  pApp->ConnectString.m_strServerAddress,
									  pApp->ConnectString.m_strDatabaseName);

		bool bState = false;
		if(m_bIsEdit)
		{
			bState = nhanVienDAO.UpdateNhanVien(InitNhanVien(), GetMaPhongBan());
		}
		else
		{
			bState = nhanVienDAO.AddNhanVien(InitNhanVien(), GetMaPhongBan());
		}
		if(bState)
		{
			OnOK();
		}
	}catch (CMyException *pException) {
		if (NULL != pException) {
			AfxMessageBox(pException->GetMessage());
		}
	}
}

void CThemDlg::GetAllListPhongBan()
{
	
	CQuanLyApp* pApp = static_cast <CQuanLyApp*> ( AfxGetApp());
	try
	{
		CMySqlPhongBanDAO phongBanDAO(pApp->ConnectString.m_strUsername,
									  pApp->ConnectString.m_strPasssword, 
									  pApp->ConnectString.m_strServerAddress,
									  pApp->ConnectString.m_strDatabaseName);
		phongBanDAO.GetAll(&m_listPhongBan);
		list<PHONGBAN>::iterator it;
		for(it = m_listPhongBan.begin(); it != m_listPhongBan.end(); it++)
		{
			m_cbbPhongBan.AddString(it->strTenPhongBan);
			if(m_bIsEdit && wcscmp(it->strTenPhongBan, m_strPhongBan) == 0)
			{
				m_cbbPhongBan.SetCurSel(m_cbbPhongBan.GetCount() - 1);
			}
		}
		if(!m_bIsEdit)
			m_cbbPhongBan.SetCurSel(0);
	}catch (CMyException *pException) {
		if (NULL != pException) {
			AfxMessageBox(pException->GetMessage());
		}
	}
	return;
}
NHANVIEN CThemDlg::InitNhanVien()
{
	UpdateData(TRUE);
	NHANVIEN nhanVien;
	_tcscpy(nhanVien.strTenNhanVien, m_strTenNhanVien);
	_tcscpy(nhanVien.strTenPhongBan, m_strPhongBan);
	_tcscpy(nhanVien.strSoDienThoai, m_strSoDienThoai);
	_tcscpy(nhanVien.strDiaChi, m_strDiaChi);
	nhanVien.iMaNhanVien = m_iMaNhanVien;
	if(m_strGioiTinh == _T("Nam"))
	{
		nhanVien.cGioiTinh = 1;
	}
	else
		nhanVien.cGioiTinh = 0;
	return nhanVien;
}
void CThemDlg::AddNhanVien()
{
	/*
	CMySqlNhanVienDAO NhanVienDAO("","","","");
	if(NhanVienDAO.AddNhanVien(InitNhanVien()))
	{
		OnOK();
	}
	*/
}
void CThemDlg::UpdateNhanVien()
{
	/*
	CMySqlNhanVienDAO NhanVienDAO("","","","");
	if(NhanVienDAO.UpdateNhanVien(InitNhanVien()))
	{
		OnOK();
	}
	*/
}
int CThemDlg::GetMaPhongBan()
{
	CString strPhongBan = _T("");
	m_cbbPhongBan.GetLBText(m_cbbPhongBan.GetCurSel(), strPhongBan);
	list<PHONGBAN>::iterator it;
	for(it = m_listPhongBan.begin(); it != m_listPhongBan.end(); it++)
	{
		if( wcscmp(it->strTenPhongBan, strPhongBan) == 0)
		{
			return it->iMaPhongBan;
		}
	}
	return -1;
}