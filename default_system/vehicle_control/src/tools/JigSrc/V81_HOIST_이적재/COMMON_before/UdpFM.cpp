//---------------------------------------------------------------------------

/*
	이 모듈을 사용하기 전에 프로젝트 설정에 세팅해야 하는 값
	(프로젝트 설정 -> C++ Compiler -> Directories and Conditionals의 Conditional defines 에 설정)

	[어시스턴스인 경우]
	IS_AS			   : 어시스턴스임을 나타낸다. 설정해야 함
	_IN_UDP_UPPER_FORM : UDP통신내용을 화면에 표시하는 모듈(frmUDP) 사용시 설정. 여기서는 설정함

	[파일 매니저인 경우]
	IS_FM 			   : 파일매니저 임을 나타낸다. 설정해야 함
	_IN_UDP_UPPER_FORM : UDP통신내용을 화면에 표시하는 모듈(frmUDP) 사용시 설정. 여기서는 설정함


*/

#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpFM.h"
#include "Protocol_Define_Modify.h"

// #include "SocketInitClose.h"
// #include "UdpServer.h"
// #include "UdpBroadCastSender.h"
// #include "UdpUniCastSender.h"

#include "PUtily.h"

#ifdef _IN_UDP_UPPER_FORM
	#include "Udp.h"
#endif
//---------------------------------------------------------------------------

SocketInitClose		*pSocketInitClose;		// 소켓의 초기화 기능

// [AS - FM]
UdpServer 			*pUdpUniCastServer;			// AS - FM 통신 전용

// [COMMON]
UdpUniCastSender	*pUdpUniCastSender; 		// 일대일 통신 전송처리

//---------------------------------------------------------------------------

UdpFM::UdpFM()
{
	pSocketInitClose = NULL;
	pUdpUniCastServer = NULL;
	pUdpUniCastSender = NULL;

	if (pSocketInitClose == NULL)
		pSocketInitClose = new SocketInitClose();

	if (pUdpUniCastSender == NULL)
		pUdpUniCastSender = new UdpUniCastSender();

	// 소켓 초기화 일괄적으로 해줌
	pSocketInitClose->SocketInit();

	// BIND 하여 포트번호를 할당받는다
	AS_FILE_MANAGER_BIND();

}

//---------------------------------------------------------------------------
void UdpFM::DisplayLog(String strLog)
{
#ifdef _IN_UDP_UPPER_FORM
	frmUDP->DisplayLog(strLog); 	// 상시 디버그를 위해 UDP폼 안에 있는 경우, 화면에 표시
#endif
}
//---------------------------------------------------------------------------

UdpFM::~UdpFM()
{
	if (pSocketInitClose != NULL)
		delete(pSocketInitClose);

	if (pUdpUniCastSender != NULL)
		delete(pUdpUniCastSender);

	pSocketInitClose->SocketClose();

	AS_FILE_MANAGER_CLOSE();
}
//---------------------------------------------------------------------------
// OHT와 어시스턴스의 수신서버 바인드 동작
// 함수명은 동일하고, 내부 포트번호만 다르다

int UdpFM::AS_FILE_MANAGER_BIND()
{
	int iRetVal = -1;

	if (pUdpUniCastServer == NULL)
		pUdpUniCastServer = new UdpServer();

#ifdef IS_AS
	iRetVal = pUdpUniCastServer->Bind(FM_2_AS_PORT);
#endif

#ifdef IS_FM
	iRetVal = pUdpUniCastServer->Bind(AS_2_FM_PORT);
#endif

	if(iRetVal == 0)
	{
		DisplayLog(L"[어시스턴스 - 파일매니저 포트] Bind 동작 성공");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[어시스턴스 - 파일매니저 포트] 이미 Bind 되어 있습니다");
	}
	else
	{
		DisplayLog(L"[어시스턴스 - 파일매니저 포트] Bind 동작 실패");
	}

	return iRetVal;
}

void UdpFM::AS_FILE_MANAGER_CLOSE()
{
	if (pUdpUniCastServer != NULL)
	{
		pUdpUniCastServer->Close();
		delete(pUdpUniCastServer);
		pUdpUniCastServer = NULL;
	}
}

int UdpFM::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("정의된 최대 패킷 사이즈보다 큰 메시지 송신시도!!");
		return UNICAST_SEND_RESULT_FAIL;
	}

	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
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


#ifdef IS_AS
int UdpFM::UnicastSend_AS_2_FM(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , AS_2_FM_PORT, buf, len);
}

int UdpFM::UnicastSend_AS_2_OHT(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , OHT_AS__OHT_PORT_NUM, buf, len);
}

#endif


// 파일매니저 => 어시스턴스로 통신 (OHT -> OCS 표준 패킷 사용)
#ifdef IS_AS
int UdpFM::FM_2_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
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

				strTmp.sprintf("데이터 전송 진척 보고 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  전송실패 : %d  전송성공 : %d  데이터 전송상태구분 : %d  전송지시 ID : %d  전송실패구분 : %d",
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

				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("데이터 전송 진척 보고 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				DisplayLog(strTmp);

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				return iReturn;		// 수신한 패킷의 크기를 리턴한다
			}
			break;

			case TagID::CMD_INERNAL_PING_RESPONSE:			// 내부 프로그램들끼리 핑을 보내고 받아서 연결여부를 확인한다
			{
				PKT_CHECK_TAG CmdPkt;
				iPacketSize = sizeof(PKT_CHECK_TAG);
			
				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("PING 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				//strTmp.sprintf("PING 패킷 (FM => AS) Tag : %d	전송구분 : %d  요구구분 : %d",
				//				   CmdPkt.TAG.Value, CmdPkt.  CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				DisplayLog("파일매니저가 살아 있습니다");

				// 복사하여 상위로 전달
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
				return iReturn; 	// 수신한 패킷의 크기를 리턴한다
			}
			break;

		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

int UdpFM::AS_ThreadCall(char* AS_Pkt, int* piMachineType)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
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
		
		if(Pkt.HEADER.MachineType == MACHINETYPE_VHL_COMMON)
		{
			*piMachineType = MACHINETYPE_VHL_COMMON;
			switch(Pkt.TAG.Value)
			{
				case TagID::CMD_DATATRANSFER_OCS2OHT:			// 데이터 전송 지시
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

					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("데이터 전송 지시 (OHT => AS) Tag : %d  전송구분 : %d  요구구분 : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				break;


				case TagID::CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS:
				{
					PKT_RESTART_REQUEST_OHT2AS CmdPkt;
					iPacketSize = sizeof(PKT_RESTART_REQUEST_OHT2AS);

					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}				

					// --- Log ---
					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("Packet Received. OHT가 자신에게 종료명령을 주고, 재시작 시켜 달라고 AS로 요청하는 패킷1");
					DisplayLog(strTmp);

					// return ready
					memcpy(AS_Pkt, &buf, iPacketSize);
					return iReturn;
				}
				break;

				case TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS:
				{
					PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS CmdPkt;
					iPacketSize = sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS);
				
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}				
				
					// --- Log ---
					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("Packet Received. OHT가 자신에게 종료명령을 주고, 재시작 시켜 달라고 AS로 요청하는 패킷2");
					DisplayLog(strTmp);
				
					// return ready
					memcpy(AS_Pkt, &buf, iPacketSize);
					return iReturn;
				}
				break;

				
			}
		}
		else if(Pkt.HEADER.MachineType == MACHINETYPE_FM)
		{
			*piMachineType = MACHINETYPE_FM;
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
					strTmp.sprintf("데이터 전송 지시 응답 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  접수불가 : %d  접수불가구분 : %d  전송지시 ID : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
													CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
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

					strTmp.sprintf("데이터 전송 진척 보고 (AS => OHT) Tag : %d  전송구분 : %d  요구구분 : %d  전송실패 : %d  전송성공 : %d  데이터 전송상태구분 : %d  전송지시 ID : %d  전송실패구분 : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
													CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
													CmdPkt.BODY.FailClass);
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				break;

				case TagID::CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT:	// 무선모듈 업데이트 또는 데이터읽기 결과
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

					memcpy(&CmdPkt, &buf, iPacketSize);

					strTmp.sprintf("무선모듈 업데이트 또는 데이터읽기 결과 (AS => OHT) Tag : %d  Result Code : %d  Result Text : %s",
													CmdPkt.TAG.Value, CmdPkt.BODY.iResult, CmdPkt.BODY.ResultText);
					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
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

					strTmp.sprintf("CLW Status 보고 (AS => OHT) Tag : %d  데이터 전송지시 ID : %d  데이터 전송 구분 : %d  데이터 전송상태 구분 : %d  Data 전송 성공 : %d  Data 전송 실패 : %d",
									CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
									CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

					DisplayLog(strTmp);

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn;		// 수신한 패킷의 크기를 리턴한다
				}
				break;
				
				case TagID::CMD_INERNAL_PING_RESPONSE:			// 내부 프로그램들끼리 핑을 보내고 받아서 연결여부를 확인한다
				{
					PKT_CHECK_TAG CmdPkt;
					iPacketSize = sizeof(PKT_CHECK_TAG);
				
					// 패킷 크기 검증
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("PING 패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);
					//strTmp.sprintf("PING 패킷 (FM => AS) Tag : %d	전송구분 : %d  요구구분 : %d",
					//				   CmdPkt.TAG.Value, CmdPkt.  CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
					DisplayLog("파일매니저가 살아 있습니다");

					// 복사하여 상위로 전달
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHT표준 패킷으로 내려준다. 패킷사이즈는 원래 패킷의 사이즈를 사용한다 (중요)
					return iReturn; 	// 수신한 패킷의 크기를 리턴한다
				}
				break;
			}
		}
		else
		{
			return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}
		
		
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_FM
int UdpFM::UnicastSend_FM_2_AS(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , FM_2_AS_PORT, buf, len); 
}
#endif


#ifdef IS_FM
// 어시스턴스 => 파일매니저 (OCS -> OHT 표준 패킷 사용)
int UdpFM::AS_2_FM_ThreadCall(PKT_OCS2OHT& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	AnsiString strConv, strChk;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	//strTmp.sprintf("iReturn : %d", iReturn);
	//DisplayLog(strTmp);

	if (iReturn > 0)
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

		//strTmp.sprintf("Pkt.TAG.Value : %d", Pkt.TAG.Value);
		//DisplayLog(strTmp);

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

			case TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS:		// FTP
			{
				PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS CmdPkt;
				iPacketSize = sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS);

				// 패킷 크기 검증
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("패킷의 크기가 너무 작습니다. 최소 size : %d 수신size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("Packet Received. Wireless FTP");
				DisplayLog(strTmp);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);
				return iReturn;
			}
			break;

			case TagID::CMD_INERNAL_PING_REQ:			// 내부 프로그램들끼리 핑을 보내고 받아서 연결여부를 확인한다
			{
				DisplayLog("어시스턴스가 확인 패킷을 보냈습니다");

				PKT_CHECK_TAG Response_Pkt;

				// auto response
				iPacketSize = sizeof(PKT_CHECK_TAG);
				memset(&Response_Pkt, 0, iPacketSize);

				Response_Pkt.HEADER.MachineType = MACHINETYPE_FM;
				Response_Pkt.TAG.Value = TagID::CMD_INERNAL_PING_RESPONSE;

				UnicastSend_FM_2_AS((char*)&Response_Pkt, iPacketSize);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);

				return iReturn;
			}

		}

	}
	return iReturn;
}

#endif



