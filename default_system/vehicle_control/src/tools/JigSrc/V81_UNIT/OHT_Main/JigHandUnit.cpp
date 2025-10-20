//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JigHandUnit.h"
#include "OHTMain.h"
#include "TaskControl.h"
#include "MyTools.h"
#include "Utility.h"
#include "PUtily.h"
#include "JigThread.h"
#include <math.h>

#define COLOR_AQUA          clAqua
#define COLOR_BLACK         clBlack
#define COLOR_BLUE          clBlue
#define COLOR_CREAM         clCream
#define COLOR_DARKGRAY      clDkGray
#define COLOR_FUCHSIA       clFuchsia
#define COLOR_GRAY          clGray
#define COLOR_GREEN         clGreen
#define COLOR_LIME          clLime
#define COLOR_LIGHTGRAY     clLtGray
#define COLOR_MAROON        clMaroon
#define COLOR_MEDIUMGRAY    clMedGray
#define COLOR_MINTGREEN     clMoneyGreen
#define COLOR_NAVYBLUE      clNavy
#define COLOR_OLIVE         clOlive
#define COLOR_PURPLE        clPurple
#define COLOR_RED           clRed
#define COLOR_SILVER        clSilver
#define COLOR_SKYBLUE       clSkyBlue
#define COLOR_WHITE         clWhite
#define COLOR_YELLOW        clYellow

#define LOG_JIG(...)   WRITE_LOG_UNIT(pLogJig, __VA_ARGS__)

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

THandUnit *HandUnit;
JigThread *JigCtrl = NULL;
TaskControl *gSeq = NULL;

//---------------------------------------------------------------------------
__fastcall THandUnit::THandUnit(TComponent* Owner)
    : TForm(Owner)
{
	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pCmdManager = OHTMainForm->GetCmdManager();

	gSeq = NULL;
	JigCtrl = NULL;
	Init();

	JigCtrl->m_StepJig_Hand.bSignal = true;  //로테이트 시그널 On

}

//---------------------------------------------------------------------------
void  THandUnit::Init()
{
	gSeq = OHTMainForm->GetTaskControl();
	JigCtrl = OHTMainForm->GetJigCtrl();

}
//---------------------------------------------------------------------------


void __fastcall THandUnit::btnHandInitClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hand.Step == 0 && JigCtrl->m_StepJig_Hand.bSignal == true)

		JigCtrl->m_StepJig_Hand.Step = 1;

}
//---------------------------------------------------------------------------

void __fastcall THandUnit::btnHand_ResetClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hand.bSignal == false)
	{
		JigCtrl->m_StepJig_Hand.bSignal = true;
		JigCtrl->m_StepJig_Hand.Step = 2;
		JigCtrl->m_StepJig_Hand.StepComment = "Thread Reset";
		edt_HandErrMsg->Text = edt_HandErrMsg->Text.sprintf(L"");

//		GetLocalTime(&CurTime);
//		edtStartDate->Text ="0";
//		edtEndDate->Text ="0";

	}
}
//---------------------------------------------------------------------------

void __fastcall THandUnit::btn_HandEStopClick(TObject *Sender)
{
//	UpdateParam(true);
//	SaveParam();

	if(JigCtrl->m_StepJig_Hand.Step>=100 && JigCtrl->m_StepJig_Hand.Step<200)
	{
		GetLocalTime(&CurTime);
		edt_HandErrTime->Text = edt_HandErrTime->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	}

	JigCtrl->m_StepJig_Hand.bSignal = false;
	JigCtrl->m_StepJig_Hand.Step = 200;
	JigCtrl->m_StepJig_Hand.StepComment = "Emergency Stop";
}
//---------------------------------------------------------------------------
void __fastcall THandUnit::btnHand_RunClick(TObject *Sender)
{
//	UpdateParam(false);
	LoadParam();

	if(JigCtrl->m_StepJig_Hand.Step != 0) // Init 상태 아니면 못하게 막음
	{
		Application->MessageBox(L"Hand Step isn't Initialized...", L"Error", MB_OK + MB_ICONSTOP);
		return;
	}

	if((gSeq->m_pHWSet-> m_pHand->IsClose())&&(gSeq->m_pHWSet->m_pHand->IsInposition()))   // Close하고 인포지션 들어와야 시작
	{
		JigCtrl->m_StepJig_Hand.Step = 100;
		GetLocalTime(&CurTime);
		edt_HandDate->Text = edt_HandDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
		edt_HandErrMsg->Text = edt_HandErrMsg->Text.sprintf(L"");
	}
	else
	{
		edt_HandErrMsg->Text = edt_HandErrMsg->Text.sprintf(L"Hand isn't Origin");
	}
}
//---------------------------------------------------------------------------


void __fastcall THandUnit::btnOut_HandOpenClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hand.Step == 0 && JigCtrl->m_StepJig_Hand.bSignal == true)
		JigCtrl->m_StepJig_Hand.Step = 4;
}
//---------------------------------------------------------------------------

void __fastcall THandUnit::btnOut_HandCloseClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hand.Step == 0 && JigCtrl->m_StepJig_Hand.bSignal == true)
		JigCtrl->m_StepJig_Hand.Step = 5;
}
//---------------------------------------------------------------------------

void __fastcall THandUnit::btnOut_AlarmResetClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hand.bSignal == true)
		JigCtrl->m_StepJig_Hand.Step = 6 ;
}
//---------------------------------------------------------------------------

void __fastcall THandUnit::btnHand_LoadParamClick(TObject *Sender)
{
	LoadParam();
	UpdateParam(false);
}
//---------------------------------------------------------------------------

void THandUnit::LoadParam()
{

    LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

    pIniFile->ReadInteger("Setting", "Hand_Interval_ms", 0, &JigCtrl->m_JigParam.nHand_Interval_ms);
	pIniFile->ReadInteger("Setting", "Hand_CycleCnt", 0, &JigCtrl->m_JigParam.nHand_CycleCnt);
    pIniFile->ReadInteger("Setting", "Hand_LastCnt", 0, &JigCtrl->m_JigParam.nHand_CurrentCnt);
//    UpdateParam(false);

	delete pIniFile;

}
//---------------------------------------------------------------------------
void THandUnit::SaveParam()
{
    LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->WriteInteger("Setting", "Hand_Interval_ms", JigCtrl->m_JigParam.nHand_Interval_ms);
	pIniFile->WriteInteger("Setting", "Hand_CycleCnt", JigCtrl->m_JigParam.nHand_CycleCnt);
	pIniFile->WriteInteger("Setting", "Hand_LastCnt",  JigCtrl->m_JigParam.nHand_CurrentCnt);
	delete pIniFile;

}
//---------------------------------------------------------------------------
void THandUnit::UpdateParam(bool bUI_toParam)
{
// UI --> 파일로 업데이트
	if(bUI_toParam)
	{
//		JigCtrl->m_JigParam.nHand_Interval_ms = StrToInt(edtHand_Interval->Text.c_str());
//		JigCtrl->m_JigParam.nHand_CycleCnt = StrToInt(edtHand_Count->Text.c_str());
//		JigCtrl->m_JigParam.nHand_CurrentCnt = StrToInt(edtHand_Current_Count->Text.c_str());


		SaveParam();
	}
	// Parameter -> UI로 업데이트
	else
	{
		edtHand_Interval->Text = edtHand_Interval->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHand_Interval_ms);
		edtHand_Count->Text = edtHand_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHand_CycleCnt);
		edtHand_Current_Count->Text = edtHand_Current_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHand_CurrentCnt);
	}


}

void __fastcall THandUnit::chkHandLongrunClick(TObject *Sender)
{
    if(chkHandLongrun->Checked == true)
        JigCtrl->m_StepJig_Hand.bLongrun = true;
    else
		JigCtrl->m_StepJig_Hand.bLongrun = false;

}
//---------------------------------------------------------------------------


void __fastcall THandUnit::tmrHandUITimer(TObject *Sender)
{
    static CStepTimer tmrEnable;
    SYSTEMTIME HandRealTime; // Windows system time
    GetLocalTime(&HandRealTime);

	// 처음식작 1초동안은 타이머 실행시키지 않음
	if(tmrEnable.OnDelay(true, 1000) == false)
		return;

	if(gSeq == NULL)
		return;



	// 프로그래스 바 세팅
//    pb_Hand->Position = RANGE_VAL(0, 100, ((double)JigCtrl->m_JigParam.nHand_CurrentCnt / JigCtrl->m_JigParam.nHand_CycleCnt) * 100);


	//Hand Input
	if(gSeq->m_pHWSet->m_pHand->ReadyToOperation() != NO_ERR)
		pnl_HandReadyStatus->Color = COLOR_FUCHSIA;
    else
        pnl_HandReadyStatus->Color = COLOR_LIME;

	 edt_HandStepMsg->Text = edt_HandStepMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Hand.Step, JigCtrl->m_StepJig_Hand.StepComment);

	pnl_HandBusy->Color = JigCtrl->m_StepJig_Hand.bSignal ? COLOR_LIME : COLOR_FUCHSIA;
	pnlIn_HandOpen->Color = gSeq->m_pHWSet-> m_pHand->IsOpen()? COLOR_LIME : COLOR_WHITE;
	pnlIn_HandClose->Color = gSeq->m_pHWSet->m_pHand->IsClose()? COLOR_LIME : COLOR_WHITE;
	pnlIn_HandAlarm->Color = gSeq->m_pHWSet->m_pHand->IsAlarm()? COLOR_FUCHSIA : COLOR_WHITE;
	pnlIn_HandInpos->Color = gSeq->m_pHWSet->m_pHand->IsInposition()? COLOR_LIME : COLOR_WHITE;

	static AnsiString prevErrMsg_Hand = "";

	if(prevErrMsg_Hand != JigCtrl->m_StepJig_Hand.ErrMsg)
	{
		edt_HandErrMsg->Text = edt_HandErrMsg->Text.sprintf(L"%d: %s",JigCtrl->m_StepJig_Hand.nErrorCode, JigCtrl->m_StepJig_Hand.ErrMsg);
		prevErrMsg_Hand = JigCtrl->m_StepJig_Hand.ErrMsg;
	}

	edtHand_RealCount->Text = edtHand_RealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHand_CurrentCnt);

}
//---------------------------------------------------------------------------



void __fastcall THandUnit::FormClose(TObject *Sender, TCloseAction &Action)
{
	SaveParam();

}
//---------------------------------------------------------------------------


void __fastcall THandUnit::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	SaveParam();
//	delete HandUnit;
}
//---------------------------------------------------------------------------

void __fastcall THandUnit::FormDestroy(TObject *Sender)
{
	SaveParam();
//	delete HandUnit;
}

void __fastcall THandUnit::btnHand_Main_ResetClick(TObject *Sender)
{
	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall THandUnit::Save_ParamClick(TObject *Sender)
{
	SaveParam();
}
//---------------------------------------------------------------------------

