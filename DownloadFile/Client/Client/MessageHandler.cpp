#include "StdAfx.h"
#include "MessageHandler.h"

int HandleError(SOCKET socket) {

	int iError = WSAGetLastError();

	shutdown(socket, SD_BOTH);
	closesocket(socket);

	return iError;
}