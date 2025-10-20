//---------------------------------------------------------------------------


#pragma hdrstop

#include "TCPClient.h"
#include "LogHandler.h"	// Log 작성 목적

#define TCP_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\TCP.log", __VA_ARGS__)
//---------------------------------------------------------------------------
/**
@brief   TCPCLIENT 생성자
@author  puting
@date    2013.05.03
*/
TCPCLIENT::TCPCLIENT()
{
	Init();
}
/**
@brief   TCPCLIENT 소멸자
@author  puting
@date    2013.05.03
*/
TCPCLIENT::~TCPCLIENT()
{
	Close();

	WSACleanup();  // 전체 소켓이 종료되므로 Check 필요

	DeleteCriticalSection(&m_CS);

   	delete m_PacketList;
	m_PacketList = NULL;
}
//-----------------------------------------------------------------------------
/**
@brief   초기화 함수
@author  puting
@date    2013.05.03
*/
void TCPCLIENT::Init()
{

	m_bConnected = false;

	////////////////////////////////////////////////
	WSADATA wsaData; ///WSA Data : 시작때 사용
	//1.크리티컬 섹션 생성
	InitializeCriticalSection(&m_CS);

	hSocket = NULL;

	m_TempclientData = NULL;
	m_clientData = NULL;

	m_PacketList = NULL;
	m_PacketList = new LList();

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		TCP_LOG("[Client]WSAStartup fail!!");
		return;
	}
    //////////////////////////////////////////
}
/**
@brief   Close 함수
@date    2013.05.03
@author  puting
*/
void TCPCLIENT::Close()
{

	freeSocketInfo();
    for(int j=0; j<GetPacketCount();j++)
	{
		m_TempclientData = (LPPER_IO_DATA)m_PacketList->popFront();     // 큐처럼 사용


		delete m_TempclientData;
	}

}
//-----------------------------------------------------------------------------
/**
@brief   Open 함수
@date    2013.05.03
@author  puting
@param   nPort : 연결할 Port Number
@return  정상인 경우 0, 에러 발생한 경우 -1 리턴
*/
int TCPCLIENT::Open(AnsiString strIP, int nPort)
{

	if(hSocket==NULL)
	{
//		hSocket = socket(AF_INET,SOCK_STREAM,0);
		hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		memset(&ServAddr,0,sizeof(ServAddr));
		ServAddr.sin_family = AF_INET;
		ServAddr.sin_port = htons(nPort);

		//추후 외부에서 접속을 원할 경우 IP를 따로 받는 부분 추가 할 필요 puting
		ServAddr.sin_addr.s_addr = inet_addr(strIP.c_str());//"10.240.21.24");//


		unsigned long nonblock = 1;
		if(CreateSocketInfo(hSocket)== -1)
		//if(ioctlsocket(hSocket, FIONBIO, &nonblock) == SOCKET_ERROR)
		{
			TCP_LOG("[Client]CreateSocketInfo fail!!");
			return -1;
		}

		if(Connect() == -1)
		{
			TCP_LOG("[Client]Connect fail!!");
			return -1;
		}
		else
		{
			if(CheckConnected() == 0)
			{
				TCP_LOG("[Client]Connect OK!!");
				return 0;
			}
			else
			{
				TCP_LOG("[Client]Connect fail[%d]!!", CheckConnected());
				return -3;
			}
		}
	}
	else
	{
		TCP_LOG("Socket not NULL");
		return -2;
    }
}
//-----------------------------------------------------------------------------
/**
@brief   Connect 함수
@date    2013.07.03
@author  puting
@return  정상인 경우 0, 에러 발생한 경우 -1 리턴
*/
int TCPCLIENT::Connect()
{
	int iResult = connect(hSocket, (SOCKADDR*)&ServAddr, sizeof(ServAddr));

	if(iResult == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Close();
			TCP_LOG("[Client]Connect_1 %d %d", WSAGetLastError(), iResult);
			return 0;
		}

        // 연결 진행 중(비동기 연결)
		TCP_LOG("[Client]Connect_4 %d %d", WSAGetLastError(), iResult);
		return 1;
	}
	else
	{
		m_bConnected = true;

		if(NewEvent != NULL)
		{
			if(WSAEventSelect(hSocket, NewEvent, FD_READ|FD_CONNECT|FD_CLOSE) == SOCKET_ERROR)
			{
				Close();
				TCP_LOG("[Client]Connect_2");
				return -1;
			}
		}

		TCP_LOG("[Client]Connect_3 %d", m_bConnected);
		return 0;
    }
}
/**
@brief   Update 함수
@date    2013.05.03
@author  puting
@param   해당 Updata 함수를 쓰레드에 의해 감시하여야 한다.(클라이언트 접속 및 데이터 받기)
*/
void TCPCLIENT::Update()
{
	int ncheckError = 0;
   //	WSANETWORKEVENTS	NetworkEvents;

	WSAWaitForMultipleEvents(1, &NewEvent, FALSE, 0, FALSE);


	if(WSAEnumNetworkEvents(hSocket,
			   NewEvent, &NetworkEvents) == SOCKET_ERROR)
	{
		ncheckError = WSAGetLastError();
		TCP_LOG("[Client]Update nError:%d", ncheckError);
		return;
	}
	if(NetworkEvents.lNetworkEvents & FD_READ )
	{
		int recvBytes = 0;
		char buf[BUF_SIZE] = {0,};

		TCP_LOG("[Client]Update Read:%d", NetworkEvents.lNetworkEvents);

		recvBytes = recv(hSocket, buf, BUF_SIZE, 0);
		if(recvBytes == 0 || recvBytes == SOCKETERROR)
		{
			int nError = WSAGetLastError();
			TCP_LOG("[Client]recv is  fail! [recvBytes:%d][nError:%d]",recvBytes,nError);
			if(nError == WSAEWOULDBLOCK)
			{
				//non overlapped 소켓 : 비 봉쇄 소켓에 아직 읽을 데이터가 없음, overlapped(:12) 소켓 : 너무 많은 중첩 입출력 요구가 있음
			}
			else if(nError == WSAECONNRESET)
			{
				//연결이 원격 호스트에 의해 재설정되었음.

				Close();
				return;
			}
			else if(nError == WSA_IO_PENDING)
			{
				//Overlapped 연산은 나중에 완료될 것이다. 중첩 연산을 위한 준비가 되었으나, 즉시 완료되지 않았을 경우발생
			}
			else if(nError == WSA_INVALID_HANDLE)
			{
				//지정된 이벤트 객체 핸들이 잘못 되었다.
				Close();
				return;
			}
		}

		EnterCriticalSection(&m_CS);
		int TempCount = GetPacketCount();
		LeaveCriticalSection(&m_CS);

		if(TempCount >= 0 && TempCount < 10)
		{
			if(recvBytes > 0 && recvBytes <= BUF_SIZE) // 데이터가 있을경우 추가
			{

				m_clientData = new  PER_IO_DATA;

				//정보를 받은 클아이언트 정보를 입력
			   //	m_clientData->hRcvSock =(SOCKET)m_ClientList->refer(eventIndex - WSA_WAIT_EVENT_0);
				memcpy(m_clientData->buffer , buf, recvBytes);
//						ClientData.buf = (hRcvbInfo->DataBuf.buf);
				m_clientData->Length = recvBytes;
				m_PacketList->insertBack(m_clientData);       //큐처럼 사용
					//크리티컬섹션 시작
				// m_clientData =NULL;

			}
			else
			{
				TCP_LOG("[Client]recvBytes is Wrong : %d", recvBytes);
			}
			/////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			TCP_LOG("[Client]PackCount is Wrong : %d", TempCount);
		}
	}

	if(NetworkEvents.lNetworkEvents & FD_CLOSE)
	{
		Close();
	}
}
int TCPCLIENT::CheckConnected()
{
	int nError = 0;
	int optLen = sizeof(int);

	if( getsockopt(hSocket, SOL_SOCKET, SO_ERROR, (char*)&nError, &optLen) == SOCKET_ERROR)
	{
		// 결과값 갖고 오는데 에러 발생
		// 연결중 에러 발생으로 errno 값으로 결과 확인
		// 에러 리턴
		TCP_LOG("[Client]CheckConnected %d", WSAGetLastError());
		return 1;
	}

	TCP_LOG("[Client]Check Connected Error %d", nError);

	if(nError != 0)
	{
		if(nError == WSAECONNREFUSED)
		{
			// 연결 거부로 연결 실패
			// 에러 처리
			return 2;
		}
		else if(nError == WSAETIMEDOUT)
		{
			// 연결 대기 시간 초과로 연결 실패
	        // 에러 처리
			return 3;
		}

		// 기타 Error
		return 4;
	}

	m_bConnected = true;
	if(NewEvent != NULL)
	{
		if(WSAEventSelect(hSocket, NewEvent, FD_READ|FD_CONNECT|FD_CLOSE) == SOCKET_ERROR)
		{
			Close();
			TCP_LOG("[Client]Check Connected Socket Close");
			return -1;
		}
		return 0;
	}
	return -2;
}
/**
@brief   Read 함수
@date    2013.07.03
@author  puting
@param   szRecv : 해당 클라이언트에서 읽은 Data
@return  Length
*/
int TCPCLIENT::Read(char* szRecv)
{
	int recvBytes = 0;

	EnterCriticalSection(&m_CS);
	int TempCount = GetPacketCount();
	LeaveCriticalSection(&m_CS);

	if(TempCount > 0)
	{

		m_TempclientData = (LPPER_IO_DATA)m_PacketList->popFront();     // 큐처럼 사용
		if(m_TempclientData->Length > 0 && m_TempclientData->Length < BUF_SIZE)
		{
			memcpy(szRecv, m_TempclientData->buffer, m_TempclientData->Length);
			recvBytes = m_TempclientData->Length;

		}
		else
		{
			TCP_LOG("[Client]ReadFail is Wrong : [Length:%d]", m_TempclientData->Length);
			memset(szRecv,0x00, sizeof(szRecv));
			recvBytes = 0;
        }

		delete m_TempclientData;
		m_TempclientData =NULL;

	}
	else
	{
//		TCP_LOG("[Client]ReadFail is Wrong : [TempCount:%d]", TempCount);
		memset(szRecv,0x00, sizeof(szRecv));
		recvBytes = 0;
	}

	return recvBytes;

}
/**
@brief   Send 함수
@date    2013.07.03
@author  puting
@param   szSend : 해당 클라이언트에 쓸 Data
@param   strLen : 해당 클라이언트에 쓸 Data의 길이
@return  정상인 경우 0, 에러 발생한 경우 -1 리턴
*/
int TCPCLIENT::Send(char* szSend,int strLen)
{

	if(strLen <= 0)
	{
		TCP_LOG("[client]write is Wrong : [strLen:%d]", strLen);
		return -1;
	}
	if(send(hSocket,szSend,strLen,0) == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();


		if(nError == WSAEWOULDBLOCK)
		{
		//non overlapped 소켓 : 비 봉쇄 소켓에 아직 읽을 데이터가 없음, overlapped(:12) 소켓 : 너무 많은 중첩 입출력 요구가 있음
		}
		else if(nError == WSAECONNRESET)
		{
		//연결이 원격 호스트에 의해 재설정되었음.
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}
		else if(nError == WSAECONNABORTED)
		{
		//타임아웃 혹은 상대방의 접속종료들과 같은 소프트웨어적인 문제로 연결이 끊겼음.
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}
		else if(nError == WSA_IO_PENDING)
		{
        //Overlapped 연산은 나중에 완료될 것이다. 중첩 연산을 위한 준비가 되었으나, 즉시 완료되지 않았을 경우발생
		}
		else if(nError == WSA_INVALID_HANDLE)
		{
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}

	}
	return 0;

}
/**
@brief   IsConnect 함수
@date    2013.05.24
@author  puting
@param   Server와 연결되었을 시 true, 연결 해제시 false
*/
bool TCPCLIENT::IsConnect()
{
	return m_bConnected;
}
//-----------------------------------------------------------------------------

int TCPCLIENT::CreateSocketInfo(SOCKET ClientSock)
{
	if((NewEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		return -1;
	}
	return 1;
}

//-----------------------------------------------------------------------------
void TCPCLIENT::freeSocketInfo()
{
	m_bConnected = false;

	closesocket(hSocket);
	hSocket= NULL;

	if(WSACloseEvent(NewEvent) == TRUE)
    {
		//정상 처리
	   	NewEvent = NULL;
	}
	else
	{
	   //실패 처리
	}
}
/**
@brief   GetPacketCount 함수
@date    2013.05.03
@author  puting
@param   받은 데이터의 갯수(최대 10개까지 저장)
@return  현재 List에 저장된 Data 갯수 return
*/
UINT TCPCLIENT::GetPacketCount()
{
	int Count = m_PacketList->getCount();
	return Count;
}
/**
@brief   ReadData Check 함수
@date    2013.05.24
@author  puting
@param   Read Data Check

bool TCPCLIENT::IsReadData()
{
	if(GetPacketCount()<=0)
		return false;
	else
		return true;
}
*/

#pragma package(smart_init) 
