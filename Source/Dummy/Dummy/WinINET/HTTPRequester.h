#pragma once


class CHTTPRequester
{

protected:

	HINTERNET m_hSession;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;

	CString m_strHeader;
	CString m_strResponse;

public:

	CHTTPRequester(void);
	~CHTTPRequester(void);

	BOOL PostRequest(LPCTSTR strServer, LPCTSTR strObjectName, LPVOID buffer, size_t uiBufferSize);
	BOOL PostRequest(LPCTSTR strServer, LPCTSTR strObjectName, LPCTSTR strData);
	BOOL PostRequest(LPCTSTR strURL, LPCTSTR strData);

	CString GetResponsedString();

protected:

	void Initalize();
	void CleanUp();
	BOOL ReceiveResponsedString();
};
