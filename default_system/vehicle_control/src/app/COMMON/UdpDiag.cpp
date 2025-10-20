/*
	이 모듈을 사용하기 전에 프로젝트 설정에 세팅해야 하는 값
	(프로젝트 설정 -> C++ Compiler -> Directories and Conditionals의 Conditional defines 에 설정)

	[OHT / OHT에뮬인 경우]
	_ONE_PC_TEST 		: 1PC상의 개발 테스트인 경우에만 설정 (OHT, OHT에뮬, OCS에뮬)
	IS_OHT		 		: OHT, OHT 에뮬레이터는 반드시 설정한다
	IS_REAL_OHT			: 에뮬이 아닌 진짜 OHT에서만 사용하도록 하는 경우 
	_UDP_MODULE_CHILD 	: OHT 에뮬 / OCS 에뮬레이터만 설정한다
	_IN_UDP_UPPER_FORM 	: OHT 에뮬 / OCS에뮬레이터는 반드시 설정. 상위로 상세로그를 전달하는 구조하면 OHT도 설정한다

	[OCS 에뮬레이터인 경우]
	_ONE_PC_TEST 		: 1PC상의 개발 테스트인 경우에만 설정 (OHT, OHT에뮬, OCS에뮬)
	IS_MCP				: OCS 에뮬레이터는 반드시 설정한다
	_UDP_MODULE_CHILD 	: OHT 에뮬 / OCS 에뮬레이터만 설정한다
	_IN_UDP_UPPER_FORM 	: OHT 에뮬 / OCS에뮬레이터는 반드시 설정. 상위로 상세로그를 전달하는 구조하면 OHT

	[TP 인 경우]
	IS_TP				: TP는 반드시 설정한다.
	_IN_UDP_UPPER_FORM	: TP는 반드시 설정한다.
      
*/


#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpDiag.h"
#include "Protocol_Define_Modify.h"
#include "SocketInitClose.h"
#include "UdpServer.h"
#include "UdpBroadCastSender.h"
#include "UdpUniCastSender.h"
#include "PUtily.h"

#ifdef _IN_UDP_UPPER_FORM
	#include "Udp.h"
#endif

#ifdef IS_MCP
	#include "McpMain.h"	// 상위를 include 해야 하니까
#endif
#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif
//---------------------------------------------------------------------------

SocketInitClose		*pDIAG_SocketInitClose;		// 소켓의 초기화 기능

// [OHT - DIAG]
UdpServer 			*pDIAG_BroadCastServer;	// 주소획득 프로토콜 처리
UdpServer 			*pDIAG_UniCastServer;		// 주소획득 및 제어 프토토콜 처리

// [OHT - AS]
UdpServer 			*pUdpAsServer;			// OHT - AS 통신 전용

// [OHT - AGENT]
UdpServer			*pUdpAgentServer;       // OHT - Agent 통신 전용

// [COMMON]
UdpBroadCastSender  *pDIAG_UdpBroadCastSender;	// 브로드케스트 전송처리
UdpUniCastSender	*pDIAG_UdpUniCastSender; 	// 일대일 통신 전송처리
UdpUniCastSender	*pAGENT_UdpUniCastSender; 	// AGENT와 일대일 통신 전송처리

VHL_INFO DIAG_LIST[1];
//---------------------------------------------------------------------------
UdpDiag::UdpDiag()
{
	pDIAG_SocketInitClose = NULL;
	pDIAG_BroadCastServer = NULL;
	pDIAG_UniCastServer = NULL;
	pUdpAsServer = NULL;
	pUdpAgentServer = NULL;

	pDIAG_UdpBroadCastSender = NULL;
	pDIAG_UdpUniCastSender	= NULL;
	pAGENT_UdpUniCastSender = NULL;

	if (pDIAG_SocketInitClose == NULL)
		pDIAG_SocketInitClose = new SocketInitClose();

	if (pDIAG_UdpBroadCastSender == NULL)
		pDIAG_UdpBroadCastSender = new UdpBroadCastSender();

	if (pDIAG_UdpUniCastSender == NULL)
		pDIAG_UdpUniCastSender = new UdpUniCastSender();

	if (pAGENT_UdpUniCastSender == NULL)
		pAGENT_UdpUniCastSender = new UdpUniCastSender();


	// 소켓 초기화 일괄적으로 해줌
	pDIAG_SocketInitClose->SocketInit();

#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPCommunication.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif

#ifdef IS_MCP
	// PORT 9020
	iDIAG_MachineType 	= MACHINETYPE_DIAG_SVR;
	strDIAG_SVR_MachineID 	= "SDG-01";
	iDIAG_BroadCastPort = ADDRESS_DIAG_GET_DEFAULT_PORT;
	iDIAG_UnicastPort 	= CONTROL_DIAG_DEFAULT_PORT;

	// #ifdef	_ONE_PC_TEST
	// 	ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
	// #endif
#endif

#ifdef IS_OHT
	//
	// 이 부분은 CLW가 OCS와 통신하기 위해 사용하는 정보이다
	// PORT 9020
	strDIAG_SVR_Address 	= "";		// DIAG_SVR의 주소. 처음에는 비어있다.
	bDIAG_SVR_Connected 	= false;	// OHT가 사용. DIAG_SVR와 연결되었다는 플래그
	strDIAG_SVR_MachineID 	= "";		// DIAG_SVR의 머신 ID
	iDIAG_MachineType 	= MACHINETYPE_DIAG_NORMAL_OHT;
	strMyMachineId = "OHT001";
	#ifdef	_ONE_PC_TEST
		// PORT 9020
		iDIAG_BroadCastPort = (ADDRESS_DIAG_GET_DEFAULT_PORT + 2);     // OHT인 경우 & 1PC 모드인 경우에 한해, 스스로 자신의 포트번호를 올려버림
		iDIAG_UnicastPort = (CONTROL_DIAG_DEFAULT_PORT + 2);
		
		// ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
	#else
		// PORT 9020
		iDIAG_BroadCastPort = ADDRESS_DIAG_GET_DEFAULT_PORT;
		iDIAG_UnicastPort = CONTROL_DIAG_DEFAULT_PORT;
	#endif
#endif

#ifdef IS_REAL_OHT
	DIAG_BroadcastBind();
	UnicastBind();
#endif

// shkim. Comm test (temp)
#if 0
	// PORT 9020
	DIAG_LIST[0].strAddress 	= K_DEMO_DIAG_VHL_ADDR;
	DIAG_LIST[0].strMachineID 	= K_DEMO_DIAG_VHL_MACHINE_ID;

	strDIAG_SVR_Address 	= K_DEMO_DIAG_SVR_ADDR;		// OCS의 주소. 처음에는 비어있다.
	bDIAG_SVR_Connected 	= true;	// OHT가 사용. OCS와 연결되었다는 플래그
	strDIAG_SVR_MachineID 	= K_DEMO_DIAG_SVR_MACHINE_ID;	// OCS의 머신 ID

	AnsiString strInfo2;
	strInfo2.sprintf("Diag OHT 주소 : %s Diag OHT 이름 : %s  \r\n Diag Svr 주소 : %s Diag Svr 이름 : %s \r\n ",
			DIAG_LIST[0].strAddress, DIAG_LIST[0].strMachineID, strDIAG_SVR_Address, strDIAG_SVR_MachineID);

    ShowMessage(strInfo2);
#endif
// ----------------------------

//#ifdef IS_OHT
	OHT_AS_Bind();
	OHT_AGENT_Bind();
//#endif

//#ifdef IS_AS
//	OHT_AS_Bind();
//#endif
}

//---------------------------------------------------------------------------
void UdpDiag::DisplayLog(String strLog)
{
#ifdef _IN_UDP_UPPER_FORM
	frmUDP->DisplayLog(strLog); 	// 상시 디버그를 위해 UDP폼 안에 있는 경우, 화면에 표시
#endif

#ifdef IS_REAL_OHT
	AnsiString strTmp = strLog;
	ADD_LOG("%s", strTmp);
#endif
}
//---------------------------------------------------------------------------

UdpDiag::~UdpDiag()
{
	pDIAG_SocketInitClose->SocketClose();

	if (pDIAG_SocketInitClose != NULL)
		delete(pDIAG_SocketInitClose);

	if (pDIAG_UdpBroadCastSender != NULL)
		delete(pDIAG_UdpBroadCastSender);

	if (pDIAG_UdpUniCastSender != NULL)
		delete(pDIAG_UdpUniCastSender);

	if (pAGENT_UdpUniCastSender != NULL)
		delete(pAGENT_UdpUniCastSender);

	OHT_AS_Close();
	OHT_AGENT_Close();
}
//---------------------------------------------------------------------------
#ifdef IS_OHT
// 인수설명 : strSvrAddr는 진단서버의 IP주소 (127.0.0.1 등), iListenPortNum는 진단통신 포트번호(OHT와 서버 동일번호 사용 - 디폴트 CONTROL_DIAG_DEFAULT_PORT),
//            (OHT와 서버 동일번호 사용),  strMyName은 OHT의 이름(OHT001 등)
// 일반 OHT : MACHINETYPE_DIAG_NORMAL_OHT 로 설정
// 청소 OHT : MACHINETYPE_DIAG_CLEAN_OHT 로 설정
int UdpDiag::SetDiagInfo(AnsiString strSvrAddr, int iListenPortNum, AnsiString strMyName, int iOhtMachineType)
{
	strDIAG_SVR_Address = strSvrAddr;
	iDIAG_MachineType 	= iOhtMachineType;

#ifdef	_ONE_PC_TEST
	iDIAG_UnicastPort = (iListenPortNum + 2);
#else
	iDIAG_UnicastPort = iListenPortNum;
#endif

	strMyMachineId = strMyName;

	bDIAG_SVR_Connected = true;

	return 0;
}
#endif

//---------------------------------------------------------------------------
#ifdef IS_MCP
// strOhtAddr : 통신할 상대방인 OHT의 IP주소, iListenPortNum : 진단통신 포트번호 (OHT와 서버 동일번호 사용 - 디폴트 CONTROL_DIAG_DEFAULT_PORT), 
// strMyName : 진단서버의 이름 (SDG-01 등),  iDiagMachineType : 진단서버 식별 ID (MACHINETYPE_DIAG_SVR 가 디폴트)
int UdpDiag::SetDiagInfo(AnsiString strOhtAddr, int iListenPortNum, AnsiString strMyName, int iDiagMachineType)
{
	DIAG_LIST[0].strAddress = strOhtAddr;
	iDIAG_UnicastPort = iListenPortNum;
	strDIAG_SVR_MachineID = strMyName;
	iDIAG_MachineType = iDiagMachineType;

	return 0;
}
#endif

//---------------------------------------------------------------------------

int UdpDiag::BroadcastSend(int iPort, AnsiString strMessage)
{
	String strMsg;
	int retval = pDIAG_UdpBroadCastSender->BroadcastSendMessage(iDIAG_BroadCastPort, strMessage);

	strMsg.sprintf(L"[UDP BroadCast] %d바이트를 보냈습니다.",retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::BroadcastSend(int iPort, char* cMessage, int iLen)
{
	int retval = pDIAG_UdpBroadCastSender->BroadcastSendMessage(iPort,cMessage,iLen);
	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::DIAG_BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

 	// bind여부부터 검사
	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "Port 9020 B/C 데이터 수신됨. 상대 IP : " + strClientAddr;
		strTmp2.sprintf(" 상대 Port : %d 길이 : %d 내용 : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("데이터 수신되지 않음. 리턴코드 : %d", iReturn);
	}

	return iReturn;
}
//---------------------------------------------------------------------------
int UdpDiag::DIAG_BroadcastBind()
{
	int iRetVal;

	if (pDIAG_BroadCastServer == NULL)
		pDIAG_BroadCastServer = new UdpServer();

	iRetVal = pDIAG_BroadCastServer->Bind(iDIAG_BroadCastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[진단서버 - 주소 컨트롤 포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[진단서버 - 주소 컨트롤 포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[진단서버 - 주소 컨트롤 포트] Bind 동작 실패");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------

int UdpDiag::DIAG_BroadcastBind(int iBroadCastListenPort)
{
	iDIAG_BroadCastPort = iBroadCastListenPort;
	return DIAG_BroadcastBind();
}
//---------------------------------------------------------------------------
void UdpDiag::DIAG_BroadcastClose()
{
	if (pDIAG_BroadCastServer != NULL)
	{
		pDIAG_BroadCastServer->Close();
		delete(pDIAG_BroadCastServer);
		pDIAG_BroadCastServer = NULL;
	}

	DisplayLog(L"DIAG(9020) Broadcast Server Closed");
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	AnsiString strMsg;
	int retval;

	retval = pDIAG_UdpUniCastSender->UnicastSendMessage(strIpAddr,iPort, strMessage);

	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d바이트를 보냈습니다.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("정의된 최대 패킷 사이즈보다 큰 메시지 송신시도!!");
        return UNICAST_SEND_RESULT_FAIL;
    }

	retval = pDIAG_UdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d바이트를 보냈습니다.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

#if 0
	// ---------- log add -------------
	strMsg = "";	// 한번 찍기 전 반드시 리셋
	for (int i = 0; i < retval; i++)
	{
		oneChar = buf[i];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("%02X(%c) ", oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("%02X ", oneChar);

		// 10개가 될 때마다 저장하고 클리어 해 준다
		if(((i % RAW_LOG_BYTE_1_LINE_SIZE) == 0) && (i != 0))
		{
			DisplayLog(strMsg);
			strMsg = "";
		}

		strMsg = strMsg + strTmp;
	}

	// 데이터는 딱 10개로 떨어지지 않을수 있으므로 나머지를 프린트 한다
	if(strMsg != "")
		DisplayLog(strMsg);
	// -------------------------------------------------------------
 #endif

	return retval;
}

int UdpDiag::UnicastSendAGENT(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("정의된 최대 패킷 사이즈보다 큰 메시지 송신시도!!");
        return UNICAST_SEND_RESULT_FAIL;
	}

	retval = pAGENT_UdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d바이트를 보냈습니다.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

#if 0
	// ---------- log add -------------
	strMsg = "";	// 한번 찍기 전 반드시 리셋
	for (int i = 0; i < retval; i++)
	{
		oneChar = buf[i];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("%02X(%c) ", oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("%02X ", oneChar);

		// 10개가 될 때마다 저장하고 클리어 해 준다
		if(((i % RAW_LOG_BYTE_1_LINE_SIZE) == 0) && (i != 0))
		{
			DisplayLog(strMsg);
			strMsg = "";
		}

		strMsg = strMsg + strTmp;
	}

	// 데이터는 딱 10개로 떨어지지 않을수 있으므로 나머지를 프린트 한다
	if(strMsg != "")
		DisplayLog(strMsg);
	// -------------------------------------------------------------
 #endif

	return retval;
}

//---------------------------------------------------------------------------

// 컨트롤 포트로 전송하는 경로중, 이 함수가 외부로 드러내는 유일한 경로이다
// 한 PC에서 테스트하는 경우 포트번호가 틀어지면 동작 안하게 되는데,
// 이 부분을 매우 주의해야 한다.
int UdpDiag::UnicastSend(char* buf, int len)
{
// 주의 : DIAG_LIST[0].strAddress는 진단서버(여기서는 OCS에뮬)가 사용하는 것이고,
// 		  strDIAG_SVR_Address는  OHT가 사용하는 것이다.

#ifdef IS_MCP
	if(DIAG_LIST[0].strAddress == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_OHT
	if(strDIAG_SVR_Address == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif


#ifdef IS_MCP
	#ifdef	_ONE_PC_TEST
		// 한 PC상에서 테스트 시, OHT는 +2포트를 사용하므로..
		return UnicastSend(DIAG_LIST[0].strAddress, (iDIAG_UnicastPort + 2), buf, len);
	#else
		return UnicastSend(DIAG_LIST[0].strAddress, iDIAG_UnicastPort, buf, len);
	#endif
#endif

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		// 한 PC테스트일때, OHT의 컨트롤 포트번호는 9023이 된다.
		// 따라서 OCS로 보낼 때에는 다시 9021이 되도록 2를 빼주자
		return UnicastSend(strDIAG_SVR_Address, (iDIAG_UnicastPort - 2), buf, len);
	#else
		return UnicastSend(strDIAG_SVR_Address, iDIAG_UnicastPort, buf, len);
	#endif
#endif

	return 0;
}
//---------------------------------------------------------------------------
int UdpDiag::UnicastBind()
{
	int iRetVal;

	if (pDIAG_UniCastServer == NULL)
		pDIAG_UniCastServer = new UdpServer();

	iRetVal = pDIAG_UniCastServer->Bind(iDIAG_UnicastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[진단서버 - 컨트롤 포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[진단서버 - 컨트롤 포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[진단서버 - 컨트롤 포트] Bind 동작 실패");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastBind(int iUnicastListenPort)
{
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iDIAG_UnicastPort = iUnicastListenPort + 2;
	#else
		iDIAG_UnicastPort = iUnicastListenPort;
	#endif
#else	// OCS Case
		iDIAG_UnicastPort = iUnicastListenPort;
#endif

	return UnicastBind();
}
//---------------------------------------------------------------------------
int UdpDiag::UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

	// bind여부부터 검사
	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "(9021) 데이터 수신됨. 상대 IP : " + strClientAddr;
		strTmp2.sprintf(" 상대 Port : %d 길이 : %d 내용 : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("(9021) 데이터 수신되지 않음. 리턴코드 : %d", iReturn);
		// DisplayLog(strTmp);
	}

	return iReturn;
}

//---------------------------------------------------------------------------
void UdpDiag::UnicastClose()
{
	if (pDIAG_UniCastServer != NULL)
	{
		pDIAG_UniCastServer->Close();
		delete(pDIAG_UniCastServer);
		pDIAG_UniCastServer = NULL;
	}
}

//---------------------------------------------------------------------------
// 3.3.1 진단서버가 OHT 주소를 획득한다 & 3.3.2 OHT가 진단서버 주소를 획득한다 (인수 지정하는 경우)
int UdpDiag::Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcast_DIAG_Packet);

	AddressBroadcast_DIAG_Packet Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcast_DIAG_Packet));

	// Step1. MCP가 패킷을 채워서 BroadCast
	Pkt.CommandID = ADDRESS_DIAG_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	Conv_AddressBroadcast_DIAG_Packet(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"(9020) 주소획득 Broadcast메시지 전송 실패");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"(9020) 주소획득 Broadcast메시지 전송 성공");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다 (현재 디폴트로 적용하는 경우)
int UdpDiag::Address_Broadcast()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast(iDIAG_MachineType, strMyMachineId, (iDIAG_BroadCastPort - 2));
#else
	return Address_Broadcast(iDIAG_MachineType, strMyMachineId, iDIAG_BroadCastPort);
#endif
}

//---------------------------------------------------------------------------
#ifdef IS_MCP
int UdpDiag::DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];

	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) 데이터가 정의된 패킷규격에 맞는 것인지 검증한다.
		// (1-1) 패킷이 주소획득용 패킷 크기인지 확인. 아니라면 그냥 리턴
		iPacketSize = sizeof(AddressBroadcast_DIAG_Packet);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(9020) B/C 수신패킷의 크기가 상이합니다. 정의size : %d 수신size : %d", iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcast_DIAG_Packet Pkt;
		memcpy(&Pkt, &buf, iPacketSize);

		Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert

		// (1-2) OHT가 OCS에게 보낸 내용인지 확인한다.
		if((Pkt.CommandID != ADDRESS_DIAG_BROADCAST_DEF) && (Pkt.CommandID != ADDRESS_DIAG_UNICAST_REPLY))
		{
			strTmp.sprintf("(9020) B/C 수신패킷의 내용이 상이합니다 (identify : %d)", Pkt.CommandID);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		// 이 부분은 OCS용 코드이며, 여기를 지나간다는 건 VHL가 보낸 거라는 뜻
		if((Pkt.MachineType != MACHINETYPE_DIAG_NORMAL_OHT)	&& (Pkt.MachineType != MACHINETYPE_DIAG_CLEAN_OHT))	
		{	// 이 부분에 들어오는 경우는 다른 OCS가 주소획득 B/C를 한 것을 수신한 경우
			strTmp.sprintf("(9020) 다른 OCS의 VHL주소요청 B/C (MachineType : %d)", Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST;
		}

		// (2) 검증이 끝나면 여기서 해석하여 정보저장
		//	이 부분은 사실 좀더 보강이 필요하지만, 일단 이렇게만 해 놓고 기능을 보강하자
		//	에뮬레이터는 1대만 제어할 것이므로 간단하게 이렇게 해놓자
		DIAG_LIST[0].strAddress = strClientAddr;
		memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
		cTemp[MACHINE_ID_LENGTH] = '\0';
		DIAG_LIST[0].strMachineID = cTemp;
		
		strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s - Registed",
						Pkt.CommandID, Pkt.MachineType, DIAG_LIST[0].strAddress, DIAG_LIST[0].strMachineID);
		DisplayLog(strTmp);

		// (3) DIAG Server이 OCS를 찾는 Broadcast에 한해 응답해야 한다.
		if (Pkt.CommandID == ADDRESS_DIAG_BROADCAST_DEF)
		{
			memset(&Pkt, 0, iPacketSize);
			Pkt.CommandID = ADDRESS_DIAG_UNICAST_REPLY;
			Pkt.MachineType = iDIAG_MachineType;
			
			memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID)); 		
			memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());			
	
						
			// (4) DIAG Server로 응답 패킷을 전송하고 끝낸다
			Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert
			UnicastSend(DIAG_LIST[0].strAddress, iPort, (char*)&Pkt, iPacketSize);
		}
		return BROADCAST_LISTEN_RESULT_SUCCESS;
	}
	else
		return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

// 이 함수는 어렵다. 우선, 이 모듈 초기설계상 자신의 정보만 초기값을 가진다.
// 포트번호는 상대방 것이 필요하며, OCS는 포트 피하기 기능이 없어 처리가 필요
// OHT는 반대로 한다
#ifdef IS_MCP
int UdpDiag::DIAG_BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort + 2));	// 상대방이 비켜 줬다고 보자 
	#else																				// (Diag Svr : 9020, OHT : 9022)
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort));
	#endif
}
#endif
#ifdef IS_OHT
int UdpDiag::DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];

	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) 데이터가 정의된 패킷규격에 맞는 것인지 검증한다.
		// (1-1) 패킷이 주소획득용 패킷 크기인지 확인. 아니라면 그냥 리턴
		iPacketSize = sizeof(AddressBroadcast_DIAG_Packet);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(9020) B/C 수신패킷의 크기가 상이합니다. 정의size : %d 수신size : %d", iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcast_DIAG_Packet Pkt;
		memcpy(&Pkt, &buf, iPacketSize);
		Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert

		// (1-2) OCS가 OHT에게 보낸 내용인지 확인한다.
		if((Pkt.CommandID != ADDRESS_DIAG_BROADCAST_DEF) && (Pkt.CommandID != ADDRESS_DIAG_UNICAST_REPLY))
		{
			strTmp.sprintf("(9020) B/C 수신패킷의 내용이 상이합니다 (CommandID : %d)", Pkt.CommandID);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		if(Pkt.MachineType != MACHINETYPE_DIAG_SVR)		// 이 부분은 OHT용 코드이며, 여기를 지나간다는 건 MCP가 보낸 거라는 뜻
		{	// 나는 OHT인데, 다른 OHT의 B/C 송신내용을 들었다. (그럼 무시해줘야 한다)
			strTmp.sprintf("(9020) 다른 VHL의 Diag SVR 주소요청 B/C (MachineType : %d)", Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST;
		}

		// (2) 검증이 끝나면 여기서 해석하여 정보저장
		// 받은 케이스에 따라 분기
		switch(Pkt.CommandID)
		{
			case ADDRESS_DIAG_BROADCAST_DEF:	// DIAG SVR가 OHT를 탐색한 경우 => 수신하면 기록후 응답한다
				{
					strDIAG_SVR_Address = strClientAddr;
					bDIAG_SVR_Connected = true;

 					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strDIAG_SVR_MachineID = cTemp;

					strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s => Registed",
									Pkt.CommandID, Pkt.MachineType, strDIAG_SVR_Address, strDIAG_SVR_MachineID);
					DisplayLog(strTmp);

					memset(&Pkt, 0, iPacketSize);
					Pkt.CommandID = ADDRESS_DIAG_UNICAST_REPLY;
					Pkt.MachineType = iDIAG_MachineType;
					memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
					memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
					
					// (4) OCS로 응답 패킷을 전송하고 끝낸다
					Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert
					UnicastSend(strDIAG_SVR_Address, iPort, (char*)&Pkt, iPacketSize);

					return BROADCAST_LISTEN_RESULT_SUCCESS;
				}
			//break;
			
			case ADDRESS_DIAG_UNICAST_REPLY:		// CLW가 MCP를 탐색한 경우 => 정보를 저장만 한다
				{
					strDIAG_SVR_Address = strClientAddr;
					bDIAG_SVR_Connected = true;

					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strDIAG_SVR_MachineID = cTemp;

					strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s => Registed",
									Pkt.CommandID, Pkt.MachineType, strDIAG_SVR_Address, strDIAG_SVR_MachineID);
					DisplayLog(strTmp);
				}
			break;			
			}
		
			return BROADCAST_LISTEN_RESULT_SUCCESS;
		}
		else
			return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

#ifdef IS_OHT
bool UdpDiag::DIAG_Svr_Connected()
{
	return bDIAG_SVR_Connected;
}
#endif

#ifdef IS_OHT
int UdpDiag::DIAG_BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort - 2));	// 내가 비켜 줬다고 보자 (OCS : 9000, OHT : 9002), 즉 2를 빼줘야 함
	#else
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort));
	#endif
}
#endif
#ifdef IS_MCP
int UdpDiag::DIAG_UniCast_ThreadCall()
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	// unsigned char oneChar;
	//unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;
	// int iBig, iSmall;
	
	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{

		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(DIAG_PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("[OHT->진단서버 제어] 컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 OHT에서 온 건지 확인도 해야한다.
		DIAG_PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다
		Conv_DIAG_PKT_CHECK_TAG(Pkt);

		switch(Pkt.HEADER.CommandID)
		{
			case DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE:	// 연결 확인 응답 (EQ -> 진단 Agent)
			{
				DIAG_PKT_CONNECT_CHK_RESPONSE	CmdPkt;		// 연결 확인 응답 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 연결 확인 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 연결 확인 응답 (EQ -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::DIAG_INFO_DATA_RESPONSE:	// 통계정보보고 (일반 OHT -> 진단 Agent)
			{
				DIAG_PKT_INFO_DATA_RESPONSE	CmdPkt;		// 통계정보보고 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_INFO_DATA_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 통계정보보고 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_INFO_DATA_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 통계정보보고 (EQ -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::DIAG_DATA_RESET_RESPONSE:	// 통계정보 초기화 응답 (일반 OHT -> 진단 Agent)
			{
				DIAG_PKT_DATA_RESET_RESPONSE	CmdPkt;		// 통계정보 초기화 응답 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_DATA_RESET_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 통계정보 초기화 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_RESET_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 통계정보 초기화 응답 (EQ -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}


			case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_RESPONSE:	// 통계정보 기준 변경 응답 (일반 OHT -> 진단 Agent)
			{
				DIAG_PKT_LEVEL_CHANGE_RESPONSE	CmdPkt;		// 연결 확인 응답 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 통계정보 기준 변경 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_LEVEL_CHANGE_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 통계정보 기준 변경 응답 (일반 OHT -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::CLEAN_CLW_TO_AGENT_LINK_INFO:	// 링크정보보고 (SEMES사 청소 OHT -> 진단 Agent)
			{
				DIAG_PKT_CLEAN_LINK_INFO	CmdPkt;		// 링크정보보고 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_CLEAN_LINK_INFO);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 링크정보보고 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CLEAN_LINK_INFO(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 링크정보보고 (일반 OHT -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}	

			case DiagCmdID::DIAG_INFO_STATISTICS_REQUEST:	// 통계정보 요구 (일반 OHT -> 진단 Agent)
			{
				DIAG_PKT_DATA_INFO_REQUEST	CmdPkt;		// 통계정보 요청 (일반 OHT -> 진단 Agent) 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_DATA_INFO_REQUEST);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->진단서버 제어] 통계정보 요구 (일반 OHT -> 진단 Agent) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_INFO_REQUEST(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->진단서버 제어] 통계정보 요구 (일반 OHT -> 진단 Agent) (일반 OHT -> 진단 Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}	


			default:
			{
				strTmp.sprintf("[OHT->진단서버 제어] CommandID 필드에 해석 불가능한 값이 들어왔습니다. CommandID : %d 보낸주소 : %s 보낸 포트 : %d",
								Pkt.HEADER.CommandID, strClientAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;



}
#endif

#ifdef IS_OHT
int UdpDiag::DIAG_UniCast_ThreadCall(PKT_OHT2DIAG&	OcsCmdPkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv;

	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(DIAG_PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("[진단서버->OHT 제어] 컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 DIAG SVR 에서 온 건지 확인도 해야한다.
		DIAG_PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다
		Conv_DIAG_PKT_CHECK_TAG(Pkt);

		switch(Pkt.HEADER.CommandID)	// 명령어 종류에 따른 분기
		{
			case DiagCmdID::AGENT_TO_CLW_CONNECT_CHK_REQUEST:	// 연결 확인 요청 (진단 Agent -> EQ)
			{
				DIAG_PKT_CONNECT_CHK_REQUEST	CmdPkt; 	// 연결 확인 요청 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[진단서버->OHT 제어] 연결 확인 요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CONNECT_CHK_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[진단서버->OHT 제어] 연결 확인 요청 (진단 Agent -> EQ) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// 진단서버로 응답 패킷을 전송
				DIAG_PKT_CONNECT_CHK_RESPONSE Pkt;
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE);	
				memset(&Pkt, 0, iPacketSize);
				Pkt.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
				Pkt.HEADER.CommandID = DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE;	// 연결확인 응답
				
				memset(&Pkt.HEADER.MachineID, ASCII_SPACE, sizeof(Pkt.HEADER.MachineID));			
				memcpy(&Pkt.HEADER.MachineID, strMyMachineId.c_str(), strMyMachineId.Length());
						
				// 진단서버로 응답 패킷을 전송
				Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(Pkt);	// Endian Convert
				// UnicastSend(strDIAG_SVR_Address, iDIAG_UnicastPort, (char*)&Pkt, iPacketSize);
				UnicastSend((char*)&Pkt, iPacketSize);

				// 복사하여 상위로 전달
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}
			
			case DiagCmdID::DIAG_INFO_DATA_REQUEST:	// 통계정보보고 요청 (진단 Agent -> 일반 OHT)
			{
				DIAG_PKT_INFO_DATA_REQUEST	CmdPkt; 	// 통계정보보고 요청 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_INFO_DATA_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[진단서버->OHT 제어] 통계정보보고 요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_INFO_DATA_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[진단서버->OHT 제어] 통계정보보고 요청 (진단 Agent -> 일반 OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// 복사하여 상위로 전달
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::DIAG_DATA_RESET_REQUEST:	// 통계정보 초기화 요청 (진단 Agent -> 일반 OHT)
			{
				DIAG_PKT_DATA_RESET_REQUEST	CmdPkt; 	// 통계정보 초기화 요청 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_DATA_RESET_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[진단서버->OHT 제어] 통계정보 초기화 요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_RESET_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[진단서버->OHT 제어] 통계정보 초기화 요청 (진단 Agent -> 일반 OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// 복사하여 상위로 전달
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_REQUEST:	// 통계정보 기준 변경 요청 (진단 Agent -> 일반 OHT)
			{
				DIAG_PKT_LEVEL_CHANGE_REQUEST	CmdPkt; 	// 통계정보 기준 변경 요청 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_LEVEL_CHANGE_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[진단서버->OHT 제어] 통계정보 기준 변경 요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_LEVEL_CHANGE_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[진단서버->OHT 제어] 통계정보 기준 변경 요청 (진단 Agent -> 일반 OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// 복사하여 상위로 전달
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			case DiagCmdID::DIAG_INFO_STATISTICS_RESPONSE:	// 통계정보 응답 (진단 Agent -> 일반 OHT)
			{
				DIAG_PKT_DATA_INFO_RESPONSE	CmdPkt; 	// 통계정보 응답 패킷으로 재 복사한다
				iPacketSize = sizeof(DIAG_PKT_DATA_INFO_RESPONSE);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[진단서버->OHT 제어] 통계정보 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_INFO_RESPONSE(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[진단서버->OHT 제어] 통계정보 응답 (진단 Agent -> 일반 OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// 복사하여 상위로 전달
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}


			default:
			{
				strTmp.sprintf("[진단서버->OHT 제어] CommandID필드에 해석 불가능한 값이 들어왔습니다. Tag ID : %d 보낸주소 : %s 보낸 포트 : %d",
								Pkt.HEADER.CommandID, strClientAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}
		}	
	}
	
	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

//---------------------------------------------------------------------------

/*
void UdpDiag::DiagInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort)
{
	iDIAG_MachineType = iMachineType;
	strMyMachineId = strMachineId;
	iDIAG_BroadCastPort = iBroadCastPort;
	iDIAG_UnicastPort = iUnicastPort;
}
*/
//---------------------------------------------------------------------------

#ifdef IS_OHT
// OHT -> AS Data send
int UdpDiag::UnicastSend_OHT_2_AS(char* buf, int len)
{
	UnicastSend(OHT_AS__LOCAL_IP, OHT_AS__AS_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif
//---------------------------------------------------------------------------

#ifdef IS_AS
// AS -> OHT Data send
int UdpDiag::UnicastSend_AS_2_OHT(char* buf, int len)
{
	UnicastSend(OHT_AS__LOCAL_IP, OHT_AS__OHT_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif

#ifdef IS_OHT
// OHT -> AGENT Data send
int UdpDiag::UnicastSend_OHT_2_AGENT(char* buf, int len)
{
	UnicastSendAGENT(OHT_AGENT__LOCAL_IP, OHT_AGENT__AGENT_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif
//---------------------------------------------------------------------------
// OHT와 어시스턴스의 수신서버 바인드 동작
// 함수명은 동일하고, 내부 포트번호만 다르다
int UdpDiag::OHT_AS_Bind()
{
	int iRetVal = -1;

	if(pUdpAsServer == NULL)
		pUdpAsServer = new UdpServer();

#ifdef IS_OHT
		iRetVal = pUdpAsServer->Bind(OHT_AS__OHT_PORT_NUM);

		if(iRetVal == 0)
		{
			DisplayLog(L"[OHT - AS Server 포트] Bind 동작 성공");
		}
		else if(iRetVal == -3)
		{
			DisplayLog(L"[OHT - AS Server 포트] 이미 Bind 되어 있습니다");
		}
		else
		{
			DisplayLog(L"[OHT - AS Server 포트] Bind 동작 실패");
		}
#endif

#ifdef IS_AS
		iRetVal = pUdpAsServer->Bind(OHT_AS__AS_PORT_NUM);

		if(iRetVal == 0)
		{
			DisplayLog(L"[OHT - AS Server 포트] Bind 동작 성공");
		}
		else if(iRetVal == -3)
		{
			DisplayLog(L"[OHT - AS Server 포트] 이미 Bind 되어 있습니다");
		}
		else
		{
			DisplayLog(L"[OHT - AS Server 포트] Bind 동작 실패");
		}
#endif

	return iRetVal;
}

//---------------------------------------------------------------------------
void UdpDiag::OHT_AS_Close()
{
	if (pUdpAsServer != NULL)
	{
		pUdpAsServer->Close();
		delete(pUdpAsServer);
		pUdpAsServer = NULL;
	}


}


//---------------------------------------------------------------------------
// OHT와 AGENT의 수신서버 바인드 동작
// 함수명은 동일하고, 내부 포트번호만 다르다
int UdpDiag::OHT_AGENT_Bind()
{
	int iRetVal = -1;

	if(pUdpAgentServer == NULL)
		pUdpAgentServer = new UdpServer();

#ifdef IS_OHT
		iRetVal = pUdpAgentServer->Bind(OHT_AGENT__OHT_PORT_NUM);

		if(iRetVal == 0)
		{
			//DisplayLog(L"[OHT - AGENT Server 포트] Bind 동작 성공");
			ADD_LOG("[OHT - AGENT Server 포트] Bind 동작 성공");
		}
		else if(iRetVal == -3)
		{
			//DisplayLog(L"[OHT - AGENT Server 포트] 이미 Bind 되어 있습니다");
			ADD_LOG("[OHT - AGENT Server 포트] 이미 Bind 되어 있습니다");
		}
		else
		{
			//DisplayLog(L"[OHT - AGENT Server 포트] Bind 동작 실패");
			ADD_LOG("[OHT - AGENT Server 포트] Bind 동작 실패");
		}
#endif

	return iRetVal;
}

//---------------------------------------------------------------------------
void UdpDiag::OHT_AGENT_Close()
{
	if (pUdpAgentServer != NULL)
	{
		pUdpAgentServer->Close();
		delete(pUdpAgentServer);
		pUdpAgentServer = NULL;
	}


}

//---------------------------------------------------------------------------
// OHT에서 AS의 패킷을 수신하는 함수임.

//---------------------------------------------------------------------------
// OHT에서 AS의 패킷을 수신하는 함수임. 

#ifdef IS_OHT
int UdpDiag::OHT_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	// char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv, strChk;

	iReturn = pUdpAsServer->Listen(buf, iLen, strClientAddr, iClientPort);	// 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다

		switch(Pkt.TAG.Value)
		{
			case TagID::RESPONSE_DATATRANSFER_OHT2OCS:			// 데이터 전송 지시 응답     --- UdpCom 1387 line.....?
			{
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				// --- Log ---
				strTmp.sprintf("데이터 전송 지시 응답 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  접수불가 : %d  접수불가구분 : %d  전송지시 ID : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
				DisplayLog(strTmp);	

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;
			
			case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:	// 데이터 전송 진척 보고
			{
				PKT_DATA_TRANSPER_ORDER_PROGRESS CmdPkt;
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				// --- Log ---
				strTmp.sprintf("데이터 전송 진척 보고 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  전송실패 : %d  전송성공 : %d  접수불가구분 : %d  전송지시 ID : %d  전송실패구분 : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
												CmdPkt.BODY.FailClass);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다

			}
			break;
			
			case TagID::CLW_TO_MCP_STATUS:						// CLW Status 보고 (CLW -> MCP)
			{
				PKT_STATUS_REPORT_ID_03 CmdPkt;
				iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);

				// --- Log ---
				strTmp.sprintf("CLW Status 보고 (AS => OHT) Tag : %d  데이터 전송지시 ID : %d  데이터 전송 구분 : %d  데이터 전송상태 구분 : %d  Data 전송 성공 : %d  Data 전송 실패 : %d",
								CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
								CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::REQ_DATATRANSFER_OHT2OCS:				// 데이터 전송 요구
			{
				PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

							

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);	
				 strTmp.sprintf("데이터 전송 진척 보고 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				 DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);
				
				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize); 
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);

				return iReturn;	
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT:
			{
				PKT_FTP_PUT_GET_RESULT_AS2OHT CmdPkt;
			
				iPacketSize = sizeof(PKT_FTP_PUT_GET_RESULT_AS2OHT);
			
				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}
			
				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);
			
				return iReturn;
			}
			break;

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

//---------------------------------------------------------------------------
// OCS보낸 패킷이 OHT를 거쳐 들어오는 부분. 패킷의 타입은 OCS
#ifdef IS_AS
int UdpDiag::OHT_AS_ThreadCall(PKT_OCS2OHT& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

//	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv, strChk;

	iReturn = pUdpAsServer->Listen(buf, iLen, strClientAddr, iClientPort);	// 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다

		switch(Pkt.TAG.Value)
		{
			case TagID::CMD_DATATRANSFER_OCS2OHT:		// 데이터 전송지시
			{
				PKT_DATA_TRANSPER_ORDER_OCS2OHT CmdPkt;
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}				

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("Packet Received. 데이터 전송지시");
				DisplayLog(strTmp);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);
				return iReturn;
			}
			break;


			case TagID::CMD_INERNAL_PING_REQ:			// 내부 프로그램들끼리 핑을 보내고 받아서 연결여부를 확인한다
			{

				return iReturn;
			}

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

//---------------------------------------------------------------------------
// OHT에서 AGENT의 패킷을 수신하는 함수임.

#ifdef IS_OHT
int UdpDiag::OHT_AGENT_ThreadCall(PKT_OHT2OCS& AGENT_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	AnsiString strConv, strChk;

	iReturn = pUdpAgentServer->Listen(buf, iLen, strClientAddr, iClientPort);	// 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		strTmp.sprintf("Agent Packet Received. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
		DisplayLog(strTmp);
		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다

		switch(Pkt.TAG.Value)
		{
			case TagID::RESPONSE_DATATRANSFER_OHT2OCS:			// 데이터 전송 지시 응답     --- UdpCom 1387 line.....?
			{
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				// --- Log ---
				strTmp.sprintf("데이터 전송 지시 응답 (AGENT => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  접수불가 : %d  접수불가구분 : %d  전송지시 ID : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:	// 데이터 전송 진척 보고
			{
				PKT_DATA_TRANSPER_ORDER_PROGRESS CmdPkt;
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				// --- Log ---
				strTmp.sprintf("데이터 전송 진척 보고 (AGENT => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  전송실패 : %d  전송성공 : %d  접수불가구분 : %d  전송지시 ID : %d  전송실패구분 : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
												CmdPkt.BODY.FailClass);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다

			}
			break;

			case TagID::CLW_TO_MCP_STATUS:						// CLW Status 보고 (CLW -> MCP)
			{
				PKT_STATUS_REPORT_ID_03 CmdPkt;
				iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				// Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);

				// --- Log ---
				strTmp.sprintf("CLW Status 보고 (AS => OHT) Tag : %d  데이터 전송지시 ID : %d  데이터 전송 구분 : %d  데이터 전송상태 구분 : %d  Data 전송 성공 : %d  Data 전송 실패 : %d",
								CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
								CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::REQ_DATATRANSFER_OHT2OCS:				// 데이터 전송 요구
			{
				PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				 strTmp.sprintf("데이터 전송 진척 보고 (AGENT => OHT) Tag : %d  전송구분 : %d  요구구분 : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				 DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AGENT => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}
			break;

			case TagID::CMD_FIRMWARE_VERSION_REQUEST_AGENT2OHT:
			{
				PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT CmdPkt;

				iPacketSize = sizeof(PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("AGENT requested Firmware Version (AGENT => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}

		}

		memcpy(&AGENT_Pkt, &buf, sizeof(PKT_OHT2OCS));
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

