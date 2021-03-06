#include "stdafx.h"
#include "ThreadFunction.h"
#include "Messages.h"
#include "MessageHandler.h"
#include "Server.h"
DWORD WINAPI SocketAcceptThreadFunction( LPVOID lpParam ) {

	SOCKET *pSocket = (SOCKET*)lpParam ;
	SOCKET sListenSocket = *pSocket;
	SOCKET *psConnectSocket = NULL;

	while (true) {

		SOCKET sConnectSocket = INVALID_SOCKET;
		sConnectSocket = accept(sListenSocket, NULL, NULL);

		if (INVALID_SOCKET != sConnectSocket) {

		//	SOCKET *psConnectSocket = new SOCKET;
		//	*psConnectSocket = sConnectSocket;

			HANDLE hThread = CreateThread(NULL, 0,  SocketCommunicationThreadFunction, (LPVOID)sConnectSocket, NULL, NULL);
		}
	}
	return 0;
}

DWORD WINAPI SocketCommunicationThreadFunction(LPVOID lpParam ) {

//	SOCKET *pSocket = (SOCKET*)lpParam ;
//	SOCKET sConnectSocket = *pSocket;
//	delete pSocket;

	SOCKET sConnectSocket = (SOCKET)lpParam;

	while (true) {

		MESSAGE_HEADER messageHeader;
		ZeroMemory(&messageHeader, sizeof(messageHeader));
		int iLastError = 0;
		
		int iBytesReceived = recv(sConnectSocket, (char*)&messageHeader, sizeof(MESSAGE_HEADER), 0);
		if (iBytesReceived != sizeof(MESSAGE_HEADER)) {
			return HandleError(sConnectSocket);
		}
		CServerApp* cApp = static_cast<CServerApp*>(AfxGetApp());
		switch(messageHeader.iType) {
			case DOWNLOAD_FILE:
				if (HandleDownloadFileMessage(sConnectSocket,cApp->m_strRootPath, &iLastError) == false) {
					::OutputDebugString(cApp->m_strRootPath);
					return iLastError;
				}
				break;
		}
	}
	return 0;
}