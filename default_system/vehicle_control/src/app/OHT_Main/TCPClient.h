//---------------------------------------------------------------------------

#ifndef TCPClientH
#define TCPClientH
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>
#include <math.h>
#include <vcl.h>
#include <stdlib.h>
#include <stdio.h>

#include "List.h"

#pragma comment(lib,"ws2_32.lib")
#define SOCKETERROR -1
#define BUF_SIZE	8192
#define INVALID_EVENT 258

typedef struct PER_IO_DATA_
{
  //	LPWSAOVERLAPPED overlapped;
 //	WSABUF	wsaBuf;
	char buffer[BUF_SIZE];
	int Length;
}PER_IO_DATA, *LPPER_IO_DATA;

class TCPCLIENT
{

   private:
		SOCKET	hSocket;
		SOCKADDR_IN	ServAddr;
		WSAEVENT	NewEvent;

		bool m_bConnected;
		//bool bConnectChk;

		PER_IO_DATA *m_clientData;                   ///<Client로 받은 Data
		LPPER_IO_DATA m_TempclientData;              ///<임시 Client로 받은 Data

		LList* m_PacketList;                        ///<최고 10개의 Receive Data 관리

		void Init();
		//int Connect();
		//////////////////////////
        CRITICAL_SECTION	m_CS;
		int CreateSocketInfo(SOCKET);               //미사용
		void freeSocketInfo();




   public:
		TCPCLIENT();
		~TCPCLIENT();

		int Open(AnsiString strIP, int nPort);
		void Close();
		int Connect();

		void Update();
		int CheckConnected();

		//char RecvData[BUF_SIZE];

		int Read(char* Recv);
		int Send(char* Send, int strLen);

		bool IsConnect();
//		bool IsReadData();             //미사용

		UINT GetPacketCount();

		WSANETWORKEVENTS NetworkEvents; ///<WaitforEvent에 넣기 위한 이벤트


};
#endif
