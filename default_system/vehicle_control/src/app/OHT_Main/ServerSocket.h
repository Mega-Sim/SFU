/**
@file    ServerSocket.h
@version OHT 7.0
@brief   ServerSocket Class Header File
*/
//---------------------------------------------------------------------------
#ifndef ServerSocketH
#define ServerSocketH
//---------------------------------------------------------------------------

#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#include "List.h"

#pragma comment(lib,"ws2_32.lib")
#define SOCKETERROR -1
#define INVALID_EVENT 258
#define BUF_SIZE	8192
#include "LogHandler.h"	// Log 작성 목적

//extern void CALLBACK CompRoutine(unsigned long, unsigned long, LPWSAOVERLAPPED, unsigned long);


//외부에서 설정하여 아무나 쓸수있도록 설정.
typedef struct _CLIENT_DATA_  //여러 클라이언트가 접속할 경우, 각 클라이언트에서 받은 정보 및 데이터를 리스트에 저장하기 위해.
{

   SOCKET hRcvSock;   //SocketInfo
   char buf[BUF_SIZE];    //Data 내용
   int  Length;		  //Data 길이
} CLIENT_DATA, *LPCLIENT_DATA ;

//리스트에 저장될 클라이언트 구조체 : 2014.11.12 doori.shin
typedef struct _CLIENT_SOCK
{
	SOCKET hClientSock;
	char szIPAddress[20];
}CLIENT_SOCK;

/**
@class   ServerSocket
@date    2013.05.03
@author  puting
@brief   ServerSocket 관련 Class
@note    Socket 통신으로 Data를 읽어들인다.
*/


class ServerSocket //: public ThreadCtrl
{

private:
	LogUnit *pLogTCP;      ///< TCP 로그 생성을 위한 변수
	SOCKET	hServSock; ///<서버 소켓 정보 저장

	WSAEVENT EventArray[10]; //Max Client 갯수

	int EventTotal;
	int nPortNum;

	SOCKADDR_IN m_CurClientInfo;

	CLIENT_DATA *m_clientData;                   ///<Client로 받은 Data
	LPCLIENT_DATA m_TempclientData;              ///<임시 Client로 받은 Data

	//char* pBuf;
	LList* m_ClientList;  						///<최고 10개의 Client 관리
	LList* m_PacketList;                        ///<최고 10개의 Receive Data 관리

	CRITICAL_SECTION	m_CS;

	int CreateSocketInfo(SOCKET socket);
	int CreateSocketInfo(SOCKET socket, char* ip);
	void freeSocketInfo(int eventIndex);
	int nClientCount;

public:
	ServerSocket();
	~ServerSocket();

	bool SocketOpen(int PortNum);
	bool SocketClose();

	void Update();         // 쓰레드안에 넣으면 됨

	bool Read(char* RecvBuf, int* Length, int* Count); //읽은 데이터의 정보를 얻기
	bool Write(char* SendBuf, int Length, UINT Index);

	//int GetIndex(void* pCmd);  //index 정보를 외부에 주기 위해 사용

	bool IsClientConnect();
	bool IsClientDisConnect();
	bool IsClientReadData();

	UINT GetPacketCount();
	UINT GetClientCount();

	void ClientRemove(UINT Index);
	SOCKET  GetClientInfo(UINT Index);

//	char* GetCurIPAddress();                      //미사용
//	char* GetIPAddress(UINT index);               //미사용

	//void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

	WSANETWORKEVENTS networkEvents; ///<WaitforEvent에 넣기 위한 이벤트

	char* BufferPointer;   //외부에서 사용하기위하여
	int nRestLenth;        //외부에서 사용하기위하여


};
#endif
