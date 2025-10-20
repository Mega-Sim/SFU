//---------------------------------------------------------------------------

#ifndef UdpServerH
#define UdpServerH

#include <winsock2.h>

#ifdef IS_REAL_OHT
	#include "LogHandler.h"	// Log 작성 목적
#endif

class UdpServer
{
private:

#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
#endif

	SOCKET sock;
	bool bListenMode;

public:
	UdpServer();
	int  Bind(int iPort);
	int  Listen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);
	int  Close();
	String err_display(char *msg);
	bool ListenMode();
};
//---------------------------------------------------------------------------
#endif
