// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
// #include <stdio.h>
#include <dos.h>
#include "SocketUnit.h"
#include "MainUnit.h"
#include "OHTInfoUnit.h"
#include "StatusUnit.h"
#include "CommDisplayUnit.h"
#include "IOMapUnit.h"
#include "Utility.h"
#include "AssistantUnit.h"
#include "ConnectUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "CMDControlUnit.h"
#include "TeachingUnit.h"
#include "HoistDistanceConverter.h"
#include "Define.h"
#include "LongrunUnit.h"

// by zzang9un 2012.11.24 : Header 파일 추가
#include "Def_TPCommand.h"
#include "Def_Protocol.h"
#include "OHT_TLV.h"
#include "Def_TP.h"
#include "SplashUnit.h"
#include "MapMakerUnit.h"
#include "OHTSysInfoUnit.h"

// shkim add
#include "UDP.h"
#include "FirmwareUpdateUnit.h"
#include "DataTransperReq.h"
#include "HidLogGet.h"

#include "VibrationMeasureUnit.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSocketForm *SocketForm;

// ---------------------------------------------------------------------------
__fastcall TSocketForm::TSocketForm(TComponent *Owner) : TForm(Owner)
{
    SocketInit();
    bATReceive = false;

    bConnectedOHT = false;

    fIsIOMapLoaded = false;
}
// ---------------------------------------------------------------------------

/**
@brief   Soket 포트 및 초기화를 위한 함수
@author  zzang9un
@date    2013.04.17
*/
void TSocketForm::SocketInit()
{
    OHTSocket->Active = false;
    OHTSocket->Port   = PORT_OHT_TP;

    AssistantSocket->Active = false;
    AssistantSocket->Port   = PORT_TP_ASSISTANT;

    AutoTeachingSocket->Active = false;
    AutoTeachingSocket->Port   = PORT_TP_AUTOTECHING;

    m_IOMapField = new IO_MAP_FIELD;
    memset(m_IOMapField, 0, sizeof(IO_MAP_FIELD));
}

// ---------------------------------------------------------------------------
// OHTSocket->Host    : Host Name 사용시
// OHTSocket->Active  : Server Socket과 연결되었을 경우..사용중지(=false), 재사용(=true)
// OHTSocket->Address : IP Address사용
// OHTSocket->Open()  : Server와 연결
// ---------------------------------------------------------------------------

/**
@brief   Soket Connect 함수(OHT)
@author  puting
@date    2012.12.14
@param   Soket Connect(IP 및 OHT Ver설정)
*/
/*
void TSocketForm::ConnectOHT(String strIPAddr, String strOHTName)
{
    int nPos;

    if(OHTSocket->Active == true)
    {
        OHTSocket->Active = false;
        MainForm->DataDisplay(COLOR_BLUE, m_strOHTName + " DisConnect...:" + OHTSocket->Address);
    }

    OHTSocket->Close();
    OHTSocket->Address = strIPAddr;
    OHTSocket->Open();

    m_strOHTName      = strOHTName;
    m_strOHTIPAddress = strIPAddr;

    nPos = m_strOHTName.Pos("#");

    if(nPos == 0)
        m_strOHTNum = "?";
    else
        m_strOHTNum = m_strOHTName.SubString(nPos + 1, m_strOHTName.Length() - nPos + 1);
}
*/
// ---------------------------------------------------------------------------
void TSocketForm::ConnectOHT(String strIPAddr, int iOhtPortNum, int iTpPortNum, String strOHTName)
{
    int nPos;

	if(OHTSocket->Active == true)
	{
		OHTSocket->Active = false;
		MainForm->DataDisplay(COLOR_BLUE, m_strOHTName + " DisConnect...:" + OHTSocket->Address);
		OHTSocket->Close();
	}

	OHTSocket->Address = strIPAddr;		// temp using
	// shkim. 2017.01.24 - 새 코드. 숫자가 들어간 부분을 찾아서 디스플레이 함
	m_strOHTName = strOHTName;
	m_strOHTNum = GetUnit(strOHTName);
	m_strOHTIPAddress = strIPAddr;

	// shkim add 1 (Conncet) --------------------------------------------
	int iResult = frmUDP->pUdpObject->OHT_TP_Bind(strIPAddr, iOhtPortNum);	// 새로운 통신모듈을 동작시킨다.

	if(iResult == -3)
		ShowMessage("TP용 UDP 수신포트가 이미 열려 있습니다!");
	else if(iResult == 0)
	{
		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize = sizeof(PKT_OCS2OHT);
		int iReturn, iTemp;

		PKT_OCS2OHT	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
		SEND_PACKET.TAG.Value = TAGCATEGORY::TAG_TP_ADDRESS_REGISTER;

		iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);  // TP주소 등록요청
		//ShowMessage("TP용 UDP 수신포트 bind 성공");

		// shkim add 1 (OHTSocketConnect event) --------------------------------------------
		OHTSocketConnecting(NULL, NULL);
		OHTSocketConnect(NULL, NULL);

		MainForm->tmrUdpListen->Enabled = true;		// UDP 통신포트 수신시작
	}
	else
		ShowMessage("TP용 UDP 수신포트 bind 실패! 포트를 확인해 주세요");

	// MainForm->tmrUdpListen->Enabled = true;		// UDP 통신포트 수신시작

	// --------------------------------------------

	// shkim. 2017.01.24 - 이전 코드. 제대로 동작 안됨.
#if 0
	m_strOHTName      = strOHTName;
	m_strOHTIPAddress = strIPAddr;

	nPos = m_strOHTName.Pos("#");

	if(nPos == 0)
		m_strOHTNum = "?";
    else
        m_strOHTNum = m_strOHTName.SubString(nPos + 1, m_strOHTName.Length() - nPos + 1);	
#endif


}


UnicodeString TSocketForm::GetUnit(UnicodeString str)
{
	int iBegin = -1;
	int iNum = 0;
	UnicodeString tmpstr;
	int tmpint;

	// 숫자 시작점 찾기
	for (int i = 0; i < str.Length(); i++)
	{
		tmpstr = str.SubString((i + 1), 1);	// 한글자를 땐다

		if(TryStrToInt(tmpstr,tmpint))
		{
			iBegin = i;	// 숫자 시작위치
			break;
		}
	}

	if(iBegin == -1)	// 숫자가 없음
		return "?";

	// 숫자가 끝날때까지 읽는다
	for (int i = iBegin; i < str.Length(); i++)
	{
		tmpstr = str.SubString((i + 1), 1);

		if(TryStrToInt(tmpstr,tmpint))	// 숫자면
		{
			iNum = iNum * 10 + tmpint;
		}
		else	// 다른 문자를 만나게 되면
		{
			break;
        }
	}

	tmpstr = iNum;

	return tmpstr;
}
// ---------------------------------------------------------------------------

/**
@brief   Soket DisConnect 함수(OHT)
@author  puting
@date    2012.12.14
@param   Soket DisConnect
*/
void TSocketForm::DisConnect()
{
    gm_OHTStatus->bReceiveStatus = true;

    // 관련된 Form의 timer를 Disable한다.
    MainForm->timerAllDisable();
    LongRunForm->timerAllDisable();

    OHTSocket->Active = false;
    OHTSocket->Close();

    AutoTeachingSocket->Active = false;
	AutoTeachingSocket->Close();

	if(AssistantSocket->Socket->Connected == true)
	{
//		AssistantSocket->Active = false;
		AssistantSocket->Close();
    }

	// -------- shkim add ----------
	MainForm->tmrUdpListen->Enabled = false;
	frmUDP->pUdpObject->BroadcastClose();
	frmUDP->pUdpObject->UnicastClose();

	// event emulation
	OHTSocketDisconnect(NULL, NULL);
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Connect 요청시 발생하는 이벤트 함수(OHT)
@author  puting
@date    2012.12.14
@param   Soket Connect Event
*/
void __fastcall TSocketForm::OHTSocketConnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
	bConnectedOHT = true;
        
	String strTitle;

	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
	String strLog;
    strLog.sprintf(L"====> %s Connected..[%s]", m_strOHTName, OHTSocket->Address);
    m_LogFile->WriteLogFile("Communication", strLog);
    delete m_LogFile;

    ConnectForm->Close();

    MainForm->panCommStatus->Color       = COLOR_YELLOW;
    MainForm->panCommStatus->Font->Color = COLOR_BLACK;
    MainForm->panCommStatus->Caption     = "["+m_strOHTName+"]" + " Connect...:" + OHTSocket->Address;
    // MainForm->StatusBar1->Panels->Items[0]->Text = m_strOHTName+" ConnectOHT...:"+OHTSocket->Address;

    MainForm->DataDisplay(COLOR_BLUE, m_strOHTName + " Connect...:" + OHTSocket->Address);

	StatusForm->Show();

    MainForm->panQuickCmd->Visible = true;

//    MainForm->panDataStatus->Visible = true;

    MainForm->ShowMainMenu();
    MainForm->mniConnect->Enabled         = false;
    MainForm->mniDisconnect->Enabled      = true;

	if(m_strOHTNum.Length() > 2)
        MainForm->panVer->Font->Size = 32;
    else
        MainForm->panVer->Font->Size = 48;

   // MainForm->panVer->Caption = m_strOHTNum;    기존
	strTitle.sprintf(L"[%s]%s", m_strOHTNum, MainForm->g_strTitle);
    Application->Title         = strTitle;
    MainForm->ppanVer->Visible = true;

	MainForm->CommOK();

/*
	MainForm->timerAllEnable();  */
	// ShowMessage(L"타이머 임시로 꺼놓음! 테스트중이라...");

    gm_OHTStatus->bReceiveStatus = true;

    // by zzang9un 2012.11.24 : 최초 접속 시 OHT에게 Info 요청
    /*
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_OHTINIT);
    delete m_CMDControl;
    */

	// OHT에게 OHT의 구성정보를 받는다
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_ESTOP);	
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_INIT_INFO_REQ);	
	delete m_CMDControl;


    MainForm->timerAllEnable();

	// OHT로 상태요청 값을 보내보자 sh8888.kim 
	// TCMDCONTROL* m_CMDControl = new TCMDCONTROL;	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_ESTOP);	
	// m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_STATUS_REQ);	
	// delete m_CMDControl;

}

// ---------------------------------------------------------------------------

/**
@brief   Soket Connecting 요청시 발생하는 이벤트 함수(OHT)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::OHTSocketConnecting(TObject *Sender,
    TCustomWinSocket *Socket)
{

	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
	String strLog;
	strLog.sprintf(L"====> Connecting..[%s]", OHTSocket->Address);
    MainForm->DataDisplay(COLOR_BLUE, strLog);
    m_LogFile->WriteLogFile("Communication", strLog);
    delete m_LogFile;

}
// ---------------------------------------------------------------------------

/**
@brief   Soket DisConnect 요청시 발생하는 이벤트 함수(OHT)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::OHTSocketDisconnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
	bConnectedOHT = true;
    
	Application->Title = MainForm->g_strTitle;

	MainForm->panCommStatus->Color       = COLOR_BLACK;
    MainForm->panCommStatus->Font->Color = COLOR_WHITE;
	MainForm->panCommStatus->Caption     = "OffLine";

    // MainForm->StatusBar1->Panels->Items[0]->Text = "OffLine";

	// 아래 라인이 있으면 void TSocketForm::DisConnect() 와 본 함수를 무한반목함
	// MainForm->CommDisconnect();
    MainForm->panQuickCmd->Visible = false;
//    MainForm->panDataStatus->Visible = false;
    StatusForm->Close();
    MainForm->CloseAllForm();
    IOMapForm->Close();
    MainForm->HideMainMenu();
    MainForm->mniConnect->Enabled         = true;
    MainForm->mniDisconnect->Enabled      = false;
    gm_OHTStatus->bReceiveStatus          = true;
    MainForm->ppanVer->Visible            = false;
	MainForm->timerAllDisable();
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
	{
		FoupOnlineCMDForm->timerAllDisable();
	}
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
		EUVOnlineCMDForm->timerAllDisable();
	}

    //2013.08.26 doori.shin 플래그 초기화
    fIsIOMapLoaded = false;

    //AssistantSocket->Close();
    OHTSocket->Close();

    TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
    String strLog;
    strLog.sprintf(L"====> %s Disconnect..[%s]", m_strOHTName, OHTSocket->Address);
    MainForm->DataDisplay(COLOR_BLUE, strLog);
    m_LogFile->WriteLogFile("Communication", strLog);
    delete m_LogFile;
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Error 요청시 발생하는 이벤트 함수(OHT)
@author  puting
@date    2012.12.14
@param   Soket Error Event
*/
void __fastcall TSocketForm::OHTSocketError(TObject *Sender,
    TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    // if(ErrorEvent == eeConnect)
    // {
    // if(OHTSocket->Active == false)
    // OHTSocket->Active = true;
    // OHTSocket->Close();

    // MainForm->StatusPanel->Color=COLOR_RED;
    // MainForm->StatusPanel->Font->Color=COLOR_BLACK;
    // MainForm->StatusPanel->Caption = m_strOHTName+" Connection Fail...";

    // MainForm->StatusBar1->Panels->Items[0]->Text = m_strOHTName+" Connection Fail...";
    // MainForm->DataDisplay(COLOR_RED, "ERROR : "+m_strOHTName+" Connection Error...");
    // }
    TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
    String strLog;
    strLog.sprintf(L"====> %s Socket Error[E%d]..[%s]", m_strOHTName, ErrorCode, OHTSocket->Address);
    MainForm->DataDisplay(COLOR_BLUE, strLog);
    m_LogFile->WriteLogFile("Communication", strLog);
    delete m_LogFile;

    ErrorCode = 0;
}
// ---------------------------------------------------------------------------

void __fastcall TSocketForm::FormDestroy(TObject *Sender)
{
    OHTSocket->Close();
}

// ---------------------------------------------------------------------------
String TSocketForm::GetLocalIP()
{
    String IPStr;
    WSAData wsaData;
    char ac[80];
    in_addr addr;

    if(!WSAStartup(MAKEWORD(1, 1), &wsaData))
    {
        if(gethostname(ac, sizeof(ac)) != SOCKET_ERROR)
        {
            hostent *phe = gethostbyname(ac);
            if(phe)
            {
                for(int i = 0; phe->h_addr_list[i] != 0; ++i)
                {
                    CopyMemory(&addr, phe->h_addr_list[i], sizeof(in_addr));
                    if(i > 0)
                        IPStr = IPStr + ".";
                    IPStr = IPStr + inet_ntoa(addr);
                }
            }
        }
    }
    WSACleanup();
    return IPStr;

}

// ---------------------------------------------------------------------------
// shkim. 2015-11-20
//        이 함수는 바로 아래의 TSocketForm::OHTSocketRead() 을 일부 수정하여 만든 것임.
//        소켓의 Read시, UDP인 경우에는 실패 case가 없으므로 (하위에서 이미 처리) 이 부분은 삭제하였다.
//        그리고 날아온 패킷에서 Common이면 패킷을 재 복사하여 기존대로 처리하고, 아니라면
//        새로 추가된 명령으로 보고 별도 처리하도록 한다

void TSocketForm::UdpSocketRead(PKT_OHT2OCS Pkt, int iMachineType)
{

	static int ReadCount = 0;

	ACK_DATA ACKData; // Ack할 정보를 가진 구조체

	int nError	 = NO_ERR;
	int RecvLen = 0;			 // 수신한 bytes
	char RecvBuf[LEN_TLV_BUFFER]; // 수신한 내용이 저장되는 버퍼
	//int RecvBytes = Socket->ReceiveLength();

	// 이전 타입의 패킷 모양으로 메모리 복사
	RecvLen = MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OHT);
	memset(RecvBuf, 0, LEN_TLV_BUFFER); // 메모리 클리어
	memcpy(RecvBuf, &Pkt.Value, RecvLen);

	// if(Pkt.TAG.Value == TAGCATEGORY::TAG_CATEGORY_OCSCOMMON)
	if(Pkt.TAG.Value == TagID::CMD_TP_COMMON) // TAGCATEGORY::TAG_CATEGORY_OCSCOMMON)
	{
		// -----------------------------------------------------------------------------
		// 아래 함수의 "TLV Socket을 Read하기 위해 호출" 부분부터 중괄호 끝까지와 동일
		// -----------------------------------------------------------------------------
		AnsiString strTmp;
		AnsiString strLog;

		// -------------------------------------------
		// [BEGIN] by zzang9un 2013.11.11 : IO Map 송신
		strLog = "[Rcv]";
		for(int i = 0; i < RecvLen; i++)
		{
			strTmp.sprintf("%02X", (BYTE)RecvBuf[i]);
			strLog += strTmp;
		}
		TOHTLOGFILE *pLogFile = new TOHTLOGFILE;
		pLogFile->WriteLogFile("CommOHT", strLog);
		// [END] by zzang9un 2013.11.11 : IO Map 송신
		// -------------------------------------------

		// by zzang9un 2013.03.18 : OHT Command Parsing
		nError = ParsingOHTCmd(RecvBuf, false);

		// by zzang9un 2013.03.07 : Status 받을 때만 설정하도록 이동해야 함
		gm_OHTStatus->bReceiveStatus = true;

		// by zzang9un 2012.11.24 : OHT와 통신 연결 시간 초기화
		MainForm->timerCommCheck->Enabled = false;

		ReadCount++;

		// OHT와 통신 상태를 Progressbar에 표시함
		if(MainForm->pbCommStatus->Position < MainForm->pbCommStatus->Max)
		{
			MainForm->pbCommStatus->Position += 10;
		}
		else if(MainForm->pbCommStatus->Position >= MainForm->pbCommStatus->Max)
		{
			MainForm->pbCommStatus->Position = 0;
		}
	}
	else	
	{
		// New type command process	
		
		ShowMessage("Not Implemented!!");
		
	}
	// -------------------------------------------------------------------------
	
	if(ReadCount > 2)
	{
		MainForm->panReceive->Color = COLOR_WHITE;
		ReadCount = 0;
	}
	else
	{
		MainForm->panReceive->Color = COLOR_LIME;
	}
}



void TSocketForm::UdpSocket_Status_Read(PKT_OHT2TP_RESPONSE Pkt, int iMachineType)
{
	static int ReadCount = 0;

     //VHL 호기명 실제 받아오기
	 memcpy(FoupOnlineCMDForm->m_VHLNAME, Pkt.HEADER.MachineID, sizeof(Pkt.HEADER.MachineID));
     memcpy(EUVOnlineCMDForm->m_VHLNAME, Pkt.HEADER.MachineID, sizeof(Pkt.HEADER.MachineID));
	if (Pkt.TAG.Value == TagID::CMD_STATUSREQ_TP_RESPONSE)
	{

		memmove(&gm_OHTStatus->m_OHTStatus, &Pkt.OHT_STATUS, sizeof(MSG_STATUS_OHT2TP));
	}
	else if(Pkt.TAG.Value == TagID::CMD_INITREQ_TP_RESPONSE)
	{

		memmove(&gm_OHTStatus->m_OHTStatus, &Pkt.OHT_STATUS, sizeof(MSG_STATUS_OHT2TP));
		memmove(&gm_OHTStatus->m_OHTInfo, &Pkt.OHT_BODY, sizeof(uTP_CMD));

		{
			// TP로 보내는 OHT Info인 경우(For TP)
			// memset(&gm_OHTStatus->m_OHTInfo, 0x00, sizeof(uTP_CMD));

			// by zzang9un 2013.03.22 : OHT Info 반영
			// by zzang9un 2013.03.22 : 추후 함수 변경 필요
			gm_OHTStatus->strLineInfo = String(gm_OHTStatus->m_OHTInfo.OHTInfo.LineInfo);
			gm_OHTStatus->strOHTNum   = String(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_ID);
			gm_OHTStatus->strOHTVer   = GetStringOHTVer(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver);
			gm_OHTStatus->strOHTType  = GetStringOHTType(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type);

			if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
			{
				FoupOnlineCMDForm->chkLongRun->Checked = false;
				FoupOnlineCMDForm->Show();
				if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70)
				|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71)
				|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_80)
				|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_81))
				{
					FoupOnlineCMDForm->panMAC_Detect->Visible = false;
				}
				else if (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_72)
				{
					FoupOnlineCMDForm->panFoupDetect->Visible = false;
				}
			}
			else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
            {
				EUVOnlineCMDForm->chkLongRun->Checked = false;
                // 2016-11-28, Rotate
                // Add by Labeler
                // Rotate 제어를 위해 강제 사용 표시
                gm_OHTStatus->nIsRotateExist = gm_OHTStatus->m_OHTStatus.IsRotateExist;

				EUVOnlineCMDForm->Show();
				if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) ||
				(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85) ||
				(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
				{
					//EUVOnlineCMDForm->panMAC_Detect->Visible = false;
				}
				else if (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_72)
				{
					EUVOnlineCMDForm->panFoupDetect->Visible = false;
				}
            }
		
		}	
	}


	// ------------- connection check ---------------
	ReadCount++;

	// OHT와 통신 상태를 Progressbar에 표시함
	if(MainForm->pbCommStatus->Position < MainForm->pbCommStatus->Max)
	{
		MainForm->pbCommStatus->Position += 10;
	}
	else if(MainForm->pbCommStatus->Position >= MainForm->pbCommStatus->Max)
	{
		MainForm->pbCommStatus->Position = 0;
	}

	if(ReadCount > 2)
	{
		MainForm->panReceive->Color = COLOR_WHITE;
		ReadCount = 0;
	}
	else
	{
		MainForm->panReceive->Color = COLOR_LIME;
	}
	// -----------------------------------------------


/*
	AnsiString strTmp;

	int a = Pkt.OHT_STATUS.HID_Erec;
	int b = Pkt.OHT_STATUS.HID_Edc;
	int c = Pkt.OHT_STATUS.HID_Idc;
	int d = Pkt.OHT_STATUS.HID_Ecap;
	int e = Pkt.OHT_STATUS.HID_Kw;

	memmove(&gm_OHTStatus->m_OHTStatus, &Pkt, sizeof(PKT_OHT2TP_RESPONSE));
*/

	// strTmp.sprintf("OHT상태수신 a = %d  b = %d  c = %d  d = %d  e = %d ", a, b, c, d, e);

	// ShowMessage(strTmp);

/*
	static int ReadCount = 0;

	ACK_DATA ACKData; // Ack할 정보를 가진 구조체

	int nError	 = NO_ERR;
	int RecvLen = 0;			 // 수신한 bytes
	char RecvBuf[LEN_TLV_BUFFER]; // 수신한 내용이 저장되는 버퍼
	//int RecvBytes = Socket->ReceiveLength();

	// 이전 타입의 패킷 모양으로 메모리 복사
	RecvLen = MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OHT);
	memset(RecvBuf, 0, LEN_TLV_BUFFER); // 메모리 클리어
	memcpy(RecvBuf, &Pkt.Value, RecvLen);

	if(Pkt.TAG.Value == TAGCATEGORY::TAG_CATEGORY_OCSCOMMON)
	{
		// -----------------------------------------------------------------------------
		// 아래 함수의 "TLV Socket을 Read하기 위해 호출" 부분부터 중괄호 끝까지와 동일
		// -----------------------------------------------------------------------------
		AnsiString strTmp;
		AnsiString strLog;

		// -------------------------------------------
		// [BEGIN] by zzang9un 2013.11.11 : IO Map 송신
		strLog = "[Rcv]";
		for(int i = 0; i < RecvLen; i++)
		{
			strTmp.sprintf("%02X", (BYTE)RecvBuf[i]);
			strLog += strTmp;
		}
		TOHTLOGFILE *pLogFile = new TOHTLOGFILE;
		pLogFile->WriteLogFile("CommOHT", strLog);
		// [END] by zzang9un 2013.11.11 : IO Map 송신
		// -------------------------------------------

		// by zzang9un 2013.03.18 : OHT Command Parsing
		nError = ParsingOHTCmd(RecvBuf, false);

		// by zzang9un 2013.03.07 : Status 받을 때만 설정하도록 이동해야 함
		gm_OHTStatus->bReceiveStatus = true;

		// by zzang9un 2012.11.24 : OHT와 통신 연결 시간 초기화
		MainForm->timerCommCheck->Enabled = false;

		ReadCount++;

		// OHT와 통신 상태를 Progressbar에 표시함
		if(MainForm->pbCommStatus->Position < MainForm->pbCommStatus->Max)
		{
			MainForm->pbCommStatus->Position += 10;
		}
		else if(MainForm->pbCommStatus->Position >= MainForm->pbCommStatus->Max)
		{
			MainForm->pbCommStatus->Position = 0;
		}
	}
	else	
	{
		// New type command process 
		
		ShowMessage("Not Implemented!!");
		
	}
	// -------------------------------------------------------------------------
	
	if(ReadCount > 2)
	{
		MainForm->panReceive->Color = COLOR_WHITE;
		ReadCount = 0;
	}
	else
	{
		MainForm->panReceive->Color = COLOR_LIME;
	}
*/
}

void TSocketForm::UdpSocket_IO_LIST_Read(PKT_OHT2TP_IOLIST Pkt)
{
	
	m_IOMapField->fIsLoaded = true; 

	m_IOMapField->InputNum = Pkt.BODY.iOhtInputNum;
	m_IOMapField->OutputNum = Pkt.BODY.iOhtOutputNum;

	m_IOMapField->InputField = new AnsiString[m_IOMapField->InputNum]; 
	m_IOMapField->OutputField = new AnsiString[m_IOMapField->OutputNum];

	BYTE strLen = 0;
	int Offset = 0;

	if((m_IOMapField->InputNum < 1) || (m_IOMapField->OutputNum < 1))
	{
		MessageDlg("OHT IO Map Field Data is empty", mtWarning, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	for(int i=0; i<m_IOMapField->InputNum; i++)
	{
		//문자열 길이 읽어옴
		memcpy(&strLen, (char*)&Pkt.BODY.cIONameList + Offset, sizeof(BYTE));
		Offset += sizeof(BYTE);

		//길이만큼 데이터를 읽음
		char* tmpBuf = new char[strLen];
		memcpy(tmpBuf, (char*)&Pkt.BODY.cIONameList + Offset, strLen);

		m_IOMapField->InputField[i] = tmpBuf;
		Offset += strLen;
		delete tmpBuf;
	}

	for(int i=0; i<m_IOMapField->OutputNum; i++)
	{
		//문자열 길이 읽어옴
		memcpy(&strLen, (char*)&Pkt.BODY.cIONameList + Offset, sizeof(BYTE));
		Offset += sizeof(BYTE);

		//길이만큼 데이터를 읽음
		char* tmpBuf = new char[strLen];
		memcpy(tmpBuf, (char*)&Pkt.BODY.cIONameList + Offset, strLen);
		m_IOMapField->OutputField[i] = tmpBuf;
		Offset += strLen;
		delete tmpBuf;
	}

	fIsIOMapLoaded = true;
	IOMapForm->InitIOMap(m_IOMapField);
	IOMapForm->Show();
}

void TSocketForm::UdpSocket_CID_INFO(PKT_OHT2TP_CID_INFO PKT)
{
    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
    {
		if(FoupOnlineCMDForm != NULL)
			FoupOnlineCMDForm->Display_CID_INFO(PKT);
	}
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
		if(EUVOnlineCMDForm != NULL)
			EUVOnlineCMDForm->Display_CID_INFO(PKT);
	}
}


void TSocketForm::UdpSocket_MapMove_Result(PKT_OHT2TP_MOVE_BY_MAP_RESULT PKT)
{
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
    {
		if(FoupOnlineCMDForm != NULL)
			FoupOnlineCMDForm->Display_MapMove_Result(PKT);
	}
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
		if(EUVOnlineCMDForm != NULL)
			EUVOnlineCMDForm->Display_MapMove_Result(PKT);
	}
}

void TSocketForm::UdpSocket_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT)
{
	MapMakerForm->Display_MapMake_Result(PKT);
}


void TSocketForm::UdpSocket_AutoTeach_Result(PKT_TP2OHT_SIMPLE_COMMAND PKT)
{
	if(TeachingForm != NULL)
		TeachingForm->Display_Teaching_Result(PKT);
}


void TSocketForm::UdpSocket_Standard_Read(PKT_OHT2OCS PKT)
{
	if (PKT.TAG.Value == TagID::JOB_ORDER_RESPONSE_TP)
    {
    	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
        {
            if(FoupOnlineCMDForm != NULL)
                FoupOnlineCMDForm->Display_Com(PKT);
        }
        else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
        {
            if(EUVOnlineCMDForm != NULL)
                EUVOnlineCMDForm->Display_Com(PKT);
        }
    }
}

// 데이터 전송 지시 응답
void TSocketForm::UdpSocket_DataTransper_Response_Read(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE PKT)
{
	AnsiString strLog;
	AnsiString strTmp;
	int iTmp = 0;

	bool bMapOrg = false;	// 맵 조정용
	bool bMapRun = false;   // 맵 운용용
	bool bFWU = false;		// 펌웨어 업데이트

	bool bAccept = false;	// 접수 불가의 경우 true

	strLog = "[데이터 전송 지시 응답]";
	// --------------------------------------

	if((PKT.BODY.Request.TestMode & 0x80) == 0x80)	// bit7 is 1
	{
		strLog = strLog + " 전송방향 : OHT->FTP";
	}
	else
	{
		strLog = strLog + " 전송방향 : FTP->OHT";
	}
	// --------------------------------------

	if((PKT.BODY.Request.CMDType & 0x01) == 0x01)	// bit0 is 1
		bMapOrg = true;

	if((PKT.BODY.Request.CMDType & 0x02) == 0x02)	// bit0 is 1
		bMapRun = true;

	if((PKT.BODY.Request.CMDType & 0x40) == 0x40)	// bit0 is 1
		bFWU = true;

	strTmp.sprintf(" 전송내용 - 맵조정용 : %d  맴운영용 : %d 펌웨어업데이트용 : %d",
					bMapOrg, bMapRun, bFWU);
	strLog = strLog + strTmp;
	// --------------------------------------

	if((PKT.BODY.Ack.Accept_Error & 0x80) == 0x80)	// 1이면 접수불가
		bAccept = true;
	strTmp.sprintf(" 접수불가여부(1=불가) : %d", bAccept);
	strLog = strLog + strTmp;
	// --------------------------------------

	iTmp = PKT.BODY.Ack.Accept_Error_Class;
	strTmp.sprintf(" 접수불가구분코드 : %d", iTmp);
	strLog = strLog + strTmp;
	// --------------------------------------

	if(FirmwareUpdateForm != NULL)
	{
		FirmwareUpdateForm->FWU_Display(strLog);
	}

	if(frmDataTransperReq != NULL)
	{
		frmDataTransperReq->DataTranaper_Display(strLog);
	}
}

// 데이터 전송 진척 보고
void TSocketForm::UdpSocket_Transper_Progress_Read(PKT_DATA_TRANSPER_ORDER_PROGRESS PKT)
{
	AnsiString strLog;
	AnsiString strTmp;
	int iTmp = 0;

	bool bMapOrg = false;	// 맵 조정용
	bool bMapRun = false;   // 맵 운용용
	bool bFWU = false;		// 펌웨어 업데이트

	bool bTransOk = false;		// 전송성공
	bool bTransFail = false;	// 전송실패

	strLog = "[데이터 전송 진척 보고]";

	// --------------------------------------

	if((PKT.BODY.Request.TestMode & 0x80) == 0x80)	// bit7 is 1
	{
		strLog = strLog + " 전송방향 : OHT->FTP";
	}
	else
	{
		strLog = strLog + " 전송방향 : FTP->OHT";
	}
	// --------------------------------------

	if((PKT.BODY.Request.CMDType & 0x01) == 0x01)	// bit0 is 1
		bMapOrg = true;

	if((PKT.BODY.Request.CMDType & 0x02) == 0x02)	// bit0 is 1
		bMapRun = true;

	if((PKT.BODY.Request.CMDType & 0x40) == 0x40)	// bit0 is 1
		bFWU = true;

	strTmp.sprintf(" 전송내용 - 맵조정용 : %d  맴운영용 : %d 펌웨어업데이트용 : %d",
					bMapOrg, bMapRun, bFWU);
	strLog = strLog + strTmp;
	// --------------------------------------

	if(PKT.BODY.Result.Trans_OK == 1)
		bTransOk = true;

	if(PKT.BODY.Result.Trans_Fail == 1)
		bTransFail = true;

	strTmp.sprintf(" 전송성공flag : %d  전송실패flag : %d", bTransOk, bTransFail);
	strLog = strLog + strTmp;
	// --------------------------------------

	strTmp = " 전송상태 구분 - ";
	iTmp = PKT.BODY.Result.Not_Accept_Class;

	if(iTmp == 0x00)
		strTmp = strTmp + "정상완료";
	else if(iTmp == 0x01)
		strTmp = strTmp + "FTP 서버와 통신중";
	else if(iTmp == 0x02)
		strTmp = strTmp + "파일의 체크 처리중";
	else
		strTmp = strTmp + "파일 보존 처리중";

	strLog = strLog + strTmp;
	// --------------------------------------

	iTmp = PKT.BODY.FailClass;
	strTmp.sprintf(" 오류코드 : 0x%02X",  iTmp);
	strLog = strLog + strTmp;

	if(FirmwareUpdateForm != NULL)
	{
		FirmwareUpdateForm->FWU_Display(strLog);
	}

	if(frmDataTransperReq != NULL)
	{
		frmDataTransperReq->DataTranaper_Display(strLog);
	}
}

// 버전 상세 보고
void TSocketForm::UdpSocket_VersionReq_Response_Read(PKT_VERSION_RESPONSE PKT)
{
	AnsiString strLog;

	char cDisplay[513];
	AnsiString strTmp;

	memset(&cDisplay, '\0', 513);
	memcpy(&cDisplay, PKT.BODY.Version,512);
	strTmp = cDisplay;

	strLog.sprintf( "[버전 상세보고] %s", strTmp);

	if(FirmwareUpdateForm != NULL)
	{
    	FirmwareUpdateForm->FWU_Display(strLog);
	}

	if(frmDataTransperReq != NULL)
	{
		frmDataTransperReq->DataTranaper_Display(strLog);
	}
}

// ---------------------------------------------------------------------------

void TSocketForm::UdpSocket_HID_LogSave_Result_Read(PKT_TP2OHT_SIMPLE_COMMAND PKT)
{
	if(frmHidLogGet != NULL)
	{
    	frmHidLogGet->Display_Result(PKT.BODY.iVar1, PKT.BODY.iVar2);
	}
}

// ---------------------------------------------------------------------------

void TSocketForm::UdpSocket_Hoist_Param_Result_Read(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT)
{
	if(HoistParamForm != NULL)
	{
    	HoistParamForm->Display_Result(PKT);
	}
}
// ---------------------------------------------------------------------------
/**
@brief   Soket Read 요청시 발생하는 이벤트 함수(OHT)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::OHTSocketRead(TObject *Sender,
    TCustomWinSocket *Socket)
{
	static int ReadCount = 0;

	ACK_DATA ACKData; // Ack할 정보를 가진 구조체

	int nError   = NO_ERR;
    int RecvLen = 0;             // 수신한 bytes
    char RecvBuf[LEN_TLV_BUFFER]; // 수신한 내용이 저장되는 버퍼
	int RecvBytes = Socket->ReceiveLength();

	if(RecvBytes == 0)
	{
		Socket->ReceiveBuf(RecvBuf, 0);
        return;
    }
    else
    {
        // TLV Socket을 Read하기 위해 호출
		nError = ReadSocket(Socket, RecvBuf, &RecvLen, false);

        // by zzang9un 2013.03.19 : 0보다 작으면 에러
        if(nError < 0)
        {
            ///@todo 에러 처리
            gm_OHTStatus->bReceiveStatus = false;
            return;
        }
        else if(RecvLen <= 0)
        {
            ///@todo 에러 처리
            gm_OHTStatus->bReceiveStatus = false;
            return;
        }
        else
        {
            AnsiString strTmp;
            AnsiString strLog;

			// -------------------------------------------
            // [BEGIN] by zzang9un 2013.11.11 : IO Map 송신
            strLog = "[Rcv]";
            for(int i = 0; i < RecvLen; i++)
            {
				strTmp.sprintf("%02X", (BYTE)RecvBuf[i]);
                strLog += strTmp;
            }
            TOHTLOGFILE *pLogFile = new TOHTLOGFILE;
            pLogFile->WriteLogFile("CommOHT", strLog);
            // [END] by zzang9un 2013.11.11 : IO Map 송신
            // -------------------------------------------

            // by zzang9un 2013.03.18 : OHT Command Parsing
			nError = ParsingOHTCmd(RecvBuf, false);

            // by zzang9un 2013.03.07 : Status 받을 때만 설정하도록 이동해야 함
            gm_OHTStatus->bReceiveStatus = true;

            // by zzang9un 2012.11.24 : OHT와 통신 연결 시간 초기화
            MainForm->timerCommCheck->Enabled = false;

            ReadCount++;

            // OHT와 통신 상태를 Progressbar에 표시함
            if(MainForm->pbCommStatus->Position < MainForm->pbCommStatus->Max)
			{
                MainForm->pbCommStatus->Position += 10;
            }
            else if(MainForm->pbCommStatus->Position >= MainForm->pbCommStatus->Max)
            {
                MainForm->pbCommStatus->Position = 0;
			}
        }
    }

    if(ReadCount > 2)
    {
        MainForm->panReceive->Color = COLOR_WHITE;
        ReadCount = 0;
    }
    else
    {
        MainForm->panReceive->Color = COLOR_LIME;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   버퍼를 OHT로 Send하는 함수 (OHT)
@param   SndBuf : 보낼 버퍼의 포인터
@param   nSndLen : 보내야할 bytes 수
*/
void TSocketForm::SendTPCmd2OHT(char *SndBuf, int nSndLen)
{
    static int nTmpCount = 0;
    String strDisplay;


	// shkim modify
    // OHTSocket->Socket->SendBuf(SndBuf, nSndLen);
	//char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OCS2OHT);
	int iReturn, iTemp;
	
	PKT_OCS2OHT SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH); 

	// tag
	SEND_PACKET.TAG.Value = TagID::CMD_TP_COMMON;	// TAGCATEGORY::TAG_CATEGORY_OCSCOMMON;

	// status skip

	// value
	memcpy(&SEND_PACKET.Value, SndBuf, nSndLen);
	
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	// -------------------------------------------
	
    strDisplay.sprintf(L"Send Cmd To OHT : %d bytes,  New Packet Size : %d", nSndLen, iPacketSize);
    CommDisplayForm->Communicaion_Display(strDisplay);

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.11.11 : TP Cmd 송신 통신 로그
    AnsiString strLog = "[Snd]";
    AnsiString strTmp = "";

    for(int i = 0; i < nSndLen; i++)
    {
        strTmp.sprintf("%02X", (BYTE)SndBuf[i]);
        strLog += strTmp;
    }
    
    TOHTLOGFILE *pLogFile = new TOHTLOGFILE;
    pLogFile->WriteLogFile("CommOHT", strLog);
    // [END] by zzang9un 2013.11.11 : TP 수신 송신 로그
    // -------------------------------------------

    nTmpCount++;
    if(nTmpCount > 2)
    {
        MainForm->panSend->Color = COLOR_WHITE;
        nTmpCount = 0;
    }
    else
    {
        MainForm->panSend->Color = COLOR_FUCHSIA;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   버퍼를 Assistant로 Send하는 함수
@author  zzang9un
@date    2013.11.07
@param   SndBuf : 보낼 버퍼의 포인터
@param   nSndLen : 보내야할 bytes 수
*/
void TSocketForm::SendTPCmd2AST(char *SndBuf, int nSndLen)
{
    String strDisplay;

    AssistantSocket->Socket->SendBuf(SndBuf, nSndLen);
    strDisplay.sprintf(L"Send Cmd To Assistant : %d bytes", nSndLen);
    CommDisplayForm->Communicaion_Display(strDisplay);

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.11.11 : Assistant Cmd 송신 통신 로그
    AnsiString strLog = "[Snd]";
    AnsiString strTmp = "";
    
    for(int i = 0; i < nSndLen; i++)
    {
        strTmp.sprintf("%02X", (BYTE)SndBuf[i]);
        strLog += strTmp;
    }
    
    TOHTLOGFILE *pLogFile = new TOHTLOGFILE;
    pLogFile->WriteLogFile("CommAssistant", strLog);
    // [END] by zzang9un 2013.11.11 : Assistant 수신 송신 로그
    // -------------------------------------------
}
// ---------------------------------------------------------------------------

void TSocketForm::AssistantConnect(String strIPAddress, String strOHTName)
{
    AssistantSocket->Address = strIPAddress;
    AssistantSocket->Active = true;

    m_strOHTName      = strOHTName;
    m_strOHTIPAddress = strIPAddress;

    MainForm->ppanVer->Visible = true;
}
// ---------------------------------------------------------------------------

void TSocketForm::Assistant_DisConnect()
{
    AssistantSocket->Active = false;
}
// ---------------------------------------------------------------------------

/**
@brief   TP에서 Assistant으로 데이터를 Send하는 함수(Assistant)
@author  puting
@date    2012.12.14
*/
void TSocketForm::Assistant_Command2OHT(String strTmpCommand)
{
    AssistantSocket->Socket->SendText(strTmpCommand);
    CommDisplayForm->Communicaion_Display(strTmpCommand);
    TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
    m_LogFile->WriteLogFile("Communication", strTmpCommand);
    delete m_LogFile;
}
// ---------------------------------------------------------------------------
/**
@brief   TP에서 Assistant으로 데이터를 Send하는 함수(Assistant)
@author  puting
@date    2012.12.14
*/
void TSocketForm::BayTestLongrun_Log(String strTmpCommand)
{
    AssistantSocket->Socket->SendText(strTmpCommand);
    CommDisplayForm->Communicaion_Display(strTmpCommand);
    TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
    m_LogFile->WriteLogFile("BayTestLongrun", strTmpCommand);
    delete m_LogFile;
}

/**
@brief   Soket Connect 요청시 발생하는 이벤트 함수(Assistant)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::AssistantSocketConnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    ConnectForm->Close();
    MainForm->DataDisplay(COLOR_BLUE, m_strOHTName + "Assistant Connect...:" + AssistantSocket->Address);

    //OHT Status 정보 요청
    uTP_CMD TPCmd;
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_ADDITIONAL_STATUS, &TPCmd);

    delete m_CMDControl;

    //Timer start
	MainForm->timerAssistant->Enabled = true;

	if(OHTSocket->Socket->Connected == false)
	{
		// 2016.12.29.  어시스턴스 뜨는 위치 조정
		int nTmpLeft, nTmpBott;

		nTmpLeft = MainForm->Left;
		nTmpBott = MainForm->Top + MainForm->Height;

		AssistantForm->Top = nTmpBott + 1;
		AssistantForm->Left = nTmpLeft;

		// 2017.01.24  어시스턴스 처음에는 안보이게 함
		// AssistantForm->Show();
	}
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Connecting 요청시 발생하는 이벤트 함수(Assistant)
@author  puting
@date    2012.12.14
@param   Soket Connecting Event
*/
void __fastcall TSocketForm::AssistantSocketConnecting(TObject *Sender,
    TCustomWinSocket *Socket)
{
    MainForm->DataDisplay(COLOR_BLUE, m_strOHTName + " Assistant Connecting...:" + AssistantSocket->Address);
}
// ---------------------------------------------------------------------------

/**
@brief   Soket DisConnect 요청시 발생하는 이벤트 함수(Assistant)
@author  puting
@date    2012.12.14
@param   Soket DisConnect Event
*/
void __fastcall TSocketForm::AssistantSocketDisconnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    AssistantSocket->Close();

    MainForm->DataDisplay(COLOR_BLUE, "Assistant Disconnect...");
	// MainForm->mniConnect->Enabled    = true;
    // MainForm->mniDisconnect->Enabled = false;
    MainForm->timerAssistant->Enabled = false;

    if(AssistantForm->Showing)
        AssistantForm->Close();
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Error 요청시 발생하는 이벤트 함수(Assistant)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::AssistantSocketError(TObject *Sender,
    TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    if((ErrorEvent == eeConnect))
    {
        MainForm->DataDisplay(COLOR_RED, "ERROR : " + m_strOHTName + " Assistant Connection Error...");
    }
    ErrorCode = 0;

}
// ---------------------------------------------------------------------------

/**
@brief   Soket Read 요청시 발생하는 이벤트 함수(Assistant)
@author  puting
@date    2012.12.14
*/
void __fastcall TSocketForm::AssistantSocketRead(TObject *Sender,
    TCustomWinSocket *Socket)
{
//    String strTmpData;
//    int nCmdType;
//
//    strTmpData = Socket->ReceiveText();
//    strTmpData.Trim();
//    MainForm->DataDisplay(COLOR_GREEN, strTmpData);
//
//    TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
//    m_LogFile->WriteLogFile("Communication", "Assistant Rcv : " + strTmpData);
//  delete m_LogFile;

    static int ReadCount = 0;

    ACK_DATA ACKData; // Ack할 정보를 가진 구조체

    int nError   = NO_ERR;
    int RecvLen = 0;             // 수신한 bytes
    char RecvBuf[LEN_TLV_BUFFER]; // 수신한 내용이 저장되는 버퍼
    int RecvBytes = Socket->ReceiveLength();

    if(RecvBytes == 0)
    {
        Socket->ReceiveBuf(RecvBuf, 0);
        return;
    }
    else
    {
        // TLV Socket을 Read하기 위해 호출
        nError = ReadSocket(Socket, RecvBuf, &RecvLen, false);

        // by zzang9un 2013.03.19 : 0보다 작으면 에러
        if(nError < 0)
        {
            ///@todo 에러 처
            return;
        }
        else if(RecvLen <= 0)
        {
            ///@todo 에러 처리
            return;
        }
        else
        {

            // by zzang9un 2013.03.18 : OHT Command Parsing
            nError = ParsingAstCmd(RecvBuf, false);

            // by zzang9un 2012.11.24 : OHT와 통신 연결 시간 초기화
            MainForm->timerCommCheck->Enabled = false;

            ReadCount++;

            // OHT와 통신 상태를 Progressbar에 표시함
            if(MainForm->pbCommStatus->Position < MainForm->pbCommStatus->Max)
            {
                MainForm->pbCommStatus->Position += 10;
            }
            else if(MainForm->pbCommStatus->Position >= MainForm->pbCommStatus->Max)
            {
                MainForm->pbCommStatus->Position = 0;
            }
        }
    }
}
// ---------------------------------------------------------------------------

void TSocketForm::AutoTeaching_Connect()
{
    AutoTeachingSocket->Address = TeachingForm->edIP->Text;
    AutoTeachingSocket->Open();
}
// ---------------------------------------------------------------------------
void TSocketForm::AutoTeaching_DisConnect()
{
    AutoTeachingSocket->Close();
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Connect 요청시 발생하는 이벤트 함수(AutoTeaching)
@author  puting
@date    2012.12.14
@param   Soket Connect Event
*/
void __fastcall TSocketForm::AutoTeachingSocketConnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    MainForm->DataDisplay(COLOR_BLUE, "AutoTeaching Connect...:" + AssistantSocket->Address);

    // Auto Teaching Panel 활성화
    TeachingForm->panAutoTeaching->Visible = true;
    
    TeachingForm->btnATConnect->Enabled  = false;
    TeachingForm->btn12mmUp->Visible     = true;
    TeachingForm->btn12mmDown->Visible   = true;
    TeachingForm->btnATRead->Visible     = true;
    TeachingForm->btnATMove->Visible     = true;
    TeachingForm->btnATReadMove->Visible = true;
    TeachingForm->btnATDiscon->Enabled   = true;
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Connecting 요청시 발생하는 이벤트 함수(AutoTeaching)
@author  puting
@date    2012.12.14
@param   Soket Connecting Event
*/
void __fastcall TSocketForm::AutoTeachingSocketConnecting(TObject *Sender,
    TCustomWinSocket *Socket)
{
    MainForm->DataDisplay(COLOR_BLUE, "AutoTeaching Connecting...:" + AssistantSocket->Address);
}
// ---------------------------------------------------------------------------

/**
@brief   Soket DisConnect 요청시 발생하는 이벤트 함수(AutoTeaching)
@author  puting
@date    2012.12.14
@param   Soket DisConnect Event
*/
void __fastcall TSocketForm::AutoTeachingSocketDisconnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    AutoTeachingSocket->Close();

    MainForm->DataDisplay(COLOR_BLUE, "AutoTeaching Disconnect...");

    // Auto Teaching Panel 비활성화
    TeachingForm->panAutoTeaching->Visible = false;
    
    TeachingForm->btnATConnect->Enabled  = true;
    TeachingForm->btn12mmUp->Visible     = false;
    TeachingForm->btn12mmDown->Visible   = false;
    TeachingForm->btnATRead->Visible     = false;
    TeachingForm->btnATMove->Visible     = false;
    TeachingForm->btnATReadMove->Visible = false;
    TeachingForm->btnATDiscon->Enabled   = false;
}
// ---------------------------------------------------------------------------

/**
@brief   Soket Error 요청시 발생하는 이벤트 함수(AutoTeaching)
@author  puting
@date    2012.12.14
@param   Soket Error Event
*/
void __fastcall TSocketForm::AutoTeachingSocketError(TObject *Sender,
    TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    if((ErrorEvent == eeConnect))
    {
        MainForm->DataDisplay(COLOR_RED, "ERROR : AutoTeaching Connection Error...");
    }
    ErrorCode = 0;
}
// ---------------------------------------------------------------------------

/**
@brief   TP에서 AutoTeaching으로 데이터를 Send하는 함수(AutoTeaching)
@author  puting
@date    2012.12.14
*/
void TSocketForm::AutoTeaching_Command2AT()
{
    AutoTeachingSocket->Socket->SendText("[AT Offset Require]");
    CommDisplayForm->Communicaion_Display("[AT Offset Require]");
}

void __fastcall TSocketForm::AutoTeachingSocketRead(TObject *Sender,
    TCustomWinSocket *Socket)
{
    String strATData;
    String strATHoistData;
    String strATShiftData;
    String strATRotateData;
    int iATLength, iAT2Length, iAT3Length, iAT4Length;
    strATData = Socket->ReceiveText();
    strATData.Trim();

    iATLength  = strATData.Length();
    iAT2Length = strATData.Pos(",");
    strATHoistData = strATData.SubString(1, (iAT2Length - 1));
    float dHoistData = StrToFloat(strATHoistData);

    strATData  = strATData.Delete(1, iAT2Length);
    iATLength  = strATData.Length();
    iAT3Length = strATData.Pos(",");
    strATShiftData = strATData.SubString(1, (iAT3Length - 1));
    float dShiftData = StrToFloat(strATShiftData);

    strATData  = strATData.Delete(1, iAT3Length);
    iATLength  = strATData.Length();
    strATRotateData = strATData.SubString(1, iATLength);
    float dSRotateData = StrToFloat(strATRotateData);

    // by zzang9un 2013.07.31 : 소수점 자리는 출력하지 않도록 변경함(반올림)
    TeachingForm->edtHoistJogDist->Text = FormatFloat("#", dHoistData);
    TeachingForm->edtShiftJogDist->Text = FormatFloat("#", dShiftData);
    TeachingForm->edtRotateJogDist->Text = FormatFloat("#", dSRotateData);

    if(strATHoistData == "N/A" || strATShiftData == "N/A" || strATRotateData == "N/A")
    {
        return;
    }

    TeachingForm->dAT2Data = strATHoistData.ToDouble();
    TeachingForm->dAT3Data = strATShiftData.ToDouble();
    TeachingForm->dAT4Data = strATRotateData.ToDouble();

    bATReceive = true;
}
// ---------------------------------------------------------------------------

/**
@brief   OHT Cmd를 파싱하는 함수
@author  zzang9un
@date    2013.03.18
@param   pRecvBuf : 수신한 버퍼 포인터
@param   IsConvert : Endian Convert가 필요한지 여부
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
@note    OHT Command는 초기화 정보, Status 정보가 있다.
@warning Status를 Decode하기 위해서 Status 구조체의 reference가 필요함
@todo    OHT의 ACK를 Parsing 하는 부분 추가 필요
*/
int TSocketForm::ParsingOHTCmd(char *pRecvBuf, bool IsConvert)
{
    int Error = NO_ERR;

	int DecodeSize = 0; // Decode한 전체 사이즈
	CMD_HEADER Header;  // Header를 parsing한 구조체

    // step1. Header parsing
	Header = GetHeader(pRecvBuf);

	// step2. Tag의 Type에 따라 Decode 함수 호출
	switch(Header.uTag.Info.CmdType)
	{
	case TAG_CMDTYPE_STATUS:
		if(Header.uTag.Info.Receiver == TAG_RECEIVER_TP)
        {
            // TP로 보내는 Status인 경우(For TP)
			memset(&gm_OHTStatus->m_OHTStatus, 0x00, sizeof(MSG_STATUS_OHT2TP));

            DecodeSize = Decode(pRecvBuf, &Header, &gm_OHTStatus->m_OHTStatus);
        }

        if(DecodeSize <= 0)
            Error = ERR_PROTOCOL_DECODEERR;

        break;

	case TAG_CMDTYPE_INIT:
        // by zzang9un 2013.03.18 : OHT는 TP접속 시 OHT Info를 보낸다.
        switch(Header.uTag.Info.CmdData)
        {
			case TAG_CMDDATA_INIT_INFO_TP :
            {
				if(Header.uTag.Info.Receiver == TAG_RECEIVER_TP)
                {
                    // TP로 보내는 OHT Info인 경우(For TP)
                    memset(&gm_OHTStatus->m_OHTInfo, 0x00, sizeof(uTP_CMD));

                    DecodeSize = Decode(pRecvBuf, &Header, &gm_OHTStatus->m_OHTInfo);

					// by zzang9un 2013.03.22 : OHT Info 반영
                    // by zzang9un 2013.03.22 : 추후 함수 변경 필요
					gm_OHTStatus->strLineInfo = String(gm_OHTStatus->m_OHTInfo.OHTInfo.LineInfo);
					gm_OHTStatus->strOHTNum   = String(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_ID);
					gm_OHTStatus->strOHTVer   = GetStringOHTVer(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver);
					gm_OHTStatus->strOHTType  = GetStringOHTType(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type);
					if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
					{
						FoupOnlineCMDForm->chkLongRun->Checked = false;
						FoupOnlineCMDForm->Show();
						if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70)
						|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71)
						|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_80)
						|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_81))
						{
							FoupOnlineCMDForm->panMAC_Detect->Visible = false;
						}
						else if (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_72)
						{
							FoupOnlineCMDForm->panFoupDetect->Visible = false;
						}
					}
                    // 2016-12-03, Add by Labeler
					else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
					{
						EUVOnlineCMDForm->chkLongRun->Checked = false;
						EUVOnlineCMDForm->Show();
							if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) ||
								(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85) ||
								(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
						{
							//EUVOnlineCMDForm->panMAC_Detect->Visible = false;
						}
						else if (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_72)
						{
							EUVOnlineCMDForm->panFoupDetect->Visible = false;
						}
					}
				
				}

                if(DecodeSize <= 0)
                    Error = ERR_PROTOCOL_DECODEERR;
                break;
            }
        }

        break;

    case TAG_CMDTYPE_DATA:
    {
        switch(Header.uTag.Info.CmdData)
        {
            case TAG_CMDDATA_DATA_IOMAP_TP :
            {
                MainForm->DataDisplay(COLOR_BLUE, L"Receive IO Map Field From OHT");

                // TP로 보내는 OHT Info인 경우(For TP)
                DecodeSize = Decode(pRecvBuf, &Header, m_IOMapField);
                MainForm->DataDisplay(COLOR_BLUE, L"Parse IO Map Field");

                if(DecodeSize > 0 )
                {
                    IOMapForm->InitIOMap(m_IOMapField);
                    fIsIOMapLoaded = true;
                }

                if(fIsIOMapLoaded)
                    IOMapForm->Show();
                else
                    MessageDlg("OHT doesn't have IO Map Field Data", mtWarning, TMsgDlgButtons() << mbOK, 0);

                break;
            }
            case TAG_CMDDATA_DATA_NOT_READY :
            {
                MessageDlg("OHT doesn't Ready For Data", mtWarning, TMsgDlgButtons() << mbOK, 0);
                break;
            }
        }

        break;
    }
    // by zzang9un 2013.07.23 : TP 명령에 의한 응답을 처리
    case TAG_CMDTYPE_TPCMD:
        // Teaching Data 응답
        if(Header.uTag.Info.CmdData == TPCMD_TEACHING_REQ)
        {
            uTP_CMD TeachingData;
            memset(&TeachingData, 0x00, sizeof(TeachingData));

            DecodeSize = Decode(pRecvBuf, &Header, &TeachingData);
//데이터 수신 표시
//안들어올듯...
            // by zzang9un 2013.07.19 : TeachingForm에 출력한다.
            //TeachingForm->DisplayTeachingData(&TeachingData);
        }

        // Map Maker 정보 응답
        else if(Header.uTag.Info.CmdData == TPCMD_MAPMAKE_INFO)
        {
            uTP_CMD MapInfo;
			memset(&MapInfo, 0x00, sizeof(MapInfo));

            DecodeSize = Decode(pRecvBuf, &Header, &MapInfo);

            // Data를 MapMakerForm 으로 넘겨준다.
			MapMakerForm->DisplayMapMakerInfo(MapInfo);
        }

    default:
        break;
    }

    return Error;

}
//---------------------------------------------------------------------------
/**
@brief   OHT Cmd를 파싱하는 함수
@author  zzang9un
@date    2013.03.18
@param   pRecvBuf : 수신한 버퍼 포인터
@param   IsConvert : Endian Convert가 필요한지 여부
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
@note    OHT Command는 초기화 정보, Status 정보가 있다.
@warning Status를 Decode하기 위해서 Status 구조체의 reference가 필요함
@todo    OHT의 ACK를 Parsing 하는 부분 추가 필요
*/
int TSocketForm::ParsingAstCmd(char *pRecvBuf, bool IsConvert)
{
    int Error = NO_ERR;

    int DecodeSize = 0; // Decode한 전체 사이즈
	CMD_HEADER Header;  // Header를 parsing한 구조체

    // step1. Header parsing
    Header = GetHeader(pRecvBuf);

    // step2. Tag의 Type에 따라 Decode 함수 호출
    switch(Header.uTag.Info.CmdType)
	{
	case TAG_CMDTYPE_DATA:
        if(Header.uTag.Info.CmdData == TAG_CMDDATA_DATA_OHT_STATUS)
        {
            OHT_ADDITIONAL_INFO *info = new OHT_ADDITIONAL_INFO;
            memset(info, 0, sizeof(OHT_ADDITIONAL_INFO));

            DecodeSize = DecodeOHTStatus_AST2TP(pRecvBuf, &Header, info, false);

            if(DecodeSize == 0)
            {
                //Error
                return Error;
            }

            SysInfoForm->SetOHTAdditionalInfo(info);
			AssistantForm->SetOHTAddidtionalInfo(info);

            delete info;
        }

    // by zzang9un 2013.07.23 : TP 명령에 의한 응답을 처리
	case TAG_CMDTYPE_TPCMD:

		//FBWF 요청에 대한 응답들
		if(Header.uTag.Info.CmdData == TPRET_AST_OHT_ACK_FBWF_STATUS)
		{
			BYTE status = pRecvBuf[LEN_HEADER];
			switch(status)
			{
			case FBWF_DISABLE:
			case FBWF_ENABLE:
			case FBWF_WILL_BE_ENABLED:
			case FBWF_WILL_BE_DISABLED:
			case FBWF_ERROR:
				AssistantForm->SetFBWFStatus(status);
				break;
			default:
				//buffer is invalid
				break;
			}

		}

		//OHT 현재 시간
		else if(Header.uTag.Info.CmdData == TPRET_OHT_TIMESETTING)
		{
			uTP_CMD *data = new uTP_CMD;
			memcpy(data, pRecvBuf + LEN_HEADER, Header.Length);
			String strTime;
            strTime.sprintf(L"%08d%06d", data->TPCmd.Data1, data->TPCmd.Data2);
            SysInfoForm->SetOHTCurrentTime(strTime);
		}

        else if(Header.uTag.Info.CmdData == TPRET_AST_OHT_MAINPROG_CLOSE)
        {
            uTP_CMD *data = new uTP_CMD;
			String msg;
            memcpy(data, pRecvBuf + LEN_HEADER, Header.Length);
			switch(data->TPCmd.CmdID)
			{
					case 0x00:
						msg = "Fail to terminate the [OHT.exe]...";
						break;
                    case 0x01:
                        msg = "Terminate the process [OHT.exe] clearly!";
                        break;
                    case 0x02:
						msg = "Kill the process [OHT.exe] forcibly.";
                        break;
            }

            MessageDlg(msg, mtCustom, TMsgDlgButtons()<<mbOK, NULL);
        }

	default:
		break;
	}

	return Error;

}
//---------------------------------------------------------------------------
void TSocketForm::DisplayTeachingData(PKT_OHT2TP_TEACHING_INFO SEND_PACKET)
{
	TeachingForm->DisplayTeachingData(&SEND_PACKET);
}

void TSocketForm::DisplayAutoTeachingData(PKT_OHT2TP_TEACHING_INFO SEND_PACKET)
{
	TeachingForm->DisplayAutoTeachingData(&SEND_PACKET);
}

void TSocketForm::DisplayDrivingMapData(PKT_OHT2TP_DRIVINGMAP_INFO SEND_PACKET)
{
	MapMakerForm->DisplayDrivingMapInfo(&SEND_PACKET);
}

void TSocketForm::UdpSocket_AVS_Vibration_Data_Read(PKT_OHT2TP_VIBRATION_INFO PKT)
{
	if(frmVibrationMeasurement != NULL)
	{
		frmVibrationMeasurement->DisplayVibrationData(PKT.BODY);
	}
}
// ---------------------------------------------------------------------------

void TSocketForm::UdpSocket_AVS_Response(int nResponse)
{
	if(frmVibrationMeasurement != NULL)
	{
		frmVibrationMeasurement->Receive_AVS_Response(nResponse);
	}
}

