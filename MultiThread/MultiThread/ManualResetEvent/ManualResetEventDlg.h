
// ManualResetEventDlg.h : header file
//

#pragma once


// CManualResetEventDlg dialog
class CManualResetEventDlg : public CDialog
{
// Construction
public:
	CManualResetEventDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_MANUALRESETEVENT_DIALOG };

	static DWORD WINAPI UpdateListThreadFunction(PVOID pvParam);
	static DWORD WINAPI HouseBuyingThreadFunction(PVOID pvParam);
	static DWORD WINAPI CarBuyingThreadFunction(PVOID pvParam);
protected:
	HICON m_hIcon;
	CListBox *m_list;

	HANDLE m_hEvent;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
