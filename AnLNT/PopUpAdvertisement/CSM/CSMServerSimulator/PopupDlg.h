#pragma once


// CPopupDlg dialog

class CPopupDlg : public CDialog
{
	DECLARE_DYNAMIC(CPopupDlg)

public:
	CPopupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPopupDlg();

// Dialog Data
	enum { IDD = IDD_POPUPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};