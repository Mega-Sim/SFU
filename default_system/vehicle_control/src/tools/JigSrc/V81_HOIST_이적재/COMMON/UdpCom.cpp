#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpCom.h"
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


//#include "OHTMain.h"

 #ifdef IS_OHT
	#include "OhtMain.h"
 #endif

#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif
#ifdef IS_TP
	#include "MainUnit.h"
	#include "SocketUnit.h"
#endif

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


        네트워크 사정으로 임의주소를 OHT와 OCS에뮬에 할당하는법 : _K_DEMO define을 설정하고, Protocol_define 파일에 원하는 값을 설정
*/

//---------------------------------------------------------------------------

SocketInitClose		*pSocketInitClose;		// 소켓의 초기화 기능

UdpServer 			*pUdpBroadCastServer;	// 주소획득 프로토콜 처리
UdpServer 			*pUdpUniCastServer;		// 주소획득 및 제어 프토토콜 처리
UdpServer 			*pUdpTpServer;			// TP통신 전용

UdpBroadCastSender  *pUdpBroadCastSender;	// 브로드케스트 전송처리
UdpUniCastSender	*pUdpUniCastSender; 	// 일대일 통신 전송처리

VHL_INFO VHL_LIST[50];
// char PacketBuf[MAX_PACKET_SIZE];	// 작업용 임시 버퍼. 이걸로 일관성있게 쓰자

//---------------------------------------------------------------------------
UdpCom::UdpCom(int PortNum)
{
	pSocketInitClose = NULL;

	pUdpBroadCastServer = NULL;
	pUdpUniCastServer = NULL;
	pUdpTpServer = NULL;

	pUdpBroadCastSender = NULL;
	pUdpUniCastSender	= NULL;

	if (pSocketInitClose == NULL)
		pSocketInitClose = new SocketInitClose();

	if (pUdpBroadCastSender == NULL)
		pUdpBroadCastSender = new UdpBroadCastSender();

	if (pUdpUniCastSender == NULL)
		pUdpUniCastSender = new UdpUniCastSender();

	// 소켓 초기화 일괄적으로 해줌
	pSocketInitClose->SocketInit();

#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPCommunication.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif



#ifdef IS_MCP
	for(int i = 0; i < 50; i++)
	{
		VHL_LIST[i].strAddress = "";
		VHL_LIST[i].strMachineID = "";
	}

	iMyMachineType = MACHINETYPE_MCP;
	strMyMachineId = "MCP01";
	iMyBroadCastPort = ADDRESS_GET_DEFAULT_PORT+PortNum;
	iMyUnicastPort = CONTROL_DEFAULT_PORT+PortNum;
	#ifdef	_ONE_PC_TEST
		ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
	#endif
#endif

#ifdef IS_OHT
	
	//
	// 이 부분은 CLW가 OCS와 통신하기 위해 사용하는 정보이다
	for(int i = 0; i < MAX_OCS_NUMBER; i++)
	{
		strOcsAddress[i] = "";		// OCS의 주소. 처음에는 비어있다.
		strOcsMachineID[i] = "";	// OCS의 머신 ID
	}
	iOcsCount = 0;

	// bOcsConnected = false;	// OHT가 사용. OCS와 연결되었다는 플래그

	// ocs_test_ver
	//
	/*
	strOcsAddress = "192.167.0.20";		// OCS의 주소. 처음에는 비어있다.
	bOcsConnected = true;	// OHT가 사용. OCS와 연결되었다는 플래그
	strOcsMachineID = "MCP01";	// OCS의 머신 ID
	*/

	

	iMyMachineType = MACHINETYPE_VHL;
	strMyMachineId = "OHT001";

	bTpConnected = false; 	// OHT가 사용. TP가 연결되었다는 플래그
	strTpAddr = "";

	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = (ADDRESS_GET_DEFAULT_PORT +PortNum+ 2);     // OHT인 경우 & 1PC 모드인 경우에 한해, 스스로 자신의 포트번호를 올려버림
		iMyUnicastPort = (CONTROL_DEFAULT_PORT +PortNum+ 2);
		ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
	#else
		iMyBroadCastPort = ADDRESS_GET_DEFAULT_PORT+PortNum;
		iMyUnicastPort = CONTROL_DEFAULT_PORT+PortNum;
	#endif
#endif

#ifdef IS_TP
	strOhtAddr = "";
	strTpMachineID = "TP01";
	bOhtConnected = false;	// TP가 사용하는 정보. OHT가 연결되었다 플래그

	#ifdef	_ONE_PC_TEST
		iOhtPortNum = (CONTROL_DEFAULT_PORT +PortNum+ 2);
	#else
		iOhtPortNum = CONTROL_DEFAULT_PORT+PortNum;
	#endif
#endif

	iClwCount = 0;


#ifdef IS_REAL_OHT
	BroadcastBind();
	UnicastBind();
	OHT_TP_Bind();
#endif


// shkim. Comm test (temp)
#ifdef _K_DEMO
	VHL_LIST[0].strAddress 		= K_DEMO_VHL_ADDR;
	VHL_LIST[0].strMachineID 	= K_DEMO_VHL_MACHINE_ID;
	iClwCount++;
	
	// bOcsConnected = true;	// OHT가 사용. OCS와 연결되었다는 플래그

	strOcsAddress[0] = K_DEMO_OCS_ADDR;		// OCS의 주소. 처음에는 비어있다.
	strOcsMachineID[0] = K_DEMO_OCS_MACHINE_ID;	// OCS의 머신 ID

	strOcsAddress[1] = K_DEMO_OCS_ADDR_2;		// OCS의 주소. 처음에는 비어있다.
	strOcsMachineID[1] = K_DEMO_OCS_MACHINE_ID_2;	// OCS의 머신 ID

	iOcsCount = MAX_OCS_NUMBER;

    AnsiString strInfo;
	strInfo.sprintf("IP 직접지정 모드입니다!! \r\n OHT주소 : %s OHT 이름 : %s  \r\n  OCS1주소 : %s OCS1이름 : %s  OCS2주소 : %s OCS2이름 : %s \r\n ",
        	VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, strOcsAddress[0], strOcsMachineID[0], strOcsAddress[1], strOcsMachineID[1]);
        ShowMessage(strInfo);
#endif

// --------------------------------------------------------------------------------------------
/*
	프로토콜 지원버전을 확인한다
*/
#ifdef IS_OHT
	#ifdef IS_REAL_OHT 	// OHT Main
		if(SUPPORT_PROTOCOL_VER < OHT_REAL_VER)
			ShowMessage(L"프로토콜 지원 버전보다 OHT Main코드 버전이 높습니다");
	#else 			// OHT emulator
		if(SUPPORT_PROTOCOL_VER < OHT_EMUL_VER)
			ShowMessage(L"프로토콜 지원 버전보다 OHT Emul코드 버전이 높습니다");
	#endif
#endif

#ifdef IS_MCP 		// OCS emulator
		if(SUPPORT_PROTOCOL_VER < OCS_EMUL_VER)
			ShowMessage(L"프로토콜 지원 버전보다 OCS Emul코드 버전이 높습니다");
#endif


#ifdef IS_TP 		// note TP
		if(SUPPORT_PROTOCOL_VER < NOTE_TP_VER)
			ShowMessage(L"프로토콜 지원 버전보다 TP코드 버전이 높습니다");
#endif

#ifdef IS_AS 		// 어시스턴스
		if(SUPPORT_PROTOCOL_VER < ASSISTANCE_VER)
			ShowMessage(L"프로토콜 지원 버전보다 어시스턴스 코드 버전이 높습니다");
#endif

#ifdef IS_FM 		// 파일매니저
		if(SUPPORT_PROTOCOL_VER < FILEMANAGER_VER)
			ShowMessage(L"프로토콜 지원 버전보다 파일매니저 코드 버전이 높습니다");
#endif
// --------------------------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
void UdpCom::DisplayLog(String strLog)
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

UdpCom::~UdpCom()
{
	pSocketInitClose->SocketClose();

	if (pSocketInitClose != NULL)
		delete(pSocketInitClose);

	if (pUdpBroadCastSender != NULL)
		delete(pUdpBroadCastSender);

	if (pUdpUniCastSender != NULL)
		delete(pUdpUniCastSender);


}
//---------------------------------------------------------------------------

int UdpCom::BroadcastSend(int iPort, AnsiString strMessage)
{
	String strMsg;
	int retval = pUdpBroadCastSender->BroadcastSendMessage(iMyBroadCastPort, strMessage);

	strMsg.sprintf(L"[UDP BroadCast] %d바이트를 보냈습니다.",retval);
	DisplayLog(strMsg);

	return retval;
}
//---------------------------------------------------------------------------
int UdpCom::BroadcastSend(int iPort, char* cMessage, int iLen)
{
	AnsiString strMsg, strTmp;
	char oneChar;

	int retval = pUdpBroadCastSender->BroadcastSendMessage(iPort,cMessage,iLen);

	strTmp =  DateTimeToStr(Now());
	strMsg.sprintf("[UDP Broadcast] (%s) PortNo : %d  %d바이트를 보냈습니다.", strTmp, iPort, retval);
	DisplayLog(strMsg);

	strMsg = "";
	for (int i = 0; i < iLen; i++)
	{
		oneChar = cMessage[i];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);

		strMsg = strMsg + strTmp;
	}
	DisplayLog(strMsg);
	

	return retval;
}

//---------------------------------------------------------------------------
int UdpCom::BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

 	// bind여부부터 검사
	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "데이터 수신됨. 상대 IP : " + strClientAddr;
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

int UdpCom::BroadcastBind()
{
	int iRetVal;

	if (pUdpBroadCastServer == NULL)
		pUdpBroadCastServer = new UdpServer();

	iRetVal = pUdpBroadCastServer->Bind(iMyBroadCastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[OCS - 주소 컨트롤 포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OCS - 주소 컨트롤 포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[OCS - 주소 컨트롤 포트] Bind 동작 실패");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpCom::BroadcastBind(int iBroadCastListenPort)
{
	// OHT/OHT에뮬레이터인 경우, ONE PC모드에서는 지정한 포트번호보다 +2한 값을
	// 포트번호로 사용한다
	// 즉, OCS : 9000번, OHT : 9002번을 사용하여 한 PC에서 동작을 가능케 한다
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = iBroadCastListenPort + 2;
	#else
		iMyBroadCastPort = iBroadCastListenPort;
	#endif
#else  	// OCS 에뮬레이터인 경우, 자신의 포트번호는 지정한 그대로 사용한다
	iMyBroadCastPort = iBroadCastListenPort;
#endif
	return BroadcastBind();
}
//---------------------------------------------------------------------------

void UdpCom::BroadcastClose()
{
	if (pUdpBroadCastServer != NULL)
	{
		pUdpBroadCastServer->Close();
		delete(pUdpBroadCastServer);
		pUdpBroadCastServer = NULL;
	}

	DisplayLog(L"Broadcast Server Closed");
}

//---------------------------------------------------------------------------
int UdpCom::UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	AnsiString strMsg;
	int retval;

	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr,iPort, strMessage);

	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d바이트를 보냈습니다.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	// int iBig, iSmall;
	unsigned char oneChar;


	if(len > MAX_BIG_PACKET_SIZE)
	{
		strMsg.sprintf("Size Over Message Send Try Error (Max Packet Size : %d)", MAX_BIG_PACKET_SIZE);
		DisplayLog(strMsg);
		return UNICAST_SEND_RESULT_FAIL;
	}


	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);	
//	strMsg.sprintf("[UDP Unicast] Target Addr : %s  PortNo : %d  %d Bytes Sended",strIpAddr, iPort, retval);
//	DisplayLog(strMsg);

//#ifdef IS_TP
//	return retval;
//#endif

	if ((iPort == TP_FIX_PORT_NUM) || (iPort == OHT_S_TP_LISTEN_PORT_NUM))
		return retval;

	strMsg = "";	// 한번 찍기 전 반드시 리셋
	for (int i = 0; i < retval; i++)
	{
		oneChar = buf[i];
		/*
		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);
		*/

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("%02X(%c) ", oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("%02X ", oneChar);
		
		// strMsg = strMsg + strTmp;

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


#if 0
	iBig = len / RAW_LOG_BYTE_1_LINE_SIZE;
	iSmall = len % RAW_LOG_BYTE_1_LINE_SIZE;

	// 10개 단위 로그저장
	for (int i = 0; i < iBig; i++)
	{
		strMsg = "";	// 한번 찍기 전 반드시 리셋
		for(int j = 0; j < RAW_LOG_BYTE_1_LINE_SIZE; j++)
		{
			oneChar = buf[(i * RAW_LOG_BYTE_1_LINE_SIZE) + j];

			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
				strTmp.sprintf("[%d]0\x%02X(%c)  ", ((i * RAW_LOG_BYTE_1_LINE_SIZE) + j), oneChar, oneChar);	// Hex표시로 바꿈
			else
				strTmp.sprintf("[%d]0\x%02X  ", (i * RAW_LOG_BYTE_1_LINE_SIZE) + j, oneChar);

			strMsg = strMsg + strTmp;
		}
		DisplayLog(strMsg);
	}

	// 1개 단위 로그저장
	strMsg = ""; 	// 한번 찍기 전 반드시 리셋
	for (int i = 0; i < iSmall; i++)
	{
		oneChar = buf[(iBig * RAW_LOG_BYTE_1_LINE_SIZE) + iSmall];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", (iBig * RAW_LOG_BYTE_1_LINE_SIZE + i), oneChar, oneChar);	// Hex표시로 바꿈
		else
			strTmp.sprintf("[%d]0\x%02X  ", (iBig * RAW_LOG_BYTE_1_LINE_SIZE + i), oneChar);

		strMsg = strMsg + strTmp;
	}
	DisplayLog(strMsg);
#endif

	return retval;

}

//---------------------------------------------------------------------------

// 컨트롤 포트로 전송하는 경로중, 이 함수가 외부로 드러내는 유일한 경로이다
// 한 PC에서 테스트하는 경우 포트번호가 틀어지면 동작 안하게 되는데,
// 이 부분을 매우 주의해야 한다.
int UdpCom::UnicastSend(char* buf, int len)
{
// 주의 : VHL_LIST[0].strAddress는 OCS가 사용하는 것이고,
// 		  strOcsAddress는 OHT가 사용하는 것이다.

#ifdef IS_MCP
	if(VHL_LIST[0].strAddress == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_OHT
	if(iOcsCount == 0)
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_TP
	if(strOhtAddr == "")
	{
		DisplayLog(L"OHT Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif


#ifdef IS_MCP
	#ifdef	_ONE_PC_TEST
		// 한 PC상에서 테스트 시, OHT는 +2포트를 사용하므로..
		UnicastSend(VHL_LIST[0].strAddress, (iMyUnicastPort + 2), buf, len);
	#else
		UnicastSend(VHL_LIST[0].strAddress, iMyUnicastPort, buf, len);
	#endif
#endif

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		// 한 PC테스트일때, OHT의 컨트롤 포트번호는 9003이 된다.
		// 따라서 OCS로 보낼 때에는 다시 9001이 되도록 2를 빼주자
		for (int i = 0; i < iOcsCount; i++) 
			UnicastSend(strOcsAddress[i], (iMyUnicastPort - 2), buf, len);
	#else
	try{
		for (int i = 0; i < iOcsCount; i++)
			UnicastSend(strOcsAddress[i], iMyUnicastPort, buf, len);
	}catch(...)
	{
    		DisplayLog(L"UnicastSend Exception!!!");
	}
	#endif
#endif

#ifdef IS_TP
	UnicastSend(strOhtAddr, iOhtPortNum, buf, len);
#endif

	return UNICAST_SEND_RESULT_SUCCESS;
}
//---------------------------------------------------------------------------

// OHT -> PC/TP Data send
int UdpCom::UnicastSend_OHT_2_TP(char* buf, int len)
{
	if(strTpAddr == "")
	{
		DisplayLog(L"PC/TP Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}

	UnicastSend(strTpAddr, TP_FIX_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}

//---------------------------------------------------------------------------

// PC/TP -> OHT Data send
/*
int UdpCom::UnicastSend_TP_2_OHT(char* buf, int len)
{
	if(strOhtAddr == "")
	{
		DisplayLog(L"PC/TP Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}

	return UnicastSend(strOhtAddr, iPort, buf, len);

} */

//---------------------------------------------------------------------------
int UdpCom::OHT_TP_Bind()		// OHT - TP의 패킷을 받기위해 사용하는 포트번호 Open
{
	int iRetVal = -1;

	if (pUdpTpServer == NULL)
		pUdpTpServer = new UdpServer();

#ifdef IS_OHT
	iRetVal = pUdpTpServer->Bind(OHT_S_TP_LISTEN_PORT_NUM);
#endif

#ifdef IS_TP
	iRetVal = pUdpTpServer->Bind(TP_FIX_PORT_NUM);
#endif

	if(iRetVal == 0)
	{
		DisplayLog(L"[OHT - TP Server 포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OHT - TP Server 포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[OHT - TP Server 포트] Bind 동작 실패");
	}

	return iRetVal;
}



//---------------------------------------------------------------------------
int UdpCom::UnicastBind()
{
	int iRetVal;

	if (pUdpUniCastServer == NULL)
		pUdpUniCastServer = new UdpServer();

	iRetVal = pUdpUniCastServer->Bind(iMyUnicastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[OCS - 제어 통신포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OCS - 제어 통신포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[OCS - 제어 통신포트] Bind 동작 실패");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastBind(int iUnicastListenPort)
{
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyUnicastPort = iUnicastListenPort + 2;
	#else
		iMyUnicastPort = iUnicastListenPort;
	#endif
#else
	iMyUnicastPort = iUnicastListenPort;
#endif

	return UnicastBind();
}
//---------------------------------------------------------------------------

// TP전용함수임. TP가 OHT에 접속할때 사용함
// OHT의 주소와 포트번호를 기록해두고, 다음번 전송부터 사용하는 목적과
// 자신의 수신포트를 열어두는 역할을 함 (TCP소켓 함수 대체를 위해 이렇게 만들어 둠)
int UdpCom::OHT_TP_Bind(AnsiString sOhtAddr, int iOhtPort)
{
	int iResult = OHT_TP_Bind();

	if(iResult == 0)
	{
		strOhtAddr  = sOhtAddr;
		iOhtPortNum = iOhtPort;
	}

	return iResult;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

	// bind여부부터 검사
	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "데이터 수신됨. 상대 IP : " + strClientAddr;
		strTmp2.sprintf(" 상대 Port : %d 길이 : %d 내용 : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("데이터 수신되지 않음. 리턴코드 : %d", iReturn);
		// DisplayLog(strTmp);
	}

	return iReturn;
}

//---------------------------------------------------------------------------

void UdpCom::OHT_TP_Close()
{
	if (pUdpTpServer != NULL)
	{
		pUdpTpServer->Close();
		delete(pUdpTpServer);
		pUdpTpServer = NULL;
	}
}
//---------------------------------------------------------------------------

void UdpCom::UnicastClose()
{
	if (pUdpUniCastServer != NULL)
	{
		pUdpUniCastServer->Close();
		delete(pUdpUniCastServer);
		pUdpUniCastServer = NULL;
	}
}


//---------------------------------------------------------------------------
// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다 (인수 지정하는 경우)
int UdpCom::Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort)
{
#ifdef IS_OHT
	// OHT에 이중화 통신이 적용되면서 추가됨 (OHT에만 적용)
	for(int i = 0; i < MAX_OCS_NUMBER; i++)
	{
		strOcsAddress[i] = "";		// OCS의 주소. 처음에는 비어있다.
		strOcsMachineID[i] = "";	// OCS의 머신 ID
	}
	iOcsCount = 0;

	// bOcsConnected = false;
#endif

	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP가 패킷을 채워서 BroadCast
	Pkt.identify = ADDRESS_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"주소획득 Broadcast메시지 전송 실패");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"주소획득 Broadcast메시지 전송 성공");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다 (인수 지정하는 경우)
// - 주소획득시 기존 OCS주소를 지우는데, 지우는 동작만 하지 않도록 하는 함수
int UdpCom::Address_Broadcast_NotClear(int iMachineType, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP가 패킷을 채워서 BroadCast
	Pkt.identify = ADDRESS_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"주소획득 Broadcast메시지 전송 실패");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"주소획득 Broadcast메시지 전송 성공");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다 (현재 디폴트로 적용하는 경우)
int UdpCom::Address_Broadcast()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
	return Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
}

//---------------------------------------------------------------------------
// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다 (현재 디폴트로 적용하는 경우)
// - 주소획득시 기존 OCS주소를 지우는데, 지우는 동작만 하지 않도록 하는 함수
int UdpCom::Address_Broadcast_NotClear()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast_NotClear(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
	return Address_Broadcast_NotClear(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
}

//---------------------------------------------------------------------------
// 3.3.3 MCP->CLW, Address drop order, Unicast
int UdpCom::Address_DropOrder_ToClw(AnsiString strIpAddr, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP가 패킷을 채워서 BroadCast
	Pkt.identify = ADDRESS_EXPIRE_ORDER;
	Pkt.MachineType = MACHINETYPE_MCP;

	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = UnicastSend(strIpAddr, iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
		return UNICAST_SEND_RESULT_FAIL;
	else
		return UNICAST_SEND_RESULT_SUCCESS;
}
//---------------------------------------------------------------------------
int UdpCom::Address_DropOrder_ToClw() 	// OHT 1대와 통신하므로, 간단하게 이렇게 인터페이스 함
{
#ifdef	_ONE_PC_TEST
	return Address_DropOrder_ToClw(VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, (iMyBroadCastPort + 2));
#else
	return Address_DropOrder_ToClw(VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, iMyBroadCastPort);
#endif
}
//---------------------------------------------------------------------------

// 3.3.4 MCP->All CLW, Address drop order, Broadcast
int UdpCom::Address_DropOrder_AllClw(AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, iPacketLength);

	// Step1. MCP가 패킷을 채워서 BroadCast
	Pkt.identify = ADDRESS_EXPIRE_ALL;
	Pkt.MachineType = MACHINETYPE_MCP;

	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);
	
	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
		return BROADCAST_SEND_RESULT_FAIL;
	else
		return BROADCAST_SEND_RESULT_SUCCESS;

}

//---------------------------------------------------------------------------
int UdpCom::Address_DropOrder_AllClw()	// OHT 1대와 통신하므로, 간단하게 이렇게 인터페이스 함
{
#ifdef	_ONE_PC_TEST
	return Address_DropOrder_AllClw(strMyMachineId, (iMyBroadCastPort + 2));
#else
	return Address_DropOrder_AllClw(strMyMachineId, iMyBroadCastPort);
#endif
}
//---------------------------------------------------------------------------

#ifdef IS_MCP
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)	// strMachineID : 자신의 MachineID, iPort : 상대방 포트번호 (주의!!)
{
	AnsiString strMsg, strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];
	char oneChar1, oneChar2;

	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strMsg =  DateTimeToStr(Now());
		
		// (1) 데이터가 정의된 패킷규격에 맞는 것인지 검증한다.
		// (1-1) 패킷이 주소획득용 패킷 크기인지 확인. 아니라면 그냥 리턴
		iPacketSize = sizeof(AddressBroadcastPacket);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(%s) B/C 수신패킷의 크기가 상이합니다. 정의size : %d 수신size : %d", strMsg, iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcastPacket Pkt;
		memcpy(&Pkt, &buf, iPacketSize);

		Conv_AddressBroadcastPacket(Pkt);	// Endian Convert

		// (1-2) OHT가 OCS에게 보낸 내용인지 확인한다.
		if((Pkt.identify != ADDRESS_BROADCAST_DEF) && (Pkt.identify != ADDRESS_UNICAST_REPLY))
		{
			strTmp.sprintf("(%s) B/C 수신패킷의 내용이 상이합니다 (identify : %d)", strMsg, Pkt.identify);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

        // VHL 기종 세분화로 인해 Logic 변경
//		if(Pkt.MachineType != MACHINETYPE_VHL)		// 이 부분은 OCS용 코드이며, 여기를 지나간다는 건 VHL가 보낸 거라는 뜻
		if((Pkt.MachineType != MACHINETYPE_VHL_COMMON) && (Pkt.MachineType != MACHINETYPE_VHL_RATICLE) && (Pkt.MachineType != MACHINETYPE_VHL_PATROL))
		{   // 이 부분에 들어오는 경우는 다른 OCS가 주소획득 B/C를 한 것을 수신한 경우
			strTmp.sprintf("(%s) 다른 OCS의 VHL주소요청 B/C (MachineType : %d)", strMsg, Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST;
		}

		// (2) 검증이 끝나면 여기서 해석하여 정보저장
		//  이 부분은 사실 좀더 보강이 필요하지만, 일단 이렇게만 해 놓고 기능을 보강하자
		//  에뮬레이터는 1대만 제어할 것이므로 간단하게 이렇게 해놓자
		VHL_LIST[0].strAddress = strClientAddr;
		memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
		cTemp[MACHINE_ID_LENGTH] = '\0';
		VHL_LIST[0].strMachineID = cTemp;
		iClwCount++;

		oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
		oneChar2 = Pkt.MachineType % 256;	// Lower Byte
		
		strTmp.sprintf("(%s) Packet Identify : 0\x%02X(%c) Machine Type : 0\x%02X(%c)(%c) Address : %s Machine ID : %s - Registed",
						strMsg, Pkt.identify, Pkt.identify, Pkt.MachineType, oneChar1, oneChar2, VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID);
		DisplayLog(strTmp);

		// (3) VHL이 OCS를 찾는 Broadcast에 한해 응답해야 한다.
		if (Pkt.identify == ADDRESS_BROADCAST_DEF)
		{
			memset(&Pkt, 0, iPacketSize);
			Pkt.identify = ADDRESS_UNICAST_REPLY;
			Pkt.MachineType = MACHINETYPE_MCP;
			
			memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));			
			memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());			
			// memcpy(&PacketBuf, &Pkt, iPacketSize);

						
			// (4) OCS로 응답 패킷을 전송하고 끝낸다
			Conv_AddressBroadcastPacket(Pkt);	// Endian Convert
			UnicastSend(VHL_LIST[0].strAddress, iPort, (char*)&Pkt, iPacketSize);
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
int UdpCom::BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort + 2));	// 상대방이 비켜 줬다고 보자 (OCS : 9000, OHT : 9002)
	#else
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort));
	#endif
}
#endif


#ifdef IS_OHT

// VHL인 경우, 이 부분이 실행된다
// MCP의 주소파기 지시 등, 동작이 추가되므로 위 함수와는 분리되어야 한다.
//
// iMachineType = MACHINETYPE_MCP, MACHINETYPE_VHL
// strMachineID = 사용자가 입력해둔 장치이름
// iPort = 환경설정에 입력해둔 broadcast 포트번호 (1PC 시뮬레이션이 아니면 같이하고, 1PC면 Define에 의해 자동수정)
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];
	char oneChar1, oneChar2;
	int iReplyOcsNo;

try{

	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);
	strAnsiAddr = strClientAddr;
	strMsg = DateTimeToStr(Now());

	if (iReturn > 0)
	{
		// (1) 데이터가 정의된 패킷규격에 맞는 것인지 검증한다.
		// (1-1) 패킷이 주소획득용 패킷 크기인지 확인. 아니라면 그냥 리턴
		iPacketSize = sizeof(AddressBroadcastPacket);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(%s) B/C 수신패킷의 크기가 상이합니다. 정의size : %d 수신size : %d", strMsg, iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcastPacket Pkt;
		memcpy(&Pkt, &buf, iPacketSize);
		Conv_AddressBroadcastPacket(Pkt);	// Endian Convert

		// (1-2) OCS가 OHT에게 보낸 내용인지 확인한다.
		if((Pkt.identify != ADDRESS_BROADCAST_DEF) && (Pkt.identify != ADDRESS_UNICAST_REPLY)
			&& (Pkt.identify != ADDRESS_EXPIRE_ORDER) && (Pkt.identify != ADDRESS_EXPIRE_ALL))
		{
			strTmp.sprintf("(%s) B/C 수신패킷의 내용이 상이합니다 (identify : %d)", strMsg, Pkt.identify);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
		oneChar2 = Pkt.MachineType % 256;	// Lower Byte

		if(Pkt.MachineType != MACHINETYPE_MCP)		// 이 부분은 OHT용 코드이며, 여기를 지나간다는 건 MCP가 보낸 거라는 뜻
		{	// 나는 OHT인데, 다른 OHT의 B/C 송신내용을 들었다. (그럼 무시해줘야 한다)
			strTmp.sprintf("(%s) 다른 장치의 OCS주소요청 B/C이므로 무시함 (MachineType : 0\x%02X(%c)(%c))", strMsg, Pkt.MachineType, oneChar1, oneChar2);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST;
		}

		// 주소 중복등록 현상을 검증하기 위해 여기에 로그를 보강한다


		strTmp.sprintf("[OCS 주소 메시지 수신] (%s) Received OCS IP : %s  OCS IP Length : %d  identify : 0\x%02X(%c)",
						strMsg, strAnsiAddr, strAnsiAddr.Length(), Pkt.identify, Pkt.identify);
		DisplayLog(strTmp);

		strTmp.sprintf("[OHT 주소 현황] Ocs Count : %d  OCS1 Addr : %s  OCS1 Length : %d  OCS2 Addr : %s  OCS2 Length : %d",
						iOcsCount, strOcsAddress[0], strOcsAddress[0].Length(), strOcsAddress[1], strOcsAddress[1].Length());
		DisplayLog(strTmp);

		// (2) 검증이 끝나면 여기서 해석하여 정보저장

		// 받은 케이스에 따라 분기
		switch(Pkt.identify)
		{
			case ADDRESS_BROADCAST_DEF:	// MCP가 CLW를 탐색한 경우 => 수신하면 기록후 응답한다
				{
					// (1) 중복인 경우 => 주소를 추가로 기억할 필요는 없지만, 상대방에게 응답은 해 줄 필요가 있다.
					//                    내가 상대방을 기억한 후에, 상대방이 꺼졌다 다시 켜진 가능성이 있기 때문이다.
					bool bOverLap = false;
					
					for(int i = 0; i < iOcsCount; i++)
					{
						if(strOcsAddress[i].Trim() == strAnsiAddr.Trim())
						{
							bOverLap = true;
							iReplyOcsNo = i;
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) Received OCS Addr 중복으로 판단됨. Received Addr : %s", strAnsiAddr);
							DisplayLog(strTmp);
						}
					}

					// (2) 현재 등록된 갯수가 MAX인 경우 (중복 정보가 필요하다)
					//		 => 모르는 상대인 경우 : 주소를 기억하지 않아야 하며, 상대방에게 응답도 불필요하다
					//		 => 아는  상대인 경우 : 주소를 기억하지 않아야 하나, 상대방에게 응답은 해 줘야 한다
					if(iOcsCount == MAX_OCS_NUMBER)
					{
						strTmp.sprintf("(ADDRESS_BROADCAST_DEF) OCS Limit reached!!, can't Regist!");
						DisplayLog(strTmp);

						if(bOverLap == false)
						{
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) 중복주소가 아닌 제3의 OCS 등장임. MAX초과. 응답 및 저장하지 않고 종료함!!");
							DisplayLog(strTmp);
							return BROADCAST_LISTEN_RESULT_SUCCESS;
						}
						else
						{
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) 중복주소를 가진 OCS 메시지임. 응답만 하고 저장하지는 않음.");
							DisplayLog(strTmp);
						}
					}

					// (3) 그외의 경우 (Count = 0, 1) => 상대방 주소를 추가한다.
					if(bOverLap == false)
					{
						iOcsCount++;
						strOcsAddress[iOcsCount - 1] = strAnsiAddr.Trim();
						iReplyOcsNo = iOcsCount - 1;
						strTmp.sprintf("(ADDRESS_BROADCAST_DEF) 새로운 OCS주소 추가됨.  Addr : %s  Length : %d  할당된 OCS NO : %d",
																strAnsiAddr, strAnsiAddr.Length(), iOcsCount);
						DisplayLog(strTmp);
					}

					// (4) 리턴하지 않고 여기까지 오면 응답할 필요성이 있는 경우이다.
					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strOcsMachineID[iReplyOcsNo] = cTemp;

					memset(&Pkt, 0, iPacketSize);
					Pkt.identify = ADDRESS_UNICAST_REPLY;
					Pkt.MachineType = MACHINETYPE_VHL;
					memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
					memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

					oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
					oneChar2 = Pkt.MachineType % 256;	// Lower Byte

					strTmp.sprintf("[OCS로 응답패킷 전송] Packet Identify : 0\x%02X(%c) Machine Type : 0\x%02X(%c)(%c) Address : %s MyMachine ID : %s 응답한 OCS NO (1,2) : %d ",
									Pkt.identify, Pkt.identify, Pkt.MachineType, oneChar1, oneChar2, strOcsAddress[iReplyOcsNo], strMachineID, (iReplyOcsNo + 1));
					DisplayLog(strTmp);

					// (4) OCS로 응답 패킷을 전송하고 끝낸다
					Conv_AddressBroadcastPacket(Pkt);	// Endian Convert
					UnicastSend(strOcsAddress[iReplyOcsNo], iPort, (char*)&Pkt, iPacketSize);

					return BROADCAST_LISTEN_RESULT_SUCCESS;
				}
			//break;

			case ADDRESS_UNICAST_REPLY:		// CLW가 MCP를 탐색한 경우 => 정보를 저장만 한다
				{
					// 중복주소인지 판단하도록 하자
					for(int i = 0; i < iOcsCount; i++)
					{
						if(strOcsAddress[i].Trim() == strAnsiAddr.Trim())
						{
							strTmp.sprintf("(ADDRESS_UNICAST_REPLY) Received OCS Addr 중복으로 판단됨 . Received Addr : %s", strAnsiAddr);
							DisplayLog(strTmp);

							return BROADCAST_LISTEN_RESULT_SUCCESS;
						}
					}

					if(iOcsCount == MAX_OCS_NUMBER)
					{
						strTmp.sprintf("(ADDRESS_UNICAST_REPLY) OCS Limit reached!!, can't Regist!");
						DisplayLog(strTmp);
						return BROADCAST_LISTEN_RESULT_SUCCESS;
					}
					else
					{
						iOcsCount++;
					}

					strOcsAddress[iOcsCount - 1] = strAnsiAddr.Trim();
					strTmp.sprintf("(ADDRESS_UNICAST_REPLY) 새로운 OCS주소 추가됨. Addr : %s Length : %d 할당된 OCS NO %d",
															strAnsiAddr, strAnsiAddr.Length(), iOcsCount);
					DisplayLog(strTmp);
					// bOcsConnected = true;

					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strOcsMachineID[iOcsCount - 1] = cTemp;
				}
			break;

			case ADDRESS_EXPIRE_ORDER:		// MCP의 주소파기 지시
				{
					DisplayLog(L"OCS의 주소파기 요청을 수신하여 주소를 삭제함");

					for(int i = 0; i < MAX_OCS_NUMBER; i++)
					{
						strOcsAddress[i] = "";
						strOcsMachineID[i] = "";
					}

					iOcsCount = 0;
					// bOcsConnected = false;

					// MCP의 주소할당을 요청하는 B/C를 보낸다
					DisplayLog(L"OCS에게 주소의 재 지정을 요청하는 Broadcast메시지를 보냄");
#ifdef	_ONE_PC_TEST
					Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
					Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
				}
			break;

			case ADDRESS_EXPIRE_ALL:
				{
					DisplayLog(L"OCS의 모든 VHL의 주소파기 요청을 수신하여 주소 삭제");

					for(int i = 0; i < MAX_OCS_NUMBER; i++)
					{
						strOcsAddress[i] = "";
						strOcsMachineID[i] = "";
					}
					iOcsCount = 0;
					// bOcsConnected = false;

					DisplayLog(L"이 부분에 현재 동작을 모두 정지, 삭제하는 호출이 들어가야 한다");

					// MCP의 주소할당을 요청하는 B/C를 보낸다
#ifdef	_ONE_PC_TEST
					Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
					Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
				}
			break;
		}
		return BROADCAST_LISTEN_RESULT_SUCCESS;
	}
	else
		return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
	}
	catch(...)
	{
    	DisplayLog(L"브로드케스트에서 예외 발생");
	}

	return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

#ifdef IS_OHT
int UdpCom::OcsConnected()
{
	// return bOcsConnected;
	return iOcsCount;
}
#endif

#ifdef IS_OHT
int UdpCom::BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort - 2));	// 내가 비켜 줬다고 보자 (OCS : 9000, OHT : 9002), 즉 2를 빼줘야 함
	#else
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort));
	#endif
}
#endif

#ifdef IS_TP
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	ShowMessage("TP not using broad cast call!!");
	return 0;
}
#endif

#ifdef IS_TP
int UdpCom::BroadCast_ThreadCall()
{
	ShowMessage("TP not using broad cast call!!");
	return 0;
}
#endif


#ifdef IS_MCP
int UdpCom::UniCast_ThreadCall()
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;
//	unsigned char oneChar;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);	// 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// Log Save (Big endian)
		strTmp = DateTimeToStr(Now());		
		strMsg.sprintf("[UDP Unicast RCV] Time = %s Rcv Byte = %d  Addr = %s  Port = %d ",strTmp, iReturn, strAnsiAddr, iClientPort); 
		DisplayLog(strMsg);
 #if 0
		strMsg = "";	// 사용전 삭제
		for (int i = 0; i < iReturn; i++)
		{
			oneChar = buf[i];

			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
				strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hex표시로 바꿈
			else
				strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);

			if(((i % RAW_LOG_BYTE_1_LINE_SIZE) == 0) && (i != 0))
				strMsg = strMsg + '\r' + '\n';

			strMsg = strMsg + strTmp;
		}
		DisplayLog(strMsg);	
 #endif
		
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("[OHT->OCS 제어] 수신패킷의 크기가 해석하기에 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 OHT에서 온 건지 확인도 해야한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다
		Conv_PKT_CHECK_TAG(Pkt);

		switch(Pkt.TAG.Value)	// 명령어 종류에 따른 분기
		{
			case TagID::CLW_TO_MCP_STATUS:	// CLW가 OCS로 상태보고 (ID 03)
			{
				PKT_STATUS_REPORT_ID_03	CmdPkt;		// 작업지시 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] CLW가 OCS로 상태보고 (ID 03) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);	// Endian변환

				// 비트 구조체를 char로 변환
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 상태보고 (ID 03) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// 복사하여 상위로 전달 (앞에서 Endian변환 했으므로 여기서는 바로 복사해주면 된다)
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			// break;

			case TagID::JOB_ORDER_RESPONSE:	// CLW가 OCS로 작업지시 응답  (ID 122)
			{
				PKT_JOB_ORDER_RESPONSE_ID_122	CmdPkt;		// 작업지시 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_JOB_ORDER_RESPONSE_ID_122);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] CLW가 OCS로 작업지시 응답  (ID 122) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_JOB_ORDER_RESPONSE_ID_122(CmdPkt);

				// 비트 구조체를 char로 변환
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 작업지시 응답  (ID 122) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			// break;

			case TagID::JOB_PROGRESS_REPORT:	// CLW가 OCS로 진척보고   (ID 103)
			{
				PKT_JOB_PROGRESS_REPORT_ID_103	CmdPkt;		// 작업지시 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_JOB_PROGRESS_REPORT_ID_103);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] CLW가 OCS로 진척보고 (ID 103) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_JOB_PROGRESS_REPORT_ID_103(CmdPkt);

				// 비트 구조체를 char로 변환
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 진척보고   (ID 103) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			// break;

			case TagID::CMD_PASSPERMIT_OHT2OCS:	// CLW가 OCS로 통과 허가 요구   (ID 241)
			{
				PKT_PASSPERMIT_OHT2OCS_ID_241	CmdPkt;		// 통과 허가 요구 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] OCS로 통과 허가 요구 (ID 241) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(CmdPkt);

				// 비트 구조체를 char로 변환
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 통과 허가 요구   (ID 241) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASSPERMIT(CmdPkt);
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			// break;


			case TagID::CMD_PASSPERMITOPEN_OHT2OCS:	// CLW가 OCS로 통과 허가 개방 요구   (ID 243)
			{
				PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243	CmdPkt;		// 통과 허가 개방 요구 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] OCS로 통과 허가 개방 요구 (ID 243) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243(CmdPkt);

				// 비트 구조체를 char로 변환
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 통과 허가 개방 요구   (ID 243) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case TagID::RESPONSE_PASSPERMITREQ_OHT2OCS:	// OHT -> OCS 통과 허가 상태 보고 (ID:246)
			{
				PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246	CmdPkt;		// 통과 허가 상태 보고 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] 통과 허가 상태 보고 (ID:246) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW가 OCS로 통과 허가 상태 보고 (ID:246) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Finish_PassPermitNum : %d",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, CmdPkt.BODY.Finish_PassPermitNum);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case TagID::RESPONSE_VERSIONREQ_OHT_OCS:	// OHT가 OCS에게 상세버전을 알림
			{
				PKT_VERSION_RESPONSE	CmdPkt;		// OHT의 버전 알려주기 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_VERSION_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] 상세버전을 알림 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_VERSION_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strMsg = CmdPkt.BODY.Version;
				
				strTmp.sprintf("CLW가 OCS로 Version 상세보고 (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Version : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, strMsg);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);

				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}

			case TagID::RESPONSE_SIMPLE_VERSION_OHT_OCS:		// OHT가 OCS에게 버전을 알림
			{
				AnsiString strMonitor, strAppVer, strParamVer, strMapVer;
				
				PKT_SIMPLE_VERSION_RESPONSE	CmdPkt; 	// OHT의 버전 알려주기 패킷으로 재 복사한다
				iPacketSize = sizeof(PKT_SIMPLE_VERSION_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] 버전을 알림 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_SIMPLE_VERSION_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				/*
				char cVersion[33];					// 임시 메모리
				memset(&cVersion, ASCII_SPACE, 32);
				memcpy(&cVersion, &CmdPkt.BODY.ApplicationVersion, 32);
				cVersion[32] = '\0';
				*/


				strMonitor 	= CmdPkt.BODY.AppMonitorVersion;
				strAppVer 	= CmdPkt.BODY.ApplicationVersion;
				strParamVer = CmdPkt.BODY.ParameterVersion;
				strMapVer 	= CmdPkt.BODY.MapVersion;

				strTmp.sprintf("CLW가 OCS로 Version 보고 (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Monitor : %s App : %s Param : %s  Map : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, strMonitor, strAppVer, strParamVer, strMapVer);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			// break;

			case TagID::RESPONSE_DATATRANSFER_OHT2OCS:		// 데이터 전송 지시 응답
			{
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE	CmdPkt; 	
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] 데이터 전송 지시 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:		// 데이터 전송 진척 보고
			{
				PKT_DATA_TRANSPER_ORDER_PROGRESS	CmdPkt; 	
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS 제어] 데이터 전송 진척 보고 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

			case TagID::REQ_DATATRANSFER_OHT2OCS:		// 데이터 전송 요구 (OHT -> OCS)
			{
				PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS	CmdPkt; 	
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] 데이터 전송 요구 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("데이터 전송 요구 - 요구구분 : %d  전송구분 : %d", 
								CmdPkt.BODY.Request.CMDType, CmdPkt.BODY.Request.TestMode);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}
			//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
			case TagID::RESPONSE_AUTOCHANGE_OCS_OHT:	// auto Change  (ID:252)
			{
				PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252	CmdPkt;
				iPacketSize = sizeof(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] 오토전환 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("오터전환 응답 - 요구구분 : %d  전송구분 : %d",
								CmdPkt.BODY.AutoChangeACK.PresionCode, CmdPkt.BODY.AutoChangeACK.Accept_Error);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);

				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}

		/*
			case TagID::RESPONSE_VERSIONREQ_OHT_OCS:
			{
				PKT_VERSION_RESPONSE	CmdPkt; 	
				iPacketSize = sizeof(PKT_VERSION_RESPONSE);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] 버전 상세 보고 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_VERSION_RESPONSE(CmdPkt);

				strTmp.sprintf("버전 상세 보고 : %s", CmdPkt.BODY.Version);

				// 복사하여 상위로 전달
				PKT_OHT2OCS OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요) 			
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				
				
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}
         */
			default:
			{
				strTmp.sprintf("Tag ID필드에 해석 불가능한 값이 들어왔습니다. Tag ID : %d 보낸주소 : %s 보낸 포트 : %d",
								Pkt.TAG.Value, strAnsiAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}
			// break;
		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif



#ifdef IS_OHT
int UdpCom::UniCast_ThreadCall(PKT_OCS2OHT& OcsCmdPkt)
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3, cVar4;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv;
	int iBig, iSmall;
	unsigned char oneChar;

try{

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);	// 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..
	strAnsiAddr = strClientAddr;


	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// Log Save (Big endian)		
//		strMsg.sprintf("[UDP Unicast RCV] Rcv Byte = %d  Addr = %s  Port = %d ",iReturn, strAnsiAddr, iClientPort);
//		DisplayLog(strMsg);
		
//		iBig 	= iReturn / RAW_LOG_BYTE_1_LINE_SIZE;
//		iSmall 	= iReturn % RAW_LOG_BYTE_1_LINE_SIZE;

		// 10개 단위 로그저장
//		for (int i = 0; i < iBig; i++)
//		{
//			strMsg = "";	// 한번 찍기 전 반드시 리셋
//			for(int j = 0; j < RAW_LOG_BYTE_1_LINE_SIZE; j++)
//			{
//				oneChar = buf[(i * RAW_LOG_BYTE_1_LINE_SIZE) + j];
//
//				if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
//					strTmp.sprintf("%02X(%c)  ", oneChar, oneChar);	// Hex표시로 바꿈
//				else
//					strTmp.sprintf("%02X  ", oneChar);
//
//				strMsg = strMsg + strTmp;
//			}
//			DisplayLog(strMsg);
//		}

		
		// 1개 단위 로그저장
//		strMsg = "";	// 한번 찍기 전 반드시 리셋
//		for (int i = 0; i < iSmall; i++)
//		{
//			oneChar = buf[(iBig * RAW_LOG_BYTE_1_LINE_SIZE) + iSmall];
//
//			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
//				strTmp.sprintf("%02X(%c)  ", oneChar, oneChar);	// Hex표시로 바꿈
//			else
//				strTmp.sprintf("%02X  ", oneChar);
//
//			strMsg = strMsg + strTmp;
//		}
//		DisplayLog(strMsg);
		

		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("[OCS->OHT 제어] 수신패킷의 크기가 해석하기에 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 OHT에서 온 건지 확인도 해야한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다
		Conv_PKT_CHECK_TAG(Pkt);

		// if(Pkt.HEADER.MachineType == MACHINETYPE_VHL)
		{
			switch(Pkt.TAG.Value)	// 명령어 종류에 따른 분기
			{
				case TagID::JOB_ORDER:	// 작업지시 (ID:121)
				{
					PKT_JOB_ORDER_ID_121	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);

					// 패킷 크기 검증 (ID 121 Only Pass! - Variable length)
					if(iReturn > MAX_PACKET_SIZE)
					{
						strTmp.sprintf("[OCS->OHT 제어] 작업지시 패킷의 크기가 너무 큽니다. 최대정의 size : %d 수신size : %d", MAX_PACKET_SIZE, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_JOB_ORDER_ID_121(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("작업지시 (ID:121) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				// break;
				case TagID::STATION_INFO_RESPONSE_OCS2OHT:	// Station 정보요구 응답 (ID:112)
				{
					PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112);

					// 패킷 크기 검증 (ID 121 Only Pass! - Variable length)
					if(iReturn > MAX_PACKET_SIZE)
					{
						strTmp.sprintf("[OCS->OHT 제어] 작업지시 패킷의 크기가 너무 큽니다. 최대정의 size : %d 수신size : %d", MAX_PACKET_SIZE, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("Station정보응답 (ID:112) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				case TagID::MCP_TO_CLW_STATUS:	// MCP 상태보고 (ID:01)
				{
					PKT_OCS_STATUS_ID_01	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] MCP 상태보고 (ID:01) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OCS_STATUS_ID_01(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("MCP 상태보고 (ID:01) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				// break;

				case TagID::RESPONSE_PASSPERMIT_OCS2OHT:	// 통과 허가 요구 응답 (ID:242)
				{
					PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 통과 허가 요구 응답 (ID:242) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("통과 허가 요구 응답 (ID:242) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				// break;

				case TagID::CMD_PASSPERMITREQ_OCS2OHT:	// 통과허가문의 (ID:245)
				{
					PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 통과허가문의 (ID:245) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 1");
					DisplayLog(strTmp);
					// Sleep(200);

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245(CmdPkt);

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 2");
					DisplayLog(strTmp);
					// Sleep(200);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("통과허가문의 (ID:245) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::MOVE_RESTART_OCS2OHT:	// 주행 재개  (ID:250)
				{
					PKT_MOVE_RESTART_OCS2OHT_ID_250	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_MOVE_RESTART_OCS2OHT_ID_250);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 주행 재개  (ID:250) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 1");
					DisplayLog(strTmp);
					// Sleep(200);

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_MOVE_RESTART_OCS2OHT_ID_250(CmdPkt);

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 2");
					DisplayLog(strTmp);
					// Sleep(200);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("주행재개 (ID:250) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				case TagID::RESPONSE_PASSPERMITOPEN_OCS2OHT:	// 통과 허가 개방 요구 (ID:244)
				{
					PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 통과 허가 개방 요구 (ID:244) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("통과 허가 개방 요구 (ID:244) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				// break;
                case TagID::RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT:	// 강제 통과 허가 개방 요구 (ID:248)
				{
					PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248	CmdPkt;		// 작업지시 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 강제 통과 허가 개방 요구 응답 (ID:248) - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248(CmdPkt);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("강제 통과 허가 개방 요구 (ID:248) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::CMD_VERSIONREQ_OCS_OHT:
				{
					PKT_OHT_VERSION_REQUEST	CmdPkt;
					iPacketSize = sizeof(PKT_OHT_VERSION_REQUEST);

					// 패킷 크기 검증
					/*
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 버전 요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					*/

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OHT_VERSION_REQUEST(CmdPkt);
					
					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("OCS가 OHT로 상세버전 확인 요청함 (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::CMD_DATATRANSFER_OCS2OHT:	// 데이터 전송지시 (OCS -> OHT)
				{
					PKT_DATA_TRANSPER_ORDER_OCS2OHT	CmdPkt;
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 데이터 전송지시 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_DATA_TRANSPER_ORDER_OCS2OHT(CmdPkt);
					
					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("데이터 전송지시 (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::RESPONSE_INSERTREQ_OCS_OHT:	// OHT 투입요구 응답 (OCS -> OHT)
				{
					PKT_OHT_INPUT_REQUEST_RESPONSE	CmdPkt;
					iPacketSize = sizeof(PKT_OHT_INPUT_REQUEST_RESPONSE);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] OHT 투입요구 응답 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OHT_INPUT_REQUEST_RESPONSE(CmdPkt);
					
					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("OHT 투입요구 응답 (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::CMD_FLOORMOVE_OCS_OHT:	// 층간 이동지시 (OCS -> OHT)
				{
					PKT_FLOOR_MOVE_ORDER	CmdPkt;
					iPacketSize = sizeof(PKT_FLOOR_MOVE_ORDER);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 층간 이동지시 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_FLOOR_MOVE_ORDER(CmdPkt);
					
					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("층간 이동지시 (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::CMD_SIMPLE_VERSION_REQUEST:			// 버전요청 (OCS -> OHT)
				{
					PKT_SIMPLE_VERSION_REQUEST	CmdPkt;
					iPacketSize = sizeof(PKT_SIMPLE_VERSION_REQUEST);

					// 패킷 크기 검증
					/*
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT 제어] 버전요청 - 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					*/

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_SIMPLE_VERSION_REQUEST(CmdPkt);
					
					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("버전요청 (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// 복사하여 상위로 전달
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				default:
				{
					strTmp.sprintf("Tag ID필드에 해석 불가능한 값이 들어왔습니다. Tag ID : %d 보낸주소 : %s 보낸 포트 : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;
			}
		}
	}
}
catch(...)
{
	strTmp.sprintf("Socket Exception");
	DisplayLog(strTmp);
}
	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

// OHT에서 TP의 패킷을 수신하는 함수임. (Endian 변환 불필요)
#ifdef IS_OHT
int UdpCom::OHT_TP_ThreadCall(PKT_OCS2OHT& OcsCmdPkt, char *szClientIP)
{
	AnsiString strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

	iReturn = pUdpTpServer->Listen(buf, iLen, strClientAddr, iClientPort); // 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		if(szClientIP != NULL)
		{
			wchar_t *p = strClientAddr.c_str();
			wcstombs(szClientIP, p, strClientAddr.Length());
		}

		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 OHT에서 온 건지 확인도 해야한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다

		if(Pkt.HEADER.MachineType == MACHINETYPE_TP)
		{
			switch(Pkt.TAG.Value)	// 명령어 종류에 따른 분기
			{
				case TAGCATEGORY::TAG_TP_ADDRESS_REGISTER:	// TP Address Auto Register
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Address auto Resigsted!! Address : %s ",strTpAddr);
					bTpConnected = true;

					DisplayLog(strTmp);
					return TP_ADDRESS_REGISTED;
				}
				// break;

				case TAGCATEGORY::TAG_TP_ADDRESS_DELETE:	// TP Address Delete Register
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Address Deleted!! 삭제된 TP Address : %s ",strTpAddr);

					strTpAddr = "";
					bTpConnected = false;

					DisplayLog(strTmp);
					return TP_ADDRESS_DELETED;
				}
				// break;

				case TagID::CMD_TP_COMMON:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send Common command Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;				

				case TagID::CMD_E_STOP_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_E_STOP_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_MANUALMODE_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_MANUALMODE_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_AUTOMODE_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_AUTOMODE_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_SOFTRESET_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_SOFTRESET_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;
				
				case TagID::CMD_STATUSREQ_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_STATUSREQ_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_INITREQ_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_INITREQ_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::JOB_ORDER_TP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send JOB_ORDER Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_MOVE_BY_MAP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send MOVE_BY_MAP Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}
				case TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send 통과 허가 강제 개방 요구 Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_IOMAP_REQ_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 OHT로 IO리스트를 요청 Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_IOMAP_USE_ONOFF_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 IO의 ON, OFF를 행한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_IOMAP_SET_RESET_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 IO의 1, 0 세팅을 행한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_TEACHING_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 티칭후 OHT로 티칭값을 전송한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TEACHING_INFO_REQ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 OHT의 티칭 데이터를 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TRANS_LOAD:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 재하 LOAD (장비로 내려놓음)를 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}

				case TagID::CMD_TRANS_UNLOAD:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 재하 UNLOAD (장비에서 꺼내 OHT로 이동)를 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }

					
					return iReturn;
				}

				case TagID::CMD_DRIVINGMAP_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 DIRIVINGMAP정보를 OHT로 전송한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_OACIS_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 OACIS정보를 OHT로 전송한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_DRIVINGMAP_INFO_REQ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 OHT의 DrivingMap 데이터를 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TRANS_MOVE_TEACHING_POINT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 TEACHING POINT로 이동하도록 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}
				case TagID::CMD_STATIONINFOREQ_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP에서 Station 요구 이동을 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_CID_M_COMMAND_SEND:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 CID INFO 요청한다 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_CID_M_COMMAND_RESULT_READ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 CID정보를 시리얼로 읽어달라고 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_SOUND_ONOFF:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 SOUND On/OFF 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_POWER_ONOFF:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Power On/OFF 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				// ------- AutoTeaching 지시 () ---------
				case TagID::CMD_TP_MAPMAKER_FILE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 MapMaker_File 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_FILE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 AutoTeaching_File 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 AutoTeaching 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_NULL_TYPE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 AutoTeaching_NullType 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_READ_DRIVING:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 AutoTeaching 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AMC_UPDATE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 amc update 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_OCS_PAUSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Pause by OCS 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_OCS_RESUME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Resume by OCS 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_TP_PAUSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Pause by TP 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_TP_RESUME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Resume by TP 요청  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_DEBUG_COMMAND:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Auto Teaching Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				// ------- 펌웨어 업데이트 지시 (데이터 전송지시) ---------				 
				case TagID::CMD_DATATRANSFER_TP_OHT:	// 데이터 전송 지시
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("[TP Send] TP에서 데이터 전송 지시 요청 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_VERSIONREQ_TP_OHT:		// 버전 상세 문의
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("[TP Send] TP에서 버전 상세 문의 요청 Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// --------------------------------------------------------

				case TagID::REQ_DATATRANSFER_OHT_TP:	// TP에서 데이터 전송요구 (TP -> OHT -> OCS)
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 데이터 전송요구 (TP -> OHT -> OCS)  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_MOVE_ALL_HOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Move All Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

                case TagID::CMD_TP_MOVE_HOIST_HOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Move All Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_AUTO_HOISTHOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 AUTO_HOISTHOME Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_AUTOMODE_CHANGE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Auto Mode Change Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}

				case TagID::CMD_TP_HID_LOG_SAVE_ORDER:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 HID Log Save Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
 				case TagID::CMD_TP_VIB_DATA_LINK:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_LINK Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_REQ_START:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_REQ_START Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_REQ_STOP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_REQ_STOP Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_CLOSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_CLOSE Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_LOAD_VALUE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Hoist Param Load Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_SAVE_VALUE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Hoist Param Save Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_MOVE_TO_PULSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP에서 Hoist Pulse Move Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_AUTOCHANGE_OCS_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("Reset + Auto Mode Change Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//first Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{
						strTmp.sprintf("Reset + Auto Mode Change Command Send  : Map fail!!");
						DisplayLog(strTmp);
						iReturn = 0;
					}
					
					return iReturn;
				}

				default:
				{
					strTmp.sprintf("TP/PC Packet Tag ID필드에 해석 불가능한 값이 들어왔습니다. Tag ID : %d 보낸주소 : %s 보낸 포트 : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
//					ShowMessage(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;

			}
		}
		// ---------------------- TP/PC Section end -----------------------------
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_OHT
bool UdpCom::TpConnected()
{
	return bTpConnected;
}
#endif



#ifdef IS_TP
int UdpCom::UniCast_ThreadCall()
{
	ShowMessage("TP Can't use UniCast_ThreadCall()");
	return -1;
}
#endif

#ifdef IS_TP
int UdpCom::OHT_TP_ThreadCall()
{
	AnsiString strTmp, strAnsiAddr;
	// char buf[MAX_PACKET_SIZE+1];
	char buf[MAX_BIG_PACKET_SIZE + 1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	iReturn = pUdpTpServer->Listen(buf, iLen, strClientAddr, iClientPort); // 이 정보가 다 필요한 것은 아니지만, 넘겨주니까..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// 가져온 정보가 있는지 확인하자
	{
		// (1) 헤더 크기와 매칭되는지 확인한다.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

		if(iReturn < iPacketSize)	// 받은 패킷의 크기가 헤더도 들어가지 않을만큼 작은경우 (처리하면 안됨)
		{
			strTmp.sprintf("컨트롤 수신패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d 보낸주소 : %s 보낸 포트 : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) 패킷의 종류를 판별한다. 그리고 OHT에서 온 건지 확인도 해야한다.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다

        // VHL 기종 세분화에 따른 Logic 변경
//		if(Pkt.HEADER.MachineType == MACHINETYPE_VHL)		// OHT -> TP
		if((Pkt.HEADER.MachineType >= MACHINETYPE_VHL_COMMON) && (Pkt.HEADER.MachineType <= MACHINETYPE_VHL_PATROL))		// OHT -> TP
		{
			switch(Pkt.TAG.Value)	// 명령어 종류에 따른 분기
			{
				case TagID::CMD_TP_COMMON:
				{
					PKT_OHT2OCS	CmdPkt;		// normal 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_OHT2OCS);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW가 TP로 Command    [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					// MainForm->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					return iReturn;		// 수신한 패킷의 크기를 리턴한다

					}
					// break;

				case TagID::CMD_STATUSREQ_TP_RESPONSE:
					{
//						PKT_OHT2TP_RESPONSE CmdPkt; 	// normal 패킷으로 재 복사한다
						iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);

						/*
						iPacketSize = sizeof(MSG_STATUS_OHT2TP);
						iPacketSize = sizeof(uTP_CMD);

						iPacketSize = sizeof(CMD_TP_JOG_DRV);
						iPacketSize = sizeof(CMD_TP_JOG_DIST);
						iPacketSize = sizeof(CMD_TP_JOG_TR);
						iPacketSize = sizeof(OHT_INFO);
						iPacketSize = sizeof(TEACHING_INFO);
						iPacketSize = sizeof(TEACHING_SAVE);
						iPacketSize = sizeof(MAPMAKE_INFO);   */

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

#if 0
						// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
						memcpy(&CmdPkt, &buf, iPacketSize);
	
						// 비트 구조체를 char로 변환
						memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
						memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
						memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
						strTmp.sprintf("CLW가 TP로 Command	  [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
										CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
										Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
						DisplayLog(strTmp);
#endif

						// 복사하여 상위로 전달
						PKT_OHT2TP_RESPONSE OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
						
						SocketForm->UdpSocket_Status_Read(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다
					
				}
				

				case TagID::CMD_INITREQ_TP_RESPONSE:
					{
//						PKT_OHT2TP_RESPONSE CmdPkt; 	// normal 패킷으로 재 복사한다
						iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);
	
						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						// 복사하여 상위로 전달
						PKT_OHT2TP_RESPONSE OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
						
						SocketForm->UdpSocket_Status_Read(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다
					
				}

				case TagID::JOB_ORDER_RESPONSE_TP:	// 작업지시 응답
				{
					PKT_OHT2OCS	CmdPkt;		// normal 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_OHT2OCS);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW가 TP로 작업지시 응답  [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

					// 이 라인에서 직접 적절한 위치의 코드를 호출해준다
					// SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocket_Standard_Read(OhtCmdPkt);

					return iReturn;		// 수신한 패킷의 크기를 리턴한다

					}

				case TagID::RESPONSE_PASSPERMITFORCEOPEN_TP2OHT:	// 통과 허가 강제 개방 응답
				{
					PKT_OHT2OCS	CmdPkt;		// normal 패킷으로 재 복사한다
					iPacketSize = sizeof(PKT_OHT2OCS);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// 문제 없는 패킷으로 확인되면 로그에 중요 내용을 표시
					memcpy(&CmdPkt, &buf, iPacketSize);

					// 비트 구조체를 char로 변환
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW가 TP로 통과 허가 강제 개방 응답 [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					PKT_OHT2OCS	OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

					// 이 라인에서 직접 적절한 위치의 코드를 호출해준다
					// SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocket_Standard_Read(OhtCmdPkt);

					return iReturn;		// 수신한 패킷의 크기를 리턴한다

					}

				case TagID::CMD_IOMAP_RESPONSE_OHT2TP:
					{
						iPacketSize = sizeof(PKT_OHT2TP_IOLIST);
	
						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT에서 TP로 IO리스트를 전송");

						// 복사하여 상위로 전달
						PKT_OHT2TP_IOLIST OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
						
						SocketForm->UdpSocket_IO_LIST_Read(OhtCmdPkt);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다					
				}

				case TagID::CMD_CID_M_COMMAND_RESULT_RETURN:
				{
						iPacketSize = sizeof(PKT_OHT2TP_CID_INFO);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT에서 TP로 CID INFO를 전송");

						// 복사하여 상위로 전달
						PKT_OHT2TP_CID_INFO OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_CID_INFO(OhtCmdPkt);
						return iReturn;
				}
				break;

				case TagID::CMD_TP_MOVE_RESULT:
				{
						iPacketSize = sizeof(PKT_OHT2TP_MOVE_BY_MAP_RESULT);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT에서 TP로 Map Move Result를 전송");

						// 복사하여 상위로 전달
						PKT_OHT2TP_MOVE_BY_MAP_RESULT OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_MapMove_Result(OhtCmdPkt);
						return iReturn;
				}
				break;

				case TagID::CMD_MAP_MAKE_RESULT:
				{
						iPacketSize = sizeof(PKT_OHT2TP_LOG_DATA);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT에서 TP로 Map Make Result를 전송");

						// 복사하여 상위로 전달
						PKT_OHT2TP_LOG_DATA OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_MapMake_Result(OhtCmdPkt);
						return iReturn;
				}
				break;


				case TagID::CMD_TP_DEBUG_RESULT:
				{
						iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT에서 TP로 Map Move Result를 전송");

						// 복사하여 상위로 전달
						PKT_TP2OHT_SIMPLE_COMMAND OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_AutoTeach_Result(OhtCmdPkt);
						return iReturn;
				}
				break;


				case TagID::CMD_TEACHING_INFO_LOAD:
					{
						iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	
						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT에 저장되어 있는 티칭값을 TP로 전송");


						// 복사하여 상위로 전달
						PKT_OHT2TP_TEACHING_INFO OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

						SocketForm->DisplayTeachingData(OhtCmdPkt);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다					
				}

				case TagID::CMD_DRIVINGMAP_INFO_LOAD:
				{
						iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT에 저장되어 있는 Driving Map을 TP로 전송");


						// 복사하여 상위로 전달
						PKT_OHT2TP_DRIVINGMAP_INFO OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

						SocketForm->DisplayDrivingMapData(OhtCmdPkt);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}

				case TagID::CMD_TP_AUTOTEACHING_LOAD_DRIVING:
					{
						iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);

						// 패킷 크기 검증
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT에 저장되어 있는 티칭값을 TP로 전송");


						// 복사하여 상위로 전달
						PKT_OHT2TP_TEACHING_INFO OhtCmdPkt;					// CLW 상태보고 패킷으로 재 복사한다
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)

						SocketForm->DisplayAutoTeachingData(OhtCmdPkt);
						return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}


				// ------- 펌웨어 업데이트 지시 (데이터 전송지시) ---------	
				case TagID::RESPONSE_DATATRANSFER_OHT_TP:			// 데이터 전송 지시 응답
				{
					iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
			
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;

					}
			
					DisplayLog("TP로 데이터 전송 지시 응답 전송");

					// 복사하여 상위로 전달
					PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE OhtCmdPkt; 				// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					
					SocketForm->UdpSocket_DataTransper_Response_Read(OhtCmdPkt);
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다					
				}

				case TagID::REPORT_DATATRANSFERPROGRESS_OHT_TP:	// 데이터 전송 진척 보고
				{
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);
			
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
			
					DisplayLog("TP로 데이터 전송 진척 보고 전송");
			
					// 복사하여 상위로 전달
					PKT_DATA_TRANSPER_ORDER_PROGRESS OhtCmdPkt; 				// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					
					SocketForm->UdpSocket_Transper_Progress_Read(OhtCmdPkt);
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다					
				}

				case TagID::RESPONSE_VERSIONREQ_OHT_TP:			// 버전 상세 보고 
				{
					iPacketSize = sizeof(PKT_VERSION_RESPONSE);
			
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
			
					DisplayLog("TP로 버전 상세 보고 전송");
			
					// 복사하여 상위로 전달
					PKT_VERSION_RESPONSE OhtCmdPkt; 				// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					
					SocketForm->UdpSocket_VersionReq_Response_Read(OhtCmdPkt);
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다					
				}

				case TagID::CMD_TP_HID_LOG_SAVE_RESULT:
				{
					iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					DisplayLog("TP로 HID log save result 전송");

					PKT_TP2OHT_SIMPLE_COMMAND OhtCmdPkt; 				// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_HID_LogSave_Result_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_LOAD_RESULT:
				{
					iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					DisplayLog("TP로 Hoist param load result 전송");

					PKT_TP2OHT_HOIST_TEACH_COMMAND OhtCmdPkt; 				// CLW 상태보고 패킷으로 재 복사한다
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_Hoist_Param_Result_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_VIBDATA_REQ_TP_RESPONSE:
				{
					iPacketSize = sizeof(PKT_OHT2TP_VIBRATION_INFO);
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					DisplayLog("TP로 AVS 진동 data 전송");
					PKT_OHT2TP_VIBRATION_INFO OhtCmdPkt;
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_AVS_Vibration_Data_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_LINK:	// link success
				case TagID::CMD_TP_VIB_DATA_CLOSE:	// link fail or link disconnection
				{
					iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					DisplayLog("AVS 진동 DATA RESPONESE");
					SocketForm->UdpSocket_AVS_Response(Pkt.TAG.Value);
					return iReturn;
				}

				default:
				{
					strTmp.sprintf("Tag ID필드에 해석 불가능한 값이 들어왔습니다. Tag ID : %d 보낸주소 : %s 보낸 포트 : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;
			}
		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_TP
bool UdpCom::OhtConnected()
{
	return bOhtConnected;
}
#endif

//---------------------------------------------------------------------------
void UdpCom::MyInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort)
{
	iMyMachineType = iMachineType;
	strMyMachineId = strMachineId;

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = iBroadCastPort + 2;
		iMyUnicastPort = iUnicastPort + 2;
	#else
		iMyBroadCastPort = iBroadCastPort;
		iMyUnicastPort = iUnicastPort;
	#endif
#else  	// OCS 에뮬레이터인 경우, 자신의 포트번호는 지정한 그대로 사용한다
	iMyBroadCastPort = iBroadCastPort;
	iMyUnicastPort = iUnicastPort;
#endif
}
//---------------------------------------------------------------------------

void UdpCom::SetBroadCastMask(AnsiString strMask)
{
	pUdpBroadCastSender->SetMask(strMask);
}

// 20161216 sh17.jo : Commfail 체크 기능 추가
void UdpCom::GetOcsAddress(char *OcsIP, int Ocs_Cnt)
{
    // OCS 주소를 인수에 복사
    strcpy(OcsIP, strOcsAddress[Ocs_Cnt-1].c_str());
}
//---------------------------------------------------------------------------
