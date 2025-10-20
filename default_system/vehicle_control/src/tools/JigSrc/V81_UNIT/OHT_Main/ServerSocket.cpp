/**
@file    ServerSocket.cpp
@version OHT 7.0
@brief   ServerSocket Class CPP File
*/
// ---------------------------------------------------------------------------
#include "ServerSocket.h"


// -------------------------------------------
// Debugging용 Define
// -------------------------------------------


//#define TCP_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\TCP.log", __VA_ARGS__)
//수정
#define TCP_LOG(...)    WRITE_LOG_UNIT(pLogTCP, __VA_ARGS__)


// ---------------------------------------------------------------------------
/**
@brief   ServerSocket 생성자
@author  puting
@date    2013.05.03
*/
ServerSocket::ServerSocket()//:ThreadCtrl()
{
	pLogTCP = getLogUnit("D:\\log\\TCP.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
    WSADATA wsaData; ///WSA Data : 시작때 사용
	//1.크리티컬 섹션 생성
	InitializeCriticalSection(&m_CS);

	EventTotal = 0;
    nPortNum = 0;
	hServSock = NULL;
	m_clientData = NULL;

	m_TempclientData = NULL;

	m_ClientList =NULL;
	m_PacketList = NULL;
	nClientCount =1;   //Server 1개

    m_ClientList = new LList();
	m_PacketList = new LList();
	BufferPointer = NULL;
	nRestLenth = 0;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		TCP_LOG("[Server]WSAStartup Fail");
		return;
	}


}
//--------------------------------------------------------------------------
/**
@brief   ServerSocket 소멸자
@author  puting
@date    2013.05.03
*/
ServerSocket::~ServerSocket()
{

	SocketClose();

	WSACleanup();  // 전체 소켓이 종료되므로 Check 필요


	DeleteCriticalSection(&m_CS);

	for(int i=0; i<m_ClientList->getCount(); i++)
	{
		CLIENT_SOCK* deleteSock = (CLIENT_SOCK*)m_ClientList->refer(i);
		if(deleteSock != NULL) delete deleteSock;
	}

	delete m_ClientList;
	delete m_PacketList;

	if(BufferPointer !=NULL)
	{
		delete[] BufferPointer;
		BufferPointer = NULL;
		nRestLenth =0;
	}

	m_ClientList = NULL;
	m_PacketList = NULL;
}
//--------------------------------------------------------------------------
/**
@brief   ServerSocket Open 함수
@date    2013.05.03
@author  puting
@param   PortNum를 매개변수로 받아 전역변수에 저장(윈속 종료에 의해 다시 초기화 및 호출하기 위해)
@return  정상인 경우 true, 에러 발생한 경우 false 리턴
*/
bool ServerSocket::SocketOpen(int PortNum)
{

	unsigned long mode = 1;

	if(hServSock==NULL)
	{
		/////비동기 소켓의 경우////

		SOCKADDR_IN	servAdr;



		/////////////////////////////////////////////////////////////////////////////
//		hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

//		for(int i=0; i<2; i++)
//		{
//		   _beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
//		}
//////////////////////////////////////////////////////////////////////////////

		hServSock = socket(AF_INET, SOCK_STREAM, 0);

	   //	ioctlsocket(hServSock, FIONBIO, &mode); // for non blocking mode socket

		// 버퍼 크기 조절 //

	 // int optlen = sizeof(optval);
	 // getsockopt(hServSock,SOL_SOCKET,SO_SNDBUF,(char*)&optval,&optlen);
//		optval = MAXLINE;
//		setsockopt(hServSock, SOL_SOCKET, SO_SNDBUF, (char*)&optval,sizeof(optval));
		/////////

		// 서버측 정보
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
		//servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servAdr.sin_port = htons(PortNum);
		nPortNum = PortNum;

		if(CreateSocketInfo(hServSock)== -1)
		{
			TCP_LOG("[Server]ServerCreateSocket Fail");
			return false;
		}

		 if(WSAEventSelect(hServSock, EventArray[EventTotal - 1], FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR)
		{
			TCP_LOG("[Server]WSAEventSelect Fail");
			return false;
		}


		// Bind Error Check
		if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		{
			TCP_LOG("[Server]bind Fail");
			return false;
		}

		// Listen Error Check
		if(listen(hServSock, 5)==SOCKET_ERROR)   //두번째 매계변수는 연결용청 대기 큐의 크기 정보 전달.
		{
			TCP_LOG("[Server]listen Fail");
			return false;
		}



		//waitforevent를 받기위한 이벤트 생성
	    //newEvent = WSACreateEvent();

	}
	return true;
}
//------------------------------------------------------------------------------
/**
@brief   ServerSocket Close 함수
@date    2013.05.03
@author  puting
@param   List 및 Data buff 해제/윈속 및 이벤트종료
@return  정상인 경우 true, 에러 발생한 경우 false 리턴
*/
bool ServerSocket::SocketClose()
{

	int SocketCount = EventTotal;
	if(SocketCount > 0)
	{
		for(int Count = SocketCount; Count--;)
		{
			freeSocketInfo(Count);
		}
	}

	int TempCount = GetPacketCount();

	for(int j=0; j < TempCount;j++)
	{
		m_TempclientData = (LPCLIENT_DATA)m_PacketList->popFront();     // 큐처럼 사용

		for(int i=0; i<m_ClientList->getCount(); i++)
		{
			CLIENT_SOCK* tmpData = (CLIENT_SOCK*)m_ClientList->refer(i);

			//WSARecv에 의해 클라이언트가 제가 되어있는 상태
			if(tmpData->hClientSock == m_TempclientData->hRcvSock)
			{
				freeSocketInfo(i);
				break;
			}
		}

		delete[] m_TempclientData->buf;
		delete m_TempclientData;
	}


	TCP_LOG("[Server]Socket Close");


}
//--------------------------------------------------------------------------
/**
@brief   Update 함수
@date    2013.05.03
@author  puting
@param   해당 Updata 함수를 쓰레드에 의해 감시하여야 한다.(클라이언트 접속 및 데이터 받기)
*/
void ServerSocket::Update()
{

		SOCKET hClientSock;
		int eventIndex = 0;

		eventIndex = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, 0, FALSE);
		if(eventIndex == WSA_WAIT_FAILED)
		{
		   //	TCP_LOG("[Server]eventIndex is WSA_WAIT_FAILED");
			return;
    	}
		else if(eventIndex==INVALID_EVENT)
		{
		   //	TCP_LOG("[Server]eventIndex is INVALID_EVENT");
			return;
		}
		//발생한 이벤트 종류 구분

		if(EventTotal >=eventIndex)
		{
			SOCKET eventSock = ((CLIENT_SOCK*)m_ClientList->refer(eventIndex - WSA_WAIT_EVENT_0))->hClientSock;
			if(WSAEnumNetworkEvents(eventSock, EventArray[eventIndex - WSA_WAIT_EVENT_0], &networkEvents) == SOCKET_ERROR)
			{
			  //	TCP_LOG("[Server]WSAEnumNetworkEvents fail");
				return;
			}

			/////////////////////////////연결시/////////////////////////////////////////////
			if(networkEvents.lNetworkEvents & FD_ACCEPT)
			{
				if(networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				{
					TCP_LOG("[Server]networkEvents is ErrorCode : &d", networkEvents.iErrorCode[FD_ACCEPT_BIT]);
					return;
				}

				int nAddrSize = sizeof(m_CurClientInfo);
				if ((hClientSock = accept(eventSock, (SOCKADDR*)&m_CurClientInfo, &nAddrSize)) == INVALID_SOCKET)
				{
					TCP_LOG("[Server]accept is INVALID_SOCKET");
					return;
				}

				if(EventTotal > 10)
				{
					TCP_LOG("[Server]EventTotal is EventTotal Over(Max :10)");
					closesocket(hClientSock);
					return;
				}

				CreateSocketInfo(hClientSock, inet_ntoa(m_CurClientInfo.sin_addr));

				if(WSAEventSelect(hClientSock, EventArray[EventTotal -1], FD_READ|FD_CLOSE) == SOCKET_ERROR)
				{
					TCP_LOG("[Server]WSAEventSelect is SOCKET_ERROR");
					return;
				}
			}
			///////////////////////////////읽는 이벤트 발생 시
			if(networkEvents.lNetworkEvents & FD_READ )
			{
				int recvBytes = 0;
				char buf[BUF_SIZE] = {0,};

				recvBytes = recv(eventSock, buf, BUF_SIZE, 0);
				if(recvBytes== 0 || recvBytes == SOCKETERROR)
				{
					int tempErrorCode = WSAGetLastError();
					TCP_LOG("[Server]recv is SOCKETERROR[recvBytes:%d][tempErrorCode:%d]",recvBytes,tempErrorCode);
					switch(tempErrorCode)
					{
						case WSAEWOULDBLOCK:
                         //non overlapped 소켓 : 비 봉쇄 소켓에 아직 읽을 데이터가 없음, overlapped(:12) 소켓 : 너무 많은 중첩 입출력 요구가 있음
						break;

						case WSAENOBUFS:
                        //남아있는 버퍼공간이 없어서 소켓을 사용할 수 없음
						break;

						case WSAECONNABORTED:
							freeSocketInfo(eventIndex - WSA_WAIT_EVENT_0);
						//타임아웃 혹은 상대방의 접속종료들과 같은 소프트웨어적인 문제로 연결이 끊겼음.
							return;
						break;

						default:
						///@todo 에러 코드 정의 필요
						break;


					}

					//Error 발생
				}

				EnterCriticalSection(&m_CS);
				int TempCount = GetPacketCount();
				LeaveCriticalSection(&m_CS);

				if(TempCount >= 0 && TempCount < 10)
				{
			/////////////////////////////////////////////////////////////////////////////////////
					if(recvBytes > 0 && recvBytes <= BUF_SIZE) // 데이터가 있을경우 추가
					{

						m_clientData = new  CLIENT_DATA;

						//정보를 받은 클아이언트 정보를 입력
						m_clientData->hRcvSock = eventSock;
						memcpy(m_clientData->buf , buf, recvBytes);
//						ClientData.buf = (hRcvbInfo->DataBuf.buf);
						m_clientData->Length = recvBytes;
						m_PacketList->insertBack(m_clientData);       //큐처럼 사용
							//크리티컬섹션 시작
						// m_clientData =NULL;

					}else
					{
						TCP_LOG("[Server]recvBytes is Wrong : %d", recvBytes);
                    }
				/////////////////////////////////////////////////////////////////////////////////
			   }else
			   {
					TCP_LOG("[Server]PackCount is Wrong : %d", TempCount);
               }

			}

			if(networkEvents.lNetworkEvents & FD_CLOSE)
			{
				freeSocketInfo(eventIndex-WSA_WAIT_EVENT_0);
			}
		}

}
//--------------------------------------------------------------------------
/**
@brief   GetPacketCount 함수
@date    2013.05.03
@author  puting
@param   받은 데이터의 갯수(최대 10개까지 저장)
@return  현재 List에 저장된 Data 갯수 return
*/
UINT ServerSocket::GetPacketCount()
{
	int Count = m_PacketList->getCount();
	return Count;
}
//--------------------------------------------------------------------------
/**
@brief   GetClientCount 함수
@date    2013.05.03
@author  puting
@param   클라이언트의 갯수(최대 10개까지 저장)
@return  현재 List에 저장된 클라이언트 갯수 return
*/
UINT ServerSocket::GetClientCount()
{
	int Count = m_ClientList->getCount();
	return Count;  //현재는 무조건 1이여야함. <<1포트에 1개의 연결만 하기때문에>>
}
//--------------------------------------------------------------------------
/**
@brief   IsClientConnect 함수
@date    2013.05.24
@author  puting
@param   접속 성공시 한번만 true를 해주기 위해
*/
bool ServerSocket::IsClientConnect()
{
	int nTempClientCount = GetClientCount();
	if(nTempClientCount > 1)
	{
		if(nClientCount < nTempClientCount)
		{
			nClientCount++;
			return true;
		}
	}
		return false;

}
//-----------------------------------------------------------------------------
/**
@brief   IsClientDisConnect 함수
@date    2013.05.24
@author  puting
@param   접속 해제시 한번만 true를 해주기 위해
*/
bool ServerSocket::IsClientDisConnect()
{
	int nTempClientCount = GetClientCount();
  	if(nTempClientCount > 0)
	{
		if(nClientCount > nTempClientCount)
		{
			nClientCount--;
			return true;
		}

		return false;
	}
}
//---------------------------------------------------------------------------
/**
@brief   ReadData Check 함수
@date    2013.05.24
@author  puting
@param   Read Data Check
*/
bool ServerSocket::IsClientReadData()
{
	if(GetPacketCount()<=0)
		return false;
	else
		return true;
}
//---------------------------------------------------------------------------
/**
@brief   Client 정보를 Remove 함수
@date    2013.05.24
@author  puting
@param   Read Data Check
*/
void ServerSocket::ClientRemove(UINT Index)
{
	freeSocketInfo(Index+1);
}
//-----------------------------------------------------------------------
/**
@brief   Client 정보를 얻는 함수
@date    2013.05.24
@author  puting
@param   Get함수
*/
SOCKET ServerSocket::GetClientInfo(UINT Index)
{
	return (SOCKET)((CLIENT_SOCK*)m_ClientList->refer(Index+1))->hClientSock;
}
//---------------------------------------------------------------------------
/**
@brief   Read 함수
@date    2013.05.03
@author  puting
@param   List에 저장된 받은 데이터를 읽는 함수
@return  정상인 경우 true, 에러 발생한 경우 false 리턴
*/
bool ServerSocket::Read(char* RcvBuf, int* Length, int* Count)	// 읽은 데이터의 정보를 얻기
{

	//m_packList를 통해 데이터가 있을경우 그 데이터를 빼서 클라이언트 정보를 함께 넘겨준다.
	//구조체를 넣어줘서 꺼낼경우 클라이언트 소켓정보와 버퍼를 함께 넘겨준다.
	bool returnValue = false;

	EnterCriticalSection(&m_CS);
	int TempCount = GetPacketCount();
	LeaveCriticalSection(&m_CS);

	if(TempCount > 0)
	{

		m_TempclientData = (LPCLIENT_DATA)m_PacketList->popFront();     // 큐처럼 사용

		if(m_TempclientData->Length > 0 && m_TempclientData->Length < BUF_SIZE)
		{
			memcpy(RcvBuf, m_TempclientData->buf, m_TempclientData->Length);
			*Length = m_TempclientData->Length;
			*Count = TempCount;
		}
		else
		{
			TCP_LOG("[Server]ReadFail is Wrong : [Length:%d]", m_TempclientData->Length);
			memset(RcvBuf, 0x00, BUF_SIZE);
			*Length  = 0;
	  		*Count = 0;
        }

		delete m_TempclientData;
		m_TempclientData =NULL;


		returnValue = true;

	}
	else
	{
		TCP_LOG("[Server]ReadFail is Wrong : [TempCount:%d]", TempCount);
		memset(RcvBuf, 0x00, BUF_SIZE);
		*Length  = 0;
		*Count = 0;
		returnValue = false;
	}


	return returnValue;
}
//--------------------------------------------------------------------------
/**
@brief   Write 함수
@date    2013.05.03
@author  puting
@param   해당 클라이언트에 Data를 쓰기
@return  정상인 경우 true, 에러 발생한 경우 false 리턴
*/
bool ServerSocket::Write(char* SendBuf, int Length, SOCKET ClientSock)
{

	// memset(overlapped,0,sizeof(WSAOVERLAPPED));
	// WSASend((SOCKET)m_ClientList->refer(Index), &(hbInfo->databuf), 1, &recvBytes, 0, overlapped, NULL);
	bool returnValue = false;
	int SendLen = 0;

	if(Length > 0)
	{
		SendLen = send(ClientSock, SendBuf, Length, 0);

		if(SendLen ==SOCKET_ERROR)
		{
			for(int i=0; i<m_ClientList->getCount(); i++)
			{
				SOCKET tmpSock = (SOCKET)((CLIENT_SOCK*)m_ClientList->refer(i))->hClientSock;
				if(tmpSock == ClientSock)
				{
					//WSARecv에 의해 클라이언트가 제가 되어있는 상태
					freeSocketInfo(i);
					TCP_LOG("[Server]write is Wrong : [SendLen:%d][index:%d]", SendLen,i);
					break;
				}
			}

			return false;
		}
		else
			return true;
	}
	else
	{
		 TCP_LOG("[Server]write is Wrong : [Length:%d]", Length);
		 return false;                    //보낼테이터의 크기가 없을경우 리턴.
	}
}
//-------------------------------------------------------------------------------

/**
@brief   Socket을 생성함
@remark	 IP정보를 저장할 필요가 없을 때 사용
@date    2014.11.12
@author  doori.shin
@param   ClientSock : 생성할 소켓
@return  정상인 경우 1, 에러 발생한 경우 -1
*/
int ServerSocket::CreateSocketInfo(SOCKET ClientSock)
{
	return CreateSocketInfo(ClientSock, NULL);
}

/**
@brief   Socket을 생성함
@remark	 IP정보를 저정해야할 때 사용
@date    2014.11.12
@author  doori.shin
@param   ClientSock : 생성할 소켓
@param 	 ip : 소켓의 IP
@return  정상인 경우 1, 에러 발생한 경우 -1
*/
int ServerSocket::CreateSocketInfo(SOCKET ClientSock, char* ip)
{
	if((EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		return -1;
	}

	CLIENT_SOCK* clientSock = new CLIENT_SOCK;
	clientSock->hClientSock = ClientSock;
	memset(clientSock->szIPAddress, 0, 20);
	if(ip != NULL)
		strcpy(clientSock->szIPAddress, ip);

	m_ClientList->insertBack((CLIENT_SOCK*)clientSock);
	EventTotal ++;
	return 1;
}
//-------------------------------------------------------------------------------
void ServerSocket::freeSocketInfo(int eventIndex)
{
//	closesocket((SOCKET)m_ClientList->pop(eventIndex));
	CLIENT_SOCK* clientSock = (CLIENT_SOCK*)m_ClientList->pop(eventIndex);
	closesocket((SOCKET)clientSock->hClientSock);
	delete clientSock;



	for(int i = eventIndex; i < EventTotal; i++)
    {
		EventArray[i] = EventArray[i+1];
	}
    EventTotal--;
}

char* ServerSocket::GetCurIPAddress()
{
	return inet_ntoa(m_CurClientInfo.sin_addr);
}

char* ServerSocket::GetIPAddress(UINT index)
{
	CLIENT_SOCK* clientSock = (CLIENT_SOCK*)m_ClientList->refer(index+1);

	return clientSock->szIPAddress;
}
