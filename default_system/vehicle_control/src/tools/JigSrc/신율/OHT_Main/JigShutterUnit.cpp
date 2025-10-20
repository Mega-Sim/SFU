//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JigShutterUnit.h"
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

#define FRONT           0 ///< Front
#define REAR            1 ///< Rear

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TShutterUnit *ShutterUnit;
JigThread *JigCtrl = NULL;
TaskControl *gSeq = NULL;

//---------------------------------------------------------------------------
__fastcall TShutterUnit::TShutterUnit(TComponent* Owner)
    : TForm(Owner)
{


}

//---------------------------------------------------------------------------
void  TShutterUnit::Init()
{
	gSeq = OHTMainForm->GetTaskControl();
	JigCtrl = OHTMainForm->GetJigCtrl();
	bCtrlFlag = false;
    bStartFlag = false;
}
//---------------------------------------------------------------------------


void __fastcall TShutterUnit::btnShutterInitClick(TObject *Sender)
{
    if(bCtrlFlag)
        JigCtrl->m_StepJig_Shutter.Step = 1;

}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::btnShutter_ResetClick(TObject *Sender)
{
		JigCtrl->m_StepJig_Shutter.bSignal = true;
		JigCtrl->m_StepJig_Shutter.Step = 2;
}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::btn_ShutterEStopClick(TObject *Sender)
{
		JigCtrl->m_StepJig_Shutter.bSignal = false;
		JigCtrl->m_StepJig_Shutter.StepComment = "Emergency Stop";
}
//---------------------------------------------------------------------------
void __fastcall TShutterUnit::btnShutter_RunClick(TObject *Sender)
{
    if(bCtrlFlag)
    {
        JigCtrl->m_StepJig_Shutter.Step = 3;
        UpdateParam(true);
        bStartFlag = true;
    }

}
//---------------------------------------------------------------------------


void __fastcall TShutterUnit::btnOut_ShutterOpenClick(TObject *Sender)
{
    if(bCtrlFlag)
        JigCtrl->m_StepJig_Shutter.Step = 200;
}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::btnOut_ShutterCloseClick(TObject *Sender)
{
    if(bCtrlFlag)
        JigCtrl->m_StepJig_Shutter.Step = 201;
}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::btnOut_AlarmResetClick(TObject *Sender)
{
    if(bCtrlFlag)
        JigCtrl->m_StepJig_Shutter.Step = 202;
}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::btnShutter_LoadParamClick(TObject *Sender)
{
    LoadParam();
}
//---------------------------------------------------------------------------

void TShutterUnit::LoadParam()
{

    LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

    pIniFile->ReadInteger("Setting", "Shutter_Interval_ms", 0, &JigCtrl->m_JigParam.nShutter_Interval_ms);
	pIniFile->ReadInteger("Setting", "Shutter_CycleCnt", 0, &JigCtrl->m_JigParam.nShutter_CycleCnt);
    pIniFile->ReadInteger("Setting", "Shutter_LastCnt", 0, &JigCtrl->m_JigParam.nShutter_CurrentCnt);
    UpdateParam(false);

	delete pIniFile;

}
//---------------------------------------------------------------------------
void TShutterUnit::SaveParam()
{
    LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->WriteInteger("Setting", "Shutter_Interval_ms", JigCtrl->m_JigParam.nShutter_Interval_ms);
	pIniFile->WriteInteger("Setting", "Shutter_CycleCnt", JigCtrl->m_JigParam.nShutter_CycleCnt);
    pIniFile->WriteInteger("Setting", "Shutter_LastCnt",  JigCtrl->m_JigParam.nShutter_CurrentCnt);
	delete pIniFile;

}
//---------------------------------------------------------------------------
void TShutterUnit::UpdateParam(bool bUI_toParam)
{
	// UI --> 메모리 Param으로 업데이트
	if(bUI_toParam)
	{

		JigCtrl->m_JigParam.nShutter_Interval_ms = StrToInt(edtShutter_Interval->Text.c_str());
		JigCtrl->m_JigParam.nShutter_CycleCnt = StrToInt(edtShutter_Count->Text.c_str());


		SaveParam();
	}
	// Parameter -> UI로 업데이트
	else
	{

		edtShutter_Interval->Text = edtShutter_Interval->Text.sprintf(L"%d", JigCtrl->m_JigParam.nShutter_Interval_ms);
		edtShutter_Count->Text = edtShutter_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nShutter_CycleCnt);

		Invalidate();
    }

}

void __fastcall TShutterUnit::chkShutterLongrunClick(TObject *Sender)
{

    if(chkShutterLongrun->Checked == true)
        JigCtrl->m_StepJig_Shutter.bLongrun = true;
    else
        JigCtrl->m_StepJig_Shutter.bLongrun = false;
}
//---------------------------------------------------------------------------


void __fastcall TShutterUnit::tmrShutterUITimer(TObject *Sender)
{
    static CStepTimer tmrEnable;
    SYSTEMTIME ShutterRealTime; // Windows system time
    GetLocalTime(&ShutterRealTime);

	// 처음식작 1초동안은 타이머 실행시키지 않음
	if(tmrEnable.OnDelay(true, 1000) == false)
		return;

	if(JigCtrl == NULL)
		return;

    bCtrlFlag = !JigCtrl->m_StepJig_Shutter.bSignal;

	// 프로그래스 바 세팅
    pb_Shutter->Position = RANGE_VAL(0, 100, ((double)JigCtrl->m_JigParam.nShutter_CurrentCnt / JigCtrl->m_JigParam.nShutter_CycleCnt) * 100);


    //Shutter Input
    if(gSeq->m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR)
        pnl_ShutterReadyStatus->Color = COLOR_FUCHSIA;
    else
        pnl_ShutterReadyStatus->Color = COLOR_LIME;

    pnl_ShutterBusy->Color = JigCtrl->m_StepJig_Shutter.bSignal ? COLOR_LIME : COLOR_FUCHSIA;
    pnlIn_ShutterFOpen->Color = gSeq->m_pHWSet-> m_pShutter->IsOpen(FRONT)? COLOR_LIME : COLOR_WHITE;
    pnlIn_ShutterROpen->Color = gSeq->m_pHWSet->m_pShutter->IsOpen(REAR)? COLOR_LIME : COLOR_WHITE;
    pnlIn_ShutterFClose->Color = gSeq->m_pHWSet->m_pShutter->IsClose(FRONT)? COLOR_LIME : COLOR_WHITE;
    pnlIn_ShutterRClose->Color = gSeq->m_pHWSet->m_pShutter->IsClose(REAR)? COLOR_LIME : COLOR_WHITE;
    pnlIn_ShutterFAlarm->Color = gSeq->m_pHWSet->m_pShutter->IsAlarm(FRONT)? COLOR_FUCHSIA : COLOR_WHITE;
    pnlIn_ShutterRAlarm->Color = gSeq->m_pHWSet->m_pShutter->IsAlarm(REAR)? COLOR_FUCHSIA : COLOR_WHITE;
    pnlIn_ShutterInpos->Color = gSeq->m_pHWSet->m_pShutter->IsInposition(BOTH)? COLOR_LIME : COLOR_WHITE;

    //롱런 시작 시간 표기
    if(bStartFlag == true)
    {
        edt_ShutterDate->Text = edt_ShutterDate->Text.sprintf(L"%d.%02d.%02d.%02d:%02d",ShutterRealTime.wYear, ShutterRealTime.wMonth, ShutterRealTime.wDay, ShutterRealTime.wHour, ShutterRealTime.wMinute);

    }

    //스텝 메시지
    edt_ShutterStepMsg->Text = edt_ShutterStepMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Shutter.Step, JigCtrl->m_StepJig_Shutter.StepComment);


    // 에러 메시지
    static String prevErrMsg_Shutter = "No error";

    if(prevErrMsg_Shutter != JigCtrl->m_StepJig_Shutter.ErrMsg)
    {
		edt_ShutterErrMsg->Text = edt_ShutterErrMsg->Text.sprintf(L"%s: %d, count: %d",JigCtrl->m_StepJig_Shutter.ErrMsg, JigCtrl->GetJigError(), JigCtrl->Err_cnt);
		prevErrMsg_Shutter = JigCtrl->m_StepJig_Shutter.ErrMsg;
    }

    //에러시간 표기
    if(JigCtrl->m_StepJig_Shutter.bError)
    {
        edt_ShutterErrTime->Text = edt_ShutterErrTime->Text.sprintf(L"%d.%02d.%02d.%02d:%02d",ShutterRealTime.wYear, ShutterRealTime.wMonth, ShutterRealTime.wDay, ShutterRealTime.wHour, ShutterRealTime.wMinute);
        JigCtrl->m_StepJig_Shutter.bError = false;
    }


    //카운트 표기
    edtShutter_RealCount->Text = edtShutter_RealCount->Text.sprintf(L"%d/%d", JigCtrl->m_JigParam.nShutter_CurrentCnt,JigCtrl->m_JigParam.nShutter_CycleCnt);


}
//---------------------------------------------------------------------------



void __fastcall TShutterUnit::FormClose(TObject *Sender, TCloseAction &Action)
{
    SaveParam();
	Action = true;
    return;
}
//---------------------------------------------------------------------------


void __fastcall TShutterUnit::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    SaveParam();
}
//---------------------------------------------------------------------------

void __fastcall TShutterUnit::FormDestroy(TObject *Sender)
{
    SaveParam();
}
//---------------------------------------------------------------------------

