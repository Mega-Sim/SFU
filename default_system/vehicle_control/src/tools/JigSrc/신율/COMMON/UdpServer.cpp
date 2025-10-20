//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpServer.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif


/*
SOCKET sock;
bool bListenMode;
*/

UdpServer::UdpServer()
{
#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPServer.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif

	bListenMode = false;
}

// 리턴값은 현재 듣기중인 상태인지를 나타냄
// true면 듣기 가능함
bool UdpServer::ListenMode()
{
	return bListenMode;
}

// bind를 시도하고, 성공하면 true를 리턴한다
int UdpServer::Bind(int iPort)
{
	int retval;

	if (bListenMode == true)
		return -3;	// 이미 접속되어 있는 상태니까 바로 리턴한다.

#if 0
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
#endif

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
		return -1;

	// 논블로킹
	u_long on = 1;
	retval = ioctlsocket(sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR)
		return -2;

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(iPort);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR)
		return -2;

	bListenMode = true;		// 데이터 수신 가능 상태

	return retval;
}

// 소켓에 들어온 값이 있나 확인하고, 있으면 들어온 바이트수를 리턴한다.
// 주의 : UDP이므로, 한번에 하나의 패킷만 꺼내준다.
//        따라서 한번 읽었다 해서, 모든 내용을 꺼낸것은 아니다. (버퍼가 있다)
int  UdpServer::Listen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	AnsiString tmpA;
	//char buf[MAX_PACKET_SIZE+1];

	// 데이터 받기
	addrlen = sizeof(clientaddr);
	retval = recvfrom(sock, buf, MAX_BIG_PACKET_SIZE, 0,
		(SOCKADDR *)&clientaddr, &addrlen);
	if(retval == SOCKET_ERROR){
		iLen = 0; 	// 수신한 byte수는 0입니다.
		return -2;
	}
	else
	{
		iLen = retval;
	}

	// 상위에서 받은 데이터 출력할때 깔끔하게 표시되도록 함
   	buf[retval] = '\0';

   //	Sleep(70);
	// clientaddr.sin_addr.S_un.S_un_b.s_b1
	tmpA.sprintf("%s", inet_ntoa(clientaddr.sin_addr));
	strClientAddr = tmpA;

//	ADD_LOG("Listen Succec %s",tmpA);
	iClientPort = ntohs(clientaddr.sin_port);

	// ------------------------------------------
	//   이 부분은 수신 즉시 송신하는 경우, 필요하면 넣어서 쓰자
	//   단, 이 부분을 쓰려면 주의가 필요하다. 상대방이 응답하지 않는
	// 	 경우의 처리가 반드시 이루어져야 하며,
	//   이를 위해 논블럭 소켓과 타임아웃이 추가되어야 한다.
	// ------------------------------------------
	/*
	retval = sendto(sock, buf, retval, 0,
		(SOCKADDR *)&clientaddr, sizeof(clientaddr));

	if(retval == SOCKET_ERROR){
		return -1;
	}
	*/

	return retval;
}

int  UdpServer::Close()
{
	// closesocket()
	closesocket(sock);

#if 0
	// 윈속 종료
	WSACleanup();
#endif

	bListenMode = false;

	return 0;
}

String UdpServer::err_display(char *msg)
{
	AnsiString strMsg;

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	strMsg.sprintf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);

	return strMsg;
}