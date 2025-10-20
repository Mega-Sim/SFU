//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpUniCastSender.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

UdpUniCastSender::UdpUniCastSender()
{

}

int UdpUniCastSender::UnicastSendMessage(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	char buf[MAX_PACKET_SIZE+1];
	int len;

	strcpy(buf, strMessage.c_str());
	len = strlen(buf);

	if(buf[len-1] == '\n')
		buf[len-1] = '\0';
	if(strlen(buf) == 0)
		return -1;

	return UnicastSendMessage(strIpAddr, iPort, buf, len);
}


int UdpUniCastSender::UnicastSendMessage(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	int retval;
	char cIpAddr[16];

	strcpy(cIpAddr, strIpAddr.c_str());	// String으로 들어온 주소를 변환

#if 0
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
#endif

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
		err_display("Broadcast Sender - Invalid_Socket");
		// err_quit("socket()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	// remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_addr.s_addr = inet_addr(cIpAddr);
	remoteaddr.sin_port = htons(iPort);

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
//	int addrlen;

	retval = sendto(sock, buf, len, 0,
		(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));

	if(retval == SOCKET_ERROR){
//		return -1;
		retval = -1;
		// err_display("Broadcast Sender Error - Send to()");
	}

	// ------------------------------------------
	//   이 부분은 송신 즉시 수신하는 경우, 필요하면 넣어서 쓰자
	//   단, 이 부분을 쓰려면 주의가 필요하다. 상대방이 응답하지 않는
	// 	 경우의 처리가 반드시 이루어져야 하며,
	//   이를 위해 논블럭 소켓과 타임아웃이 추가되어야 한다.
	// ------------------------------------------
		// 데이터 받기
	/*
		char Receive[MAX_PACKET_SIZE+1];
		int  iRtn = 0;
		AnsiString strRtnMsg;
		addrlen = sizeof(peeraddr);
		iRtn = recvfrom(sock, Receive, MAX_PACKET_SIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if(iRtn == SOCKET_ERROR){
			ShowMessage("[오류] recvfrom() Unicast 응답 오류");
		}

		// 송신자의 IP 주소 체크
		if(memcmp(&peeraddr, &remoteaddr, sizeof(peeraddr))){
			ShowMessage("[오류] 잘못된 상대방 주소가 왔습니다");
		}

		// 받은 데이터 출력
		buf[iRtn] = '\0';
		strRtnMsg.sprintf("[UDP 클라이언트] %d바이트를 받았습니다.", iRtn);
		ShowMessage(strRtnMsg);
	*/
	// ------------------------------------------

	closesocket(sock);

#if 0
	// 윈속 종료
	WSACleanup();
#endif

	return retval;
}


String UdpUniCastSender::err_display(char *msg)
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