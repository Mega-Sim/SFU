// ----------------------------------------------------------------------- ----

#include <vcl.h>
#include <dos.h>
#include <dir.h>

#pragma hdrstop

// For DaysBetween function
#include "DateUtils.hpp"

#include "MainUnit.h"
#include "ConnectUnit.h"
#include "SocketUnit.h"
#include "CMDControlUnit.h"
#include "OHTInfoUnit.h"
#include "FileUnit.h"

#include "StatusUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "CommDisplayUnit.h"
#include "MapMakerUnit.h"
#include "IOTest.h"
#include "IOMapUnit.h"
#include "Utility.h"
#include "AssistantUnit.h"
#include "Define.h"
#include "FileUnit.h"
//#include "DebugUnit.h"
#include "PIOUnit.h"
#include "OHTSysInfoUnit.h"
#include "WarnningUnit.h"
#include "M2AUnit.h"
#include "HIDGraphUnit.h"
#include "VelGraphUnit.h"
#include "VibrationMeasureUnit.h"
#include "CmdWinUnit.h"
#include "UserSelectUnit.h"
#include "TeachingUnit.h"
#include "LogFileUnit.h"
#include "ChangeIOFileUnit.h"
#include "ConMainUnit.h"
#include "AMCParaUnit.h"
#include "HoistParamUnit.h"
#include "ErrorUnit.h"
#include "LongRunUnit.h"
#include "HIDVelUnit.h"
#include "HIDVelSecUnit.h"
#include "E23PIOUnit.h"
#include "AmcDebugMain.h"
#include "APMonitoringUnit.h"
#include "ErrorSearchUnit.h"
#include "About.h"
#include "SplashUnit.h"
#include "Def_TP.h"
#include "OptionUnit.h"
#include "FirmwareUpdateUnit.h"

#include "AssistantUnit.h"
#include "UDP.h"

#include "DataTransperReq.h"

#include "Protocol_Define_Modify.h"

#include "HidLogGet.h"
#include "CidCheckerUnit.h"
#include "BayTestUnit.h"
#include "SensorSettingUnit.h"
#include "DistanceSensorViewer.h"
#include "CheckerUnit.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma resource "*.dfm"

#define INTERVAL_COMM_CHECK         1000        ///< 통신 체크 timer 주기
#define INTERVAL_STATUS_REQUEST     200         ///< OHT 상태 정보 요청 주기
#define INTERVAL_UPDATE_DISPLAY     200         ///< Display 갱신 주기

#define STATUS_NORMAL					0
#define STATUS_OBS_STOP					1
#define STATUS_PBS_STOP					2
#define STATUS_OHT_PAUSED    			3

TMainForm *MainForm;

// ---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
    String strFileAccessTime, strFileDate, strVersion;

    fLoadIOMap = false;

    g_strTitle = Application->Title;

    bExitFlag = false;

    timerInit();

    MainForm->Width = 416;

    strFileAccessTime = Get_FileAccessTime(Application->ExeName);
    strFileDate.sprintf(L"[Date:%s.%s.%s]", strFileAccessTime.SubString(1, 4), strFileAccessTime.SubString(5, 2),
        strFileAccessTime.SubString(7, 2));

    // by zzang9un 2013.02.18 : 프로그램 버전 정보 추가
    g_Caption_Name = g_strTitle + VerInfo.VersionString() + strFileDate;
	MainForm->Caption = g_Caption_Name;

	btnPause_SetReset->Caption == "Pause:Set";
	m_bIgnoreJungBokConnect = false;

}

// ----------------------------------------------------------------------------
TTextAttributes *__fastcall TMainForm::CurrText(void)
{
    return memoMain->SelAttributes;
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::WMEXITSIZEMOVE(TMessage & msg)
{
    int nTmpLeft, nTmpBott, nTmpTop, nTmpRight;

	nTmpLeft = MainForm->Left;
    nTmpTop = MainForm->Top;
    nTmpBott = MainForm->Top + MainForm->Height;
    nTmpRight = nTmpLeft + MainForm->Width;

    // StatusForm-> MainForm의 왼쪽,,나머지는 모두 아래.
    StatusForm->Top = nTmpTop;
    StatusForm->Left = nTmpRight;
    // StatusForm->Height=MainForm->Height;

    ConnectForm->Top = nTmpBott;
    ConnectForm->Left = nTmpLeft;
    ConnectForm->Width = MainForm->Width;

    FileForm->Top = nTmpBott;
    FileForm->Left = nTmpLeft;
    FileForm->Width = MainForm->Width;
    // FileForm->SpeedButton1->Left = FileForm->Panel2->Left+FileForm->Panel2->Width-FileForm->SpeedButton1->Width;
    // FileForm->SpeedButton1->Top = FileForm->Panel2->Top;

    // OnlineCMDForm->Top = nTmpBott;
    // OnlineCMDForm->Left = nTmpLeft;
    // OnlineCMDForm->Width = MainForm->Width;

    // OnlineCMDForm->Top = MainForm->Top;
    // OnlineCMDForm->Width = MainForm->Width;
    // OnlineCMDForm->Left = MainForm->Left-OnlineCMDForm->Width;

    MapMakerForm->Top = nTmpBott;
    MapMakerForm->Left = nTmpLeft;
	MapMakerForm->Width = MainForm->Width;
	MapMakerForm->Height = 560;

    IOTestForm->Top = nTmpBott;
    IOTestForm->Left = nTmpLeft;
    // IOTestForm->Width = MainForm->Width;
    IOTestForm->Width = MainForm->Width + StatusForm->Width;

    LongRunForm->Top = nTmpBott;
    LongRunForm->Left = nTmpLeft;

    AssistantForm->Top = nTmpBott + 1;
    AssistantForm->Left = nTmpLeft;
    AssistantForm->Width = MainForm->Width;
}

// ---------------------------------------------------------------------------
TPLAYBACK::TPLAYBACK()
{
    Init();
    tList_Communication = new TStringList;
    tList_HIDData = new TStringList;
    tList_Input1 = new TStringList;
	tList_Input2 = new TStringList;
    tList_Output1 = new TStringList;
    tList_Output2 = new TStringList;
    tList_Sum = new TStringList;

}

// ---------------------------------------------------------------------------
TPLAYBACK::~TPLAYBACK()
{
    delete tList_Communication;
    delete tList_HIDData;
    delete tList_Input1;
    delete tList_Input2;
    delete tList_Output1;
    delete tList_Output2;
    delete tList_Sum;
}

// ---------------------------------------------------------------------------
int TPLAYBACK::Init()
{
    nCount_TrackBar = 0;

    nCount_Communication = 0;
    nCount_HIDData = 0;
    nCount_Input1 = 0;
    nCount_Input2 = 0;
    nCount_Output1 = 0;
    nCount_Output2 = 0;
    nCount_Max = 0;

    return 0;

}
// ---------------------------------------------------------------------------

void TMainForm::CloseAllForm()
{
	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
		FoupOnlineCMDForm->Close();
	}
    // 2016-12-03, Add by Labeler
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
		EUVOnlineCMDForm->Close();
	}

	StatusForm->Close();
    CloseSubForm();
}
// ---------------------------------------------------------------------------

void TMainForm::CloseSubForm()
{
	ConnectForm->Close();
    FileForm->Close();
    LogFileForm->Close();
    MapMakerForm->Close();
    IOTestForm->Close();
	LongRunForm->Close();
    TeachingForm->Close();
    CommDisplayForm->Close();
    PIOForm->Close();
    SysInfoForm->Close();
    CmdWinForm->Close();
    E23PIOForm->Close();
	frm_Bay_Test->Close();
	OBS_UBG_Form->Close();
	DistanceSensorViewrForm->Close();
	UnitChecker->Close();
	frmDataTransperReq->Close();
}
// ---------------------------------------------------------------------------

void TMainForm::ShowMainMenu()
{
    mniFile->Visible = true;
    mniControl->Visible = true;
    mniInfomation->Visible = true;
}
// ---------------------------------------------------------------------------

void TMainForm::HideMainMenu()
{
    mniFile->Visible = false;
    mniControl->Visible = false;
    mniInfomation->Visible = false;
}
// ---------------------------------------------------------------------------

/**
 @brief   Class 생성 함수
 @author  puting
 @date    2012.12.17
 */
void TMainForm::ClassInit()
{
	gm_OHTStatus = new TOHTSTATUS;
	memset(&gm_OHTStatus->m_OHTStatus.AMCResponse, 0x00, sizeof(unsigned char));
	gm_OHTConnectInfo = new TOHTCONNECTINFO;

    TOHTLOGFILE *m_Dir = new TOHTLOGFILE;
    m_Dir->CheckDirectory();
    delete m_Dir;
}
// ---------------------------------------------------------------------------

/**
@brief   TP에 저장된 로그 파일 중 오래된 파일을 삭제하는 함수
@author  ks4423
@author  zzang9un
@date    2013.12.02
@date    2014.04.22
*/
void TMainForm::DeleteOldLogFile()
{
	FILE *fp;
	TIniFile *ini;

	int nDeleteTime = LOG_DELETE_DAY;

	AnsiString sExePath = ExtractFilePath(Application->ExeName);
	AnsiString sIniFilePath = sExePath + "\\DataFiles";
	AnsiString sLogFilePath = sExePath + "\\Log";

	TSearchRec sr;
	AnsiString strTmpFileName;

	__try
	{
		ini = new TIniFile(sIniFilePath + "\\Config.ini");
		nDeleteTime = ini->ReadInteger("Log", "Delete_Day", LOG_DELETE_DAY);

		if(FindFirst(sLogFilePath+"\\*.log", faArchive, sr) == 0)
		{
			do 
            {
                if((sr.Name == ".") || (sr.Name == "..")) continue;

                TDateTime Date1,Date2;
                AnsiString sDate1, sFileDate = sr.Name;

                int nPos;

                SYSTEMTIME CurTime;
                GetLocalTime(&CurTime);

                nPos = sFileDate.Pos(".log");
                sDate1.sprintf("%s-%s-%s 00:00:00", sFileDate.SubString(nPos-10, 4), sFileDate.SubString(nPos-10+4, 2), sFileDate.SubString(nPos-10+4+2, 2));

                Date1 = StrToDateTime(sDate1);
                Date2 = Now();

                int nDaysBetween = DaysBetween(Date1, Date2);

                if(nDeleteTime <= nDaysBetween)
                {
                	strTmpFileName = sLogFilePath + "\\" + sr.Name;
                	DeleteFile(strTmpFileName);
                }
			}while(FindNext(sr) == 0);
		}
	}
	__finally
	{
		FindClose(sr);
		delete ini;
	}
}
// ---------------------------------------------------------------------------

/**
 @brief   Class 해제 함수
 @author  puting
 @date    2012.12.17
 */
void TMainForm::ClassClose()
{
    delete gm_OHTStatus;
	delete gm_OHTConnectInfo;
}
// ---------------------------------------------------------------------------

/**
 @brief   Main Form에 있는 Statusbar를 panel로 사용하기 위해 설정
 @author  zzang9un
 @date    2013.03.26
 */
void TMainForm::SettingStatusBar()
{
    // Main Statusbar를 panel로 사용하기 위해 설정

    // -------------------------------------------
    // 통신 상태 표시 패널
    panCommStatus = new TPanel(StatusBarMain);
    panCommStatus->Parent = StatusBarMain;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panCommStatus->ParentBackground = false;

    panCommStatus->Visible = true;
    panCommStatus->Top = 2;
    panCommStatus->Height = StatusBarMain->Height - 2;
    panCommStatus->Width = StatusBarMain->Panels->Items[0]->Width;
    panCommStatus->BevelInner = bvNone;
    panCommStatus->BevelOuter = bvLowered;
    panCommStatus->Font->Color = COLOR_WHITE;
    panCommStatus->Color = COLOR_BLACK;
    panCommStatus->Caption = "OffLine";
    // -------------------------------------------

    // -------------------------------------------
    // User 상태 표시 패널
    panUser = new TPanel(StatusBarMain);
    panUser->Parent = StatusBarMain;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panUser->ParentBackground = false;

    panUser->Visible = true;
    panUser->Top = 2;
    panUser->Left = panCommStatus->Width;
    panUser->Height = StatusBarMain->Height - 2;
	panUser->Width = StatusBarMain->Panels->Items[1]->Width;
    panUser->BevelInner = bvNone;
    panUser->BevelOuter = bvLowered;
    panUser->Font->Color = COLOR_BLACK;
    panUser->Color = (TColor)0x00FCD2AD;
    panUser->Caption = "";
    // -------------------------------------------

    // -------------------------------------------
    // 송신 상태 표시 패널
    panSend = new TPanel(StatusBarMain);
    panSend->Parent = StatusBarMain;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panSend->ParentBackground = false;

    panSend->Visible = true;
    panSend->Top = 2;
    panSend->Left = panCommStatus->Width + panUser->Width;
    panSend->Height = StatusBarMain->Height - 2;
    panSend->Width = StatusBarMain->Panels->Items[2]->Width;
    panSend->BevelInner = bvNone;
    panSend->BevelOuter = bvLowered;
    panSend->Font->Color = COLOR_WHITE;
    panSend->Color = COLOR_FUCHSIA;
    panSend->Caption = "";
    // -------------------------------------------

    // -------------------------------------------
    // 수신 상태 표시 패널
    panReceive = new TPanel(StatusBarMain);
    panReceive->Parent = StatusBarMain;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panReceive->ParentBackground = false;

    panReceive->Visible = true;
    panReceive->Top = 2;
    panReceive->Left = panCommStatus->Width + panUser->Width + panSend->Width;
    panReceive->Height = StatusBarMain->Height - 2;
    panReceive->Width = StatusBarMain->Panels->Items[3]->Width;
    panReceive->BevelInner = bvNone;
    panReceive->BevelOuter = bvLowered;
    panReceive->Font->Color = COLOR_WHITE;
    panReceive->Color = COLOR_BLUE;
    panReceive->Caption = "";
    // -------------------------------------------

    // -------------------------------------------
    // Main 프로그램 버전 표시 패널
    panOHTVer = new TPanel(StatusBarVersionInfo);
    panOHTVer->Parent = StatusBarVersionInfo;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panOHTVer->ParentBackground = false;

    panOHTVer->Visible = true;
    panOHTVer->Top = 2;
    panOHTVer->Height = StatusBarVersionInfo->Height - 2;
    panOHTVer->Width = StatusBarVersionInfo->Panels->Items[0]->Width;
    panOHTVer->BevelInner = bvNone;
    panOHTVer->BevelOuter = bvLowered;
    panOHTVer->Font->Color = COLOR_BLACK;
    panOHTVer->Caption = "";
    // -------------------------------------------

    // -------------------------------------------
    // AMC 프로그램 버전 표시 패널
    panAMCVer = new TPanel(StatusBarVersionInfo);
    panAMCVer->Parent = StatusBarVersionInfo;

    // by zzang9un 2013.03.26 : panel 색상 표시를 위해 반드시 설정해야 함
    panAMCVer->ParentBackground = false;

    panAMCVer->Visible = true;
    panAMCVer->Top = 2;
    panAMCVer->Left = panOHTVer->Width;
    panAMCVer->Height = StatusBarVersionInfo->Height - 2;
    panAMCVer->Width = StatusBarVersionInfo->Panels->Items[1]->Width;
    panAMCVer->BevelInner = bvNone;
    panAMCVer->BevelOuter = bvLowered;
    panAMCVer->Font->Color = COLOR_BLACK;
    panAMCVer->Caption = "";
    // -------------------------------------------
}
// ---------------------------------------------------------------------------

/**
 @brief   Form 생성시 호출되는 이벤트 함수
 @author  zzang9un
 @date    2013.03.26
 */
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	// Statusbar Setting
    SettingStatusBar();

    strMyPath = ExtractFilePath(Application->ExeName);

    HideMainMenu();
    ClassInit();

	DeleteOldLogFile();
	mniConnect->Enabled = true;
	mniDisconnect->Enabled = false;


    MainForm->Left = 0;
    MainForm->Top = 0;

    // by zzang9un 2013.07.22 : Timer 활성화
    timerUpdateDisplay->Enabled = true;

	// Data Panel에 속한 각 panel의 크기 조절
    int width = (panDataStatus->Width - panFBWF->Width) / 2;

	panMap->Width = width *1.5;
    panLogDirve->Width = width *0.5;

    // by zzang9un 2013.04.23 : 무조건 개발자로 로그인되도록 임시 변경
	gm_OHTStatus->nProgram_Mode = MODE_SETUP;
	MainForm->panUser->Caption = "개발자";

}
// ---------------------------------------------------------------------------

/**
@brief   Connect 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniConnectClick(TObject *Sender)
{
	if(AssistantForm->Visible == true)
        AssistantForm->Close();

	ConnectForm->Show();
}

// ---------------------------------------------------------------------------
/**
 @brief   Form 종료시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.17
 */
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if(bExitFlag == false)
    {
        Action = caNone;
        return;
    }

    timerAllDisable();

    timerUpdateDisplay->Enabled = false;
    
    gm_OHTConnectInfo->RemoteFileSave(); // Remote File Save...
    SocketForm->DisConnect();

    // by zzang9un 2012.11.24 : 기존 SocketForm 사용을 위해 주석 처리함
    // OHTCommunication->ActiveOCS(false);

    FileForm->FileControlClose();
    LogFileForm->FileControlClose();
    ClassClose();
    // btnTimer->Enabled = false;
}

// ---------------------------------------------------------------------------
/**
 @brief   Disconnect 클릭시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.17
 */
void __fastcall TMainForm::mniDisconnectClick(TObject *Sender)
{
	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
    String strLog;
    strLog.sprintf(L"====> [DISCONNECT BUTTON] Click...");
    m_LogFile->WriteLogFile("Communication", strLog);
    delete m_LogFile;

	CommDisconnect();

	// 벨로시티 그래프 멈추도록 함
	VelGraphForm->Motor1Timer->Enabled = false;
	VelGraphForm->Motor24Timer->Enabled = false;
}

// ---------------------------------------------------------------------------
int TMainForm::CommDisconnect()
{
    SocketForm->DisConnect();
    panLineName->Caption = gm_OHTConnectInfo->g_strLineName;
    // by zzang9un 2012.11.24 : 기존 SocketForm 사용을 위해 주석 처리함
    // OHTCommunication->ActiveOCS(false);

    FileForm->FileControlClose();
    LogFileForm->FileControlClose();
	FoupOnlineCMDForm->Close();
    // 2016-12-03, Add by Labeler

	EUVOnlineCMDForm->Close();
//	ProbeJigOnlineCMDForm->Close();
    TeachingForm->Close();
	FirmwareUpdateForm->Close();
    HoistParamForm->Close();
	ErrorForm->Close();
	UnitChecker->Close();

    StatusBarMain->Panels->Items[1]->Text = "";
    ConnectForm->Show();
    HIDGraphForm->Timer1->Enabled = false;
    gm_OHTStatus->strOHTVer = "";
	gm_OHTStatus->nSTBDire = -1;
    gm_OHTStatus->nIsRotateExist = -1;
    //gm_OHTStatus->nIsRFPIORS23 = -1;
//    gm_OHTStatus->nIsRFPIO6Digit = -1;
    gm_OHTStatus->nIsRFPIOType = -1;
    Caption = g_Caption_Name + gm_OHTStatus->strOHTVer;
    StatusBarVersionInfo->Panels->Items[0]->Text = "";
    StatusForm->bReadFlg = false;
    APMonitoringForm->Close();
	frm_Bay_Test->Close();
	OBS_UBG_Form->Close();
    DistanceSensorViewrForm->Close();

	// shkim add
	// (1) ------------------ step 1. address delete ------------------
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OCS2OHT);
	int iReturn, iTemp;
	
	PKT_OCS2OHT SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH); 
	SEND_PACKET.TAG.Value = TAGCATEGORY::TAG_TP_ADDRESS_DELETE;
	
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);  // TP주소 delete 요청

	// (2) ------------------ step 2. port close -----------------
	tmrUdpListen->Enabled = false;
	frmUDP->pUdpObject->OHT_TP_Close();
	// -------
	
    return 0;
}

// ---------------------------------------------------------------------------
/**
 @brief   FileUpDownLoad 클릭시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.17
 */
void __fastcall TMainForm::mniFileUpDownloadClick(TObject *Sender)
{
	int nError;
    LogFileForm->FileControlClose();
    CloseSubForm();

	nError = FileForm->FileControlStart(SocketForm->m_strOHTIPAddress);
 

}
// ---------------------------------------------------------------------------

/**
 @brief   Form 종료 함수
 @author  zzang9un
 @date    2013.04.18
 */
void TMainForm::MainForm_Exist()
{
    SocketForm->DisConnect();
    FileForm->FileControlClose();
    LogFileForm->FileControlClose();
    ConnectForm->Close();

    bExitFlag = true;

    Close();
}

// ---------------------------------------------------------------------------
/**
 @brief   종료버튼 클릭시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.17
 */
void __fastcall TMainForm::mniYESClick(TObject *Sender)
{
    MainForm_Exist();
}

// ---------------------------------------------------------------------------
void __fastcall TMainForm::mniCommunicationClick(TObject *Sender)
{
    CommDisplayForm->Show();
}

// ---------------------------------------------------------------------------
/**
 @brief   Map_Maker 클릭시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.17
 */

void __fastcall TMainForm::pmnuMemoClearClick(TObject *Sender)
{
    memoMain->Clear();
}
// ---------------------------------------------------------------------------

/**
@brief   IO Map 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniIOMapClick(TObject *Sender)
{
	if(SocketForm->fIsIOMapLoaded == false)
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;		
		m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_DATA_IOMAP);
		delete m_CMDControl;
	}
	else
		IOMapForm->Show();

/*
	if(SocketForm->fIsIOMapLoaded == false)
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_DATA_IOMAP);
		delete m_CMDControl;
	}
	else
		IOMapForm->Show();
*/
}
// ---------------------------------------------------------------------------

void TMainForm::DataDisplay(TColor TmpFontColor, String strDisplayData)
{
    if(memoMain->Lines->Count > 500)
        memoMain->Clear();

    CurrText()->Color = TmpFontColor;

    String strTime = TimeToStr(Now());
    memoMain->Lines->Add("[" + strTime + "]" + strDisplayData);
}
// ---------------------------------------------------------------------------

/**
@brief   Connect 실패시 호출되는 Timer 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::timerCommCheckTimer(TObject *Sender)
{
/*
    struct time currenttime;
    static int FailCount = 0;
    String strData;

    if(SocketForm->bConnectedOHT == true)
    {
        if(SocketForm->OHTSocket->Active == false)
            SocketForm->OHTSocket->Active = true;

        CommFail();

        FailCount++;

        if(FailCount <= 5)
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_STATUS);
            gm_OHTStatus->bReceiveStatus = false;
			delete m_CMDControl;

            gettime(&currenttime);
            strData.sprintf(L"%02d:%02d:%02d.%02d ReSend #%d Status...", currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec,
                currenttime.ti_hund, FailCount);
            memoMain->Lines->Add(strData);
        }
        else
        {
            timerCommCheck->Enabled = false;
            FailCount = 0;
            SocketForm->bConnectedOHT == false;

            gettime(&currenttime);
            strData.sprintf(L"%02d:%02d:%02d.%02d Disconnect...", currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec,
                currenttime.ti_hund);
            memoMain->Lines->Add(strData);
        }
	}
*/
}

// ---------------------------------------------------------------------------

/**
@brief   Status Timer에 의해 주기적으로 호출되는 함수

         OHT에게 Status를 요청하는 Command를 전송하는 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::timerStatusRequestTimer(TObject *Sender)
{
#if 1


	timerCommCheck->Enabled = false;

	// OHT로 상태요청 값을 보내보자
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_ESTOP);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_STATUS_REQ);

	delete m_CMDControl;

	timerCommCheck->Enabled = true;


	// shkim. Status값을 이용한 연결 끊어짐 감지
	static int iErrorCount = 0;
	static int iBeforeProgress = 0;
	static int iNowProgress = 0;

	iNowProgress = pbCommStatus->Position;	// 프로그래스 바의 현재값을 확인

	if(iBeforeProgress != iNowProgress)		// 이전값과 현재값이 다르면 통신중임
	{
		iBeforeProgress = iNowProgress;		// 다음턴에 사용하도록 저장
		iErrorCount = 0;
	}
	else
	{
		iErrorCount++;
	}

	if(iErrorCount > 20)	// 0.2초 x 20 = 4초 동안 Status가 안오면 끊어졌다고 본다
	{
		// 벨로시티 그래프 멈추도록 함
		VelGraphForm->Motor1Timer->Enabled = false;
		VelGraphForm->Motor24Timer->Enabled = false;

		memoMain->Lines->Add(L"통신 끊어짐 감지함.");
		iErrorCount = 0;

		mniDisconnectClick(NULL);
		ShowMessage(L"OHT가 응답이 없어 통신을 종료합니다!");
	}
#else
	timerStatusRequest->Enabled = false;
	ShowMessage("시험용 버전이므로 상태 읽어오기 기능이 꺼져 있습니다");
#endif

#if 1
	FirmwareUpdateForm->btnUpdate->Enabled = gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_AUTO;
#else
 	ShowMessage("검증 모드이므로 Firmware Update 버튼 인터락이 해제되어 있습니다");
#endif

	static bool bPrevBtnStat = true;
	static DWORD dwStartTick = 0;
	AnsiString autoBtnTxt = "Auto";

	// Auto로 변환되는 중
	bool bAutoChanging = gm_OHTStatus->m_OHTStatus.DataLoaded.Info.AutoChanging == 1;
	btnAuto->Enabled = !bAutoChanging;

	// SHCHO 2018.10.10
	// Auto로 변환되는 중에 사용자에게 UI적으로 인폼하기 위해서
	// 변경되는 동안 .uto, A.to, Aut. 같은 방식으로 표시해준다.
	if(bAutoChanging)
	{
		if(bPrevBtnStat != btnAuto->Enabled)
			dwStartTick = GetTickCount();

		int nSec = (GetTickCount() - dwStartTick)/1000;
		int nDotPos = nSec % (autoBtnTxt.Length()+1);
		autoBtnTxt.Insert(".", nDotPos);
		autoBtnTxt.Delete(nDotPos+1, 1);
	}

	if(btnAuto->Caption != autoBtnTxt)
		btnAuto->Caption = autoBtnTxt;

	bPrevBtnStat = btnAuto->Enabled;

	// TP 다중 접속 차단 부분*******
	//
	static bool bCritical = true;   // timer는 계속 돌기 때문에 메시지 박스 무한으로 뜨는것 방지

	if(!bCritical)
		return;

	// Main에서 중복접속되었다고 알려주면, 처리하는 로직
	bCritical = false;
	bool bJungbok = gm_OHTStatus->m_OHTStatus.DataLoaded.Info.MultiConnTP == 1;
	if(bJungbok && !m_bIgnoreJungBokConnect)
	{
		m_bIgnoreJungBokConnect = true;  // 다시 묻지 않도록 Ignore 플래그 On

		// 매뉴얼로 동작중이면 강제 종료, 그게 아니면 접속을 끊을지 물어봄
		if(IsManualWorking())
		{
			bool bSkip = GetAsyncKeyState(VK_CONTROL);
			if(bSkip)
			{
				if(Application->MessageBox(L"다른 PC에서 TP에서 메뉴얼 동작 중입니다.\n계속 하시겠습니까?", L"경고",  MB_YESNO) != IDYES)
					mniDisconnectClick(NULL);
			}
			else
			{	Application->MessageBox(L"다른 PC에서 TP접속중입니다.\n현재 Manual 동작 중이기 때문에 접속을 강제 종료 합니다.", L"경고",  MB_OK);
				mniDisconnectClick(NULL);
			}
		}
		else
		{
			bool bDisconnect = Application->MessageBox(L"다른 PC에서 TP 접속중입니다.\n접속 끊는 것을 권장합니다.", L"경고",  MB_YESNO) == ID_YES;
			if(bDisconnect)
				mniDisconnectClick(NULL);


		}
	}

	bCritical = true;

	// 중복접식일 경우 LineName 라벨 깜박거리면서 사용자에게 경고함
	if(bJungbok)
	{
		static bool bPrevStat = false;
		static bool bToggle = false;
		static DWORD dwBlinkTick = 0;
		if(bJungbok != bPrevStat)
		{
			bPrevStat = bJungbok;
			dwBlinkTick = GetTickCount();
		}

		if(GetTickCount() - dwBlinkTick > 1000)	// 1000 msec
		{
			dwBlinkTick = GetTickCount();
			bToggle = !bToggle;
		}
		panLineName->Caption = bToggle ? String(L"중복접속!!!") : String(L"Check!");

	}
	else
		panLineName->Caption = gm_OHTConnectInfo->g_strLineName;
	// ************


}
// ---------------------------------------------------------------------------

/**
@brief   Estop버튼 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::btnEStopClick(TObject *Sender)
{
    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_ESTOP);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_ESTOP);

    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

/**
@brief   Manual버튼 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::btnManualClick(TObject *Sender)
{
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_MANUALMODE);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_MANUALMODE);

    delete m_CMDControl;

}
// ---------------------------------------------------------------------------

/**
@brief   Auto버튼 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::btnAutoClick(TObject *Sender)
{
	M2AForm->ShowModal();
}
// ---------------------------------------------------------------------------

/**
@brief   Reset버튼 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::btnResetClick(TObject *Sender)
{
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_RESET);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_RESET);

    panDataStatus->Caption = "";
	delete m_CMDControl;


}
// ---------------------------------------------------------------------------

/**
@brief   OnlineControl 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniOnlineControlClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
		FoupOnlineCMDForm->Show();
    }
    // 2016-12-03, Add by Labeler
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
    {
		EUVOnlineCMDForm->Show();
	}
}
// ---------------------------------------------------------------------------

void TMainForm::CommOK()
{
    pbCommStatus->Visible = true;
	panCommFail->Color = COLOR_LIME;
	panVer->Color = COLOR_YELLOW;
	panVer->Font->Size = 15;
	lblCommFail->Caption = "Good";
	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
		FoupOnlineCMDForm->panVer->Color = COLOR_YELLOW;
	}
    // 2016-12-03, Add by Labeler
    else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
    {
		EUVOnlineCMDForm->panVer->Color = COLOR_YELLOW;
    }


}
// ---------------------------------------------------------------------------
void TMainForm::CommFail()
{
	pbCommStatus->Visible = false;
	panCommFail->Color = COLOR_RED;
	panVer->Color = COLOR_RED;
	lblCommFail->Caption = "Bad";
	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
		FoupOnlineCMDForm->panVer->Color = COLOR_RED;
	}
    // 2016-12-03, Add by Labeler
    else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
    {
		EUVOnlineCMDForm->panVer->Color = COLOR_RED;
    }


}
// ---------------------------------------------------------------------------

/**
@brief   PIODisplay 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniPIODisplayClick(TObject *Sender)
{
	if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E23)
		E23PIOForm->Show();
	else
		PIOForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniETCClick(TObject *Sender)
{
    // HelpViewForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniOHTSysInfoClick(TObject *Sender)
{
	uTP_CMD *data = new uTP_CMD;
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_ADDITIONAL_STATUS, data);
	m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_TIMESETTING, data);
	delete m_CMDControl;
	delete data;

    SysInfoForm->Show();
}
// ---------------------------------------------------------------------------

void TMainForm::Warnning_Display(TColor tWColor, String strData)
{
    DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   HID Graph 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniHIDClick(TObject *Sender)
{
    HIDGraphForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   IO Control 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniIOControlClick(TObject *Sender)
{
    CloseSubForm();
    IOTestForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   Vel Gragh 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniVelGraphClick(TObject *Sender)
{
    VelGraphForm->Show();
}
// ---------------------------------------------------------------------------


void __fastcall TMainForm::StatusBarMainDblClick(TObject *Sender)
{
    if(panCommStatus->Caption == "OffLine")
        UserSelectForm->ShowModal();
}
// ---------------------------------------------------------------------------

/**
@brief   TaachingWindow 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniTeachingWindowClick(TObject *Sender)
{
    CloseSubForm();
    TeachingForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::Cancel1Click(TObject *Sender)
{
    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_LOGSAVE_DISABLE);
    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

String TMainForm::Get_FileAccessTime(String strFileName)
{
    struct _wffblk ffblk;
    int done;
    TDateTime LastDate, LastTime, CurrentDate;
    int Year, Month, Day, Hour, Min, Sec;
    char FileName[1024];
    String strFName, strTmpFullName;

    CurrentDate = Now();

    strFName = strFileName;

    done = _wfindfirst(strFName.w_str(), &ffblk, 0);
    if(!done)
    {
        Year = 1980 + ((ffblk.ff_fdate >> 9) & 127);
        Month = (ffblk.ff_fdate >> 5) & 15;
        Day = (ffblk.ff_fdate) & 31;
        Hour = (ffblk.ff_ftime >> 11) & 31;
        Min = (ffblk.ff_ftime >> 5) & 63;
        // Sec = 2*((ffblk.ff_ftime)&31);
        strTmpFullName.sprintf(L"%04d%02d%02d%02d%02d", Year, Month, Day, Hour, Min);
    }
    else
        strTmpFullName.sprintf(L"%04d%02d%02d%02d%02d", 0, 0, 0, 0, 0);

    return strTmpFullName;
}
// ---------------------------------------------------------------------------
void __fastcall TMainForm::NodeDistOffsetclear1Click(TObject *Sender)
{
    String strTmpMessage;

    // puting 2012.12.03 구조체 초기화
    GO_CMD SndGoCmd;
    memset(&SndGoCmd, 0x00, sizeof(GO_CMD));

    strTmpMessage = "Node Distance Offset Clear ... OK?";

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL)
    {
        if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->SendGoCmd2OHT(TPCMD_OHT_NODEDIST_CLEAR, &SndGoCmd);
            delete m_CMDControl;

            MainForm->DataDisplay(COLOR_BLACK, "Node Distance Offset Clear");
        }
    }
    else
        MainForm->DataDisplay(COLOR_RED, "OHT Mode == AutoMode");
}
// ---------------------------------------------------------------------------

/**
@brief   Unit Conversion 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniUnitConvertClick(TObject *Sender)
{
    ConversionForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   AMCParameter 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniAMCParameterClick(TObject *Sender)
{
    // AMCParaForm = new TAMCParaForm(Application);
    AMCParaForm->Show();
    // delete AMCParaForm;
}
// ---------------------------------------------------------------------------

/**
@brief   LogDownLoad 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniLogDownloadClick(TObject *Sender)
{
	int nError;
    FileForm->FileControlClose();
    CloseSubForm();

    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
	// m_CMDControl->MakeCommand2OHT(TPCMD_OHT_OHTINIT);
    delete m_CMDControl;

    nError = LogFileForm->FileControlStart(SocketForm->m_strOHTIPAddress);



}
// ---------------------------------------------------------------------------
/**
@brief   HoistParam 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniHoistParameterClick(TObject *Sender)
{
    HoistParamForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   LogRun 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniLogRunClick(TObject *Sender)
{
    CloseSubForm();
    LongRunForm->Show();
}
// ---------------------------------------------------------------------------


/**
@brief   Manual버튼 클릭시 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::btnPause_SetResetClick(TObject *Sender)
{
	if(btnPause_SetReset->Caption == "Pause:Set")	// 현재 일반 상태. Pause 걸수 있음
	{
		if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_AUTO)
		{
			ShowMessage("Pause 기능은 AUTO 모드에서만 사용 가능합니다");
			return;
		}

		if (gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec != 0)	// OHT가 이동 중일 때
		{
			if(ID_YES != MessageBox(this->Handle, L"OHT가 이동 중입니다.\r\n 정말 Pause하시겠습니까?", L"경고",  MB_YESNO))
				return;		// 사용자가 NO를 선택하면 빠져 나간다
		}

		// 실제 Pause를 거는 부분
		mnuOCSPauseClick(NULL);
	}
	else if(btnPause_SetReset->Caption == "Pause:Reset")	// 현재 Pause상태
	{
		mnuOCSResumeClick(NULL);
    }

/*
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
	if(btnPause_SetReset->Caption == "Pause:Set")
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_FC_PAUSE_SET);
		Sleep(500);

		if(gm_OHTStatus->m_OHTStatus.IsFCPause == STATUS_OHT_PAUSED)
		{
			btnPause_SetReset->Caption = "Pause:Reset";
			DataDisplay(COLOR_BLACK, "OHT_FC_PAUSE_SET");
		}
		else
		{
			DataDisplay(COLOR_BLACK, "Unable to pause because OHT is moving");
		}
	}
	else if(btnPause_SetReset->Caption == "Pause:Reset")
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_FC_PAUSE_RESET);
		btnPause_SetReset->Caption = "Pause:Set";
		DataDisplay(COLOR_BLACK, "OHT_FC_PAUSE_RESET");
	}
    delete m_CMDControl;
*/
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniAMCDebugClick(TObject *Sender)
{
    AmcDebugForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniAPSignalDisplayClick(TObject *Sender)
{
    APMonitoringForm->Show();
}
// ---------------------------------------------------------------------------

/**
@brief   Error 조회 호출되는 이벤트 함수
@author  puting
@date    2012.12.17
*/
void __fastcall TMainForm::mniErrorSearchClick(TObject *Sender)
{
	ErrorSearchForm->ShowModal();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniAboutClick(TObject *Sender)
{
    AboutBox->ShowModal();
}
// ---------------------------------------------------------------------------

void __fastcall TMainForm::mniUserModeChangeClick(TObject *Sender)
{
    UserSelectForm->ShowModal();
}
// ---------------------------------------------------------------------------

/**
@brief   MainForm의 UI를 주기적으로 업데이트 하는 함수
@author  zzang9un
@date    2013.07.22
@todo    Data외에 Main Form UI를 처리하도록 수정 필요
*/
void __fastcall TMainForm::timerUpdateDisplayTimer(TObject *Sender)
{
    // 처음 켤때 실제 null 상태 발생함
	if(SocketForm == NULL)
	{
		panMap->Color = COLOR_SILVER;
		return;
	}

	// OHT가 연결된 경우에만 Display 업데이트
	if(SocketForm->bConnectedOHT == false)
	{	panMap->Color = COLOR_SILVER;

		if(ConnectForm->Visible == false)
			ConnectForm->Show();

		return;
	}

	// Map Load 여부 표시
	if(gm_OHTStatus->m_OHTStatus.DataLoaded.Info.Map)
		panMap->Color = COLOR_LIME;
	else
		panMap->Color = COLOR_SILVER;


	// Log 드라이브 유무 표시
	if(gm_OHTStatus->m_OHTStatus.isLogDrive)
		panLogDirve->Color = COLOR_LIME;
	else
		panLogDirve->Color = COLOR_RED;

	//puting AMC Update 유무

	if(gm_OHTStatus->m_OHTStatus.AMCResponse==0x01)  //진행중 or 대기중
		WaitForm->DisplayMemo->Color = clSilver;
	else if(gm_OHTStatus->m_OHTStatus.AMCResponse==0x02) //완료
	{

		memoMain->Lines->Add("Amc.out Complete....");

		uTP_CMD TPCmd;
		memset(&TPCmd, 0x00, sizeof(uTP_CMD));
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_MAINPROG_CLOSE, &TPCmd);
		MainForm->DataDisplay(COLOR_BLACK, "OHT Main Program Close");
		delete m_CMDControl;

		gm_OHTStatus->m_OHTStatus.AMCResponse =0x00;

		WaitForm->Close();
	}
	else if(gm_OHTStatus->m_OHTStatus.AMCResponse==0x03) //에러
	{
		memoMain->Lines->Add("Amc.out fail....");

		uTP_CMD TPCmd;
		memset(&TPCmd, 0x00, sizeof(uTP_CMD));
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_MAINPROG_CLOSE, &TPCmd);
		MainForm->DataDisplay(COLOR_BLACK, "OHT Main Program Close");
		delete m_CMDControl;

		gm_OHTStatus->m_OHTStatus.AMCResponse =0x00;

		WaitForm->Close();
	}

}
//---------------------------------------------------------------------------

/**
@brief   MainForm의 Timer를 초기화 하는 함수
@author  zzang9un
@date    2013.11.19
*/
void TMainForm::timerInit()
{
    timerCommCheck->Interval = INTERVAL_COMM_CHECK;
    timerCommCheck->Enabled = false;

    timerStatusRequest->Interval = INTERVAL_STATUS_REQUEST;
    timerStatusRequest->Enabled = false;

    timerUpdateDisplay->Interval = INTERVAL_UPDATE_DISPLAY;
    timerUpdateDisplay->Enabled = false;
}
//---------------------------------------------------------------------------

/**
@brief   MainForm의 Timer를 Disable하는 함수
@author  zzang9un
@date    2013.11.19
@warning UI를 업데이트 하는 timer는 제외한다.
*/
void TMainForm::timerAllDisable()
{
    timerCommCheck->Enabled = false;
    timerStatusRequest->Enabled = false;
}
//---------------------------------------------------------------------------

/**
@brief   MainForm의 Timer를 Enable하는 함수
@author  zzang9un
@date    2013.11.19
@warning UI를 업데이트 하는 timer는 제외한다.
*/
void TMainForm::timerAllEnable()
{
    timerCommCheck->Enabled = true;
	timerStatusRequest->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Optionconfig1Click(TObject *Sender)
{
	OptionForm->Show();
}
//---------------------------------------------------------------------------

/**
@brief	Assistant 메뉴를 눌렀을때 폼을 띄움
@author  doori.shin
@date    	2013.12.31
@remark	Assistant와 네트워크 드라이브연결을 시도함
*/
void __fastcall TMainForm::mniOHTAssistantClick(TObject *Sender)
{
#if 0
	if(SocketForm->AssistantSocket->Active == true)
	{
        // 연결되어있을시 폼을 띄운다
		AssistantForm->Show();
	}
	else
	{
        // Disconnect 상황이면 연결을 시도함
		int msg = MessageDlg("Assistant Socket cannot available...\n Would you retry to connect?", mtWarning, mbYesNo, 0);

		if(msg == mrYes)
		{
			int nError = 0;

			MainForm->mniNetworkConnect->Enabled    = true;
			MainForm->mniNetworkDisconnect->Enabled = false;

			SocketForm->AssistantConnect(SocketForm->m_strOHTIPAddress.Trim(), SocketForm->m_strOHTName.Trim());

			nError = FileForm->FileControlStart(SocketForm->m_strOHTIPAddress.Trim());
			if(nError != 0)
			{
				Warnning_Display(COLOR_RED, "ERROR : Network Connect Error..");
				MainForm->mniNetworkConnect->Enabled    = true;
				MainForm->mniNetworkDisconnect->Enabled = false;
			}
			else
			{
				MainForm->mniNetworkConnect->Enabled    = false;
				MainForm->mniNetworkDisconnect->Enabled = true;
			}
		}
	}
#endif

	// 위 코드는 이상한 부분이 있는것은 아님. 단지 사용자가 보기에 오류로 보일수 있으므로 
	// 접속안된 상태이면 바로 접속하도록 해줌
	if(SocketForm->AssistantSocket->Active == true)
	{
		// 연결되어있을시 폼을 띄운다
		AssistantForm->Show();
	}
	else
	{
		int nError = 0;
		

		
		SocketForm->AssistantConnect(SocketForm->m_strOHTIPAddress.Trim(), SocketForm->m_strOHTName.Trim());
		
		nError = FileForm->FileControlStart(SocketForm->m_strOHTIPAddress.Trim());


		AssistantForm->Show();
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::timerAssistantTimer(TObject *Sender)
{
	uTP_CMD TPCmd;
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_ADDITIONAL_STATUS, &TPCmd);

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::trackbarChange(TObject *Sender)
{
	timerAssistant->Interval = trackbar->Position * 1000;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::mnuUDPDebugVisibleYesClick(TObject *Sender)
{
	frmUDP->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuUDPDebugVisibleNoClick(TObject *Sender)
{
	frmUDP->Visible = false;
}
//---------------------------------------------------------------------------

void TMainForm::StandardRead(PKT_OHT2OCS Pkt, int iMachineType)
{
	AnsiString strTmp;
	strTmp.sprintf("OHT 패킷 수신, 명령타입 : %d  머신타입 : %d" , Pkt.TAG.Value, iMachineType);
    ShowMessage(strTmp);
}


void __fastcall TMainForm::tmrUdpListenTimer(TObject *Sender)
{
	frmUDP->pUdpObject->OHT_TP_ThreadCall();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormActivate(TObject *Sender)
{
	//static bool bShowOnce = false;
//#ifdef	_ONE_PC_TEST
//	if (bShowOnce == false)
//	{
//		ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
//		bShowOnce = true;
//	}
// #endif
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::mnFirmwareUpdateClick(TObject *Sender)
{
	FirmwareUpdateForm->Show();

	FirmwareUpdateForm->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuOCSPauseClick(TObject *Sender)
{
	// 인터락 검사
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_AUTO)
	{
		ShowMessage("이 기능은 AUTO 모드에서만 사용 가능합니다");
		return;
    }


	// Pause, Resume 변환
	mnuOCSPause->Enabled = false;
	mnuOCSResume->Enabled = true;

	// Command 전송
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	SEND_PACKET.TAG.Value = TagID::CMD_TP_OCS_PAUSE;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuOCSResumeClick(TObject *Sender)
{

	// Pause, Resume 변환
	mnuOCSPause->Enabled = true;
	mnuOCSResume->Enabled = false;


	// Command 전송
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	SEND_PACKET.TAG.Value = TagID::CMD_TP_OCS_RESUME;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuTpPauseClick(TObject *Sender)
{
	// 인터락 검사
	if(
		(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
		&& (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_GOING)
		&& (gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec == 0)
	  )
	  {
		 // 이 기능은 Auto, Going이면서 주행중이 아닐때 (속도 0) 만 사용가능.
		 // 이 조건을 만족하면 다음 메뉴로 진행
	  }
	else
	 {
		ShowMessage("이 기능은 Auto, Going이면서 주행중이 아닐때 (속도 0) 만 사용가능합니다");
		return;
	 }

	// Pause, Resume 변환
	mnuTpPause->Enabled = false;
	mnuTpResume->Enabled = true;

	// Command 전송
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	SEND_PACKET.TAG.Value = TagID::CMD_TP_TP_PAUSE;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;


}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuTpResumeClick(TObject *Sender)
{
	// Pause, Resume 변환
	mnuTpPause->Enabled = true;
	mnuTpResume->Enabled = false;

	// Command 전송
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	SEND_PACKET.TAG.Value = TagID::CMD_TP_TP_RESUME;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;



}
//---------------------------------------------------------------------------



void __fastcall TMainForm::BayMovingTransTeest1Click(TObject *Sender)
{
    frm_Bay_Test->Show();
    frm_Bay_Test->BringToFront();
}
//---------------------------------------------------------------------------

// 현재 OHT가 Manual 동작중인지 판단
bool TMainForm::IsManualWorking()
{
	BYTE stat = gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status;
	BYTE mode = gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode;

	bool bWorking = (stat == STS_GOING) || (stat == STS_UNLOADING) || (stat == STS_LOADING)|| (stat == STS_RUNNING);	//(stat == STS_GOING || stat == STS_UNLOADING || stat == STS_LOADING || stat == STS_RUNNING);
	return bWorking && mode == MODE_MANUAL;
}

void __fastcall TMainForm::mnOHT_Obstacle_SensorSettingClick(TObject *Sender)
{
	OBS_UBG_Form->Show();
    OBS_UBG_Form->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnDistanceSensorViewerClick(TObject *Sender)
{
	DistanceSensorViewrForm->Show();
	DistanceSensorViewrForm->BringToFront();
}
//---------------------------------------------------------------------------
void TMainForm::SetMapVersion(String MapVer)
{
	frmDataTransperReq->lblMapVersion->Caption = MapVer;
}
void __fastcall TMainForm::mniDataTransper1Click(TObject *Sender)
{
	frmDataTransperReq->Show();
	frmDataTransperReq->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniCIDChecker2Click(TObject *Sender)
{
	frmCidChecker->Show();
	frmCidChecker->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniHIDLOGSaveClick(TObject *Sender)
{
	frmHidLogGet->Show();

	frmHidLogGet->BringToFront();
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::mniHIDVelGraphcts1Click(TObject *Sender)
{
	HIDVelForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mniHIDVelGraphsec1Click(TObject *Sender)
{
	HIDVelSecForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::mniDrivingMapMake1Click(TObject *Sender)
{
	CloseSubForm();
	MapMakerForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::mniSterringCheckClick(TObject *Sender)
{
	UnitChecker->Show();
	UnitChecker->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuTpAVSViewerClick(TObject *Sender)
{
	frmVibrationMeasurement->Show();
}
//---------------------------------------------------------------------------

