
// ManualResetEventDlg.h : header file
//

#pragma once

#include "library/include/MyEvent.h"
#pragma comment (lib, "library/lib/synchronization.lib")

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
	CMyEvent m_myEvent;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
