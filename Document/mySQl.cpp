// mySQl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mySQl.h"
#include "MyException.h"
#include "MySQLDataAccessHelper.h"
#include "MySQLPhongBanDAO.h"
#include "PhongBanDAO.h"
#include "NhanVienDAO.h"
#include "MySQLNhanVienDAO.h"

#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

CWinApp theApp;

using namespace std;


int divide(int x, int y) {
	if (y == 0) {
		//throw new CMyException(_T("hôm nay mẹ vắng nhà"), 1);
		throw new CMyException("divide by zero", 1);
	}
	return x/y;
}

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
		
		try {

			char strUser[] = "root";
			char strPassword[] = "hai";
			char strServerAddress[] = "localhost";
			char strDatabase[] = "quanlynhanvien";

			//0.Test check user

	/*		if (CMySQLDataAccessHelper::CheckUser(strUser, strPassword, strServerAddress, strDatabase)) {
				AfxMessageBox(_T("User hợp lệ"), MB_ICONINFORMATION);
			} else {
				AfxMessageBox(_T("User không hợp lệ"), MB_ICONSTOP);
			}*/

			//1. Test Lay Danh Sach Phong Ban
			//CPhongBanDAO *pPhongBanDAO = new CMySqlPhongBanDAO(strUser, strPassword, strServerAddress, strDatabase); 
			//list<PHONGBAN> lstPhongBan;
			//pPhongBanDAO->GetAll(&lstPhongBan);
			//delete pPhongBanDAO;


			//2. Test lay danh sach nhan vien
			CNhanVienDAO *pNhanVienDAO = new CMySqlNhanVienDAO(strUser, strPassword, strServerAddress, strDatabase); 
			list<NHANVIEN> lstNhanVien;
			pNhanVienDAO->GetAll(&lstNhanVien);
			delete pNhanVienDAO;
		

			//3. Test them 1 nhan vien
			//NHANVIEN nhanVien;
			//_tcscpy_s(nhanVien.strDiaChi, MAX_ADDRESS + 1, _T("227 Nguyễn Văn Cừ, Phường 7, Quận 5"));
			//_tcscpy_s(nhanVien.strTenNhanVien, MAX_NAME + 1, _T("Nguyễn Văn Bé"));
			//_tcscpy_s(nhanVien.strSoDienThoai, MAX_PHONE + 1, _T("0979 657 228"));
			//nhanVien.cGioiTinh = 1;
			//CNhanVienDAO *pNhanVienDAO = new CMySqlNhanVienDAO(strUser, strPassword, strServerAddress, strDatabase); 
			//pNhanVienDAO->AddNhanVien(nhanVien, 2);
			//delete pNhanVienDAO;

			//4. Test xoa một nhan vien
			//int iMaNhanVien = 74;
			//CNhanVienDAO *pNhanVienDAO = new CMySqlNhanVienDAO(strUser, strPassword, strServerAddress, strDatabase); 
			//pNhanVienDAO->DeleteNhanVien(iMaNhanVien);
			//delete pNhanVienDAO;

			//5.Test update mot nhanvien
			//NHANVIEN nhanVien;

			//nhanVien.iMaNhanVien = 7;
			//_tcscpy_s(nhanVien.strDiaChi, MAX_ADDRESS + 1, _T("3/4 Dạ Nam, Phường 2, Quận 8"));
			//_tcscpy_s(nhanVien.strTenNhanVien, MAX_NAME + 1, _T("Trần Quốc Thảo"));
			//_tcscpy_s(nhanVien.strSoDienThoai, MAX_PHONE + 1, _T("3344556677"));
			//nhanVien.cGioiTinh = 1;
			//int iMaPhong = 2;
			//
			//CNhanVienDAO *pNhanVienDAO = new CMySqlNhanVienDAO(strUser, strPassword, strServerAddress, strDatabase); 
			//pNhanVienDAO->UpdateNhanVien(nhanVien, iMaPhong);
			//delete pNhanVienDAO;

			//AfxMessageBox(_T("Thành Công"), MB_ICONINFORMATION);
		} catch (CMyException *pException ) {
			if (NULL != pException) {
				int t = pException->GetCode();
				AfxMessageBox(pException->GetMessage(), MB_ICONSTOP);
				delete pException;
			}
		}

		
	
	}

	return nRetCode;
}
