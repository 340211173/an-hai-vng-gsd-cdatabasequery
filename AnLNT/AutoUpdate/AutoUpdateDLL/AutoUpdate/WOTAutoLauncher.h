#pragma once
#include "launcher.h"

class CWOTAutoLauncher :
	public CLauncher
{
protected:

	HANDLE m_hProcess;
	FILETIME m_ftConfigFileLastWriteTime;
	CString m_strConfigFilePath;
	CString m_strLogFilePath;
	CString m_strTempUpdateDirectoryPath;

	int m_iDelayTime;
	BOOL m_bIsFailed;

public:
	CWOTAutoLauncher(LPCTSTR strSource);
	virtual ~CWOTAutoLauncher(void);

	virtual CString GetName();
	virtual BOOL Run();

	virtual CGameSourceCompare * GetComparer (LPCTSTR strNewSource, LPCTSTR strOldSource);
protected:
	
	virtual BOOL StartLauncherProcess();
	BOOL GetConfigFileLastWriteTime();
	BOOL WaitForUpdateBeginning();
	BOOL WaitForComplate();

	BOOL ProcessError();
	BOOL KillProcess();
	BOOL ProcessComplete();

};
