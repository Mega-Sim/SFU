//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpBroadCastSender.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

UdpBroadCastSender::UdpBroadCastSender()
{
	strBroadCastMask = BROADCAST_IP;
}

void UdpBroadCastSender::SetMask(AnsiString strMask)
{
	strBroadCastMask = strMask;
}

int UdpBroadCastSender::BroadcastSendMessage(int iPort, AnsiString strMessage)
{
	char buf[MAX_PACKET_SIZE+1];
	int len;

	strcpy(buf, strMessage.c_str());
	len = strlen(buf);

	if(buf[len-1] == '\n')
		buf[len-1] = '\0';
	if(strlen(buf) == 0)
		return -1;

	return BroadcastSendMessage(iPort, buf, len);
}

int UdpBroadCastSender::BroadcastSendMessage(int iPort, char* buf, int len)
{
	int retval;
//	char cIpAddr[16];

#if 0
	// 윈속 초기화
	// sock_err
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
#endif

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
		err_display("Broadcast Sender - Invalid_Socket");
		// err_quit("socket()");

	// 브로드캐스팅 활성화
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char *)&bEnable, sizeof(bEnable));
	if(retval == SOCKET_ERROR)
		err_display("Broadcast Sender Error");
		// err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(strBroadCastMask.c_str());
	// remoteaddr.sin_addr.s_addr = inet_addr(cIpAddr);
	remoteaddr.sin_port = htons(iPort);

	retval = sendto(sock, buf, len, 0,
		(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));

	if(retval == SOCKET_ERROR){
		return -1;
	}

	closesocket(sock);

#if 0
	// 윈속 종료
	WSACleanup();
#endif

	return retval;
}

String UdpBroadCastSender::err_display(char *msg)
{
	AnsiString strMsg;

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	//strMsg.printf()
	strMsg.sprintf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);

	return strMsg;
}