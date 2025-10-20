//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHTMain.h"
#include "JigHoistUnit.h"
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



#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#define HOIST_SPEED_RATIO      0.8573       // OHT V7.0 감속비 12.86:1
#else
#define HOIST_SPEED_RATIO      1.0      // OHT V7.0 감속비 15:1
#endif

#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MAX_SPEED	2.75
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 400.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 280.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#endif

#define Hoist_Home	300
//---------------------------------------------------------------------------

#define LOG_JIG(...)   WRITE_LOG_UNIT(pLogJig, __VA_ARGS__)

#pragma package(smart_init)
#pragma resource "*.dfm"

THoistUnit *HoistUnit;
JigThread *JigCtrl = NULL;
TaskControl *gSeq = NULL;


//---------------------------------------------------------------------------
__fastcall THoistUnit::THoistUnit(TComponent* Owner)
	: TForm(Owner)
{
	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	gSeq = NULL;
	JigCtrl = NULL;
	Init();

	//Hoist축 세팅
	gSeq->m_pHWCommon->Set_Jig(2, true); // Hoist 축

	gSeq->bHoistJig = true;     //지그 Setting
	JigCtrl->m_StepJig_Hoist.bSignal = true;                    //시그널
	OHTMainForm->GetTransControl()->bHoistJig = true;           //Trans control  Hoist 설정 On
	OHTMainForm->GetTransControl()->nHoistTeaching = 0;         //Teaching
	OHTMainForm->GetTransControl()->m_dHoistHomePos = 0;        //Teaching
	m_pCmdManager = OHTMainForm->GetCmdManager();               //Main Reset을 위해 사용

	//IO Setting
	//에러 무시처리 I/O Mask : 44, 46, 47, 48, 64
	MASK_INPUT  *pMaskOfInput   = NULL;

// IO 무시처리 timy

#if(OHT_NAME == OHT_NAME_STD_V85)
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(46-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(48-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(49-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(51-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(66-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

#elif (OHT_NAME == OHT_NAME_STD_V86)     //timy 8.6 추가
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(47-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(49-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(50-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(51-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(67-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

#else
	//2가지로 운영 (*1) - V8.1 , (*2) - V8.2
	//(*1)
	/*
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(44-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(46-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(47-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(48-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(64-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;
	*/

	//(*2)
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(46);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(48);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(49);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(50);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(62);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(66);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;
#endif


	//셔터 input 무시처리 : 21~28
	//셔터 오리진 무시처리 : m_HWSet->m_pShutter->bHoistJig

#if (OHT_NAME == OHT_NAME_STD_V86)      //timy 8.6 추가
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(22-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(23-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(24-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(25-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(26-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(27-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(28-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(29-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

#else
	//2가지로 운영 (*1) - V8.1 V8.5  , (*2) - V8.2
	//(*1)
	/*
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(21-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(22-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(23-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(24-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(25-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(26-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(27-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(28-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;
	*/

	//(*2)
	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(21);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(22);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(23);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(24);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(25);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(26);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(27);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(28);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;
#endif

	gSeq->m_pHWSet->m_pShutter->bHoistJig = true;    // Shutter에서 Shutter Ready 상태로 만들어줌
	edt_RelMoveVal->Text  = "0";

	LoadParam();
	UpdateParam(false);
}
void THoistUnit::Init()
{
	gSeq = OHTMainForm->GetTaskControl();
	JigCtrl = OHTMainForm->GetJigCtrl();
}
//---------------------------------------------------------------------------
void __fastcall THoistUnit::FormClose(TObject *Sender, TCloseAction &Action)
{
	//Param 저장
	SaveParam();
}
//---------------------------------------------------------------------------



void THoistUnit::UpdateParam(bool bUI_toParam)      // 수정 예정
{
	// UI --> 메모리 Param으로 업데이트
	if(bUI_toParam)
	{
//		JigCtrl->m_JigParam.dHoist_HomePos =   StrToFloat(edt_HoistHomePos->Text.c_str());
		JigCtrl->m_JigParam.dHoist_TeachingPos = StrToFloat(edt_HoistMoveDist->Text.c_str());
		JigCtrl->m_JigParam.nHoist_Interval_ms = StrToInt(edt_HoistInterval->Text.c_str());
		JigCtrl->m_JigParam.nHoist_CurrentCnt = StrToInt(edt_HoistRealCount->Text.c_str());
		JigCtrl->m_JigParam.nHoist_CycleCnt = StrToInt(edt_HoistCount->Text.c_str());

		SaveParam();
	}
	// 메모리 Param -> UI로 업데이트
	else
	{
//		edt_HoistHomePos->Text   = edt_HoistHomePos->Text.sprintf(L"%g", JigCtrl->m_JigParam.dHoist_HomePos);
		edt_HoistMoveDist->Text  = edt_HoistMoveDist->Text.sprintf(L"%g", JigCtrl->m_JigParam.dHoist_TeachingPos);
		edt_HoistInterval->Text  = edt_HoistInterval->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHoist_Interval_ms);
		edt_HoistCount->Text 	 = edt_HoistCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHoist_CycleCnt);
		edt_HoistRealCount->Text = edt_HoistRealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHoist_CurrentCnt);
	}

}

void THoistUnit::SaveParam()
{
	LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

//	pIniFile->WriteDouble("Setting", "dHoist_HomePos", JigCtrl->m_JigParam.dHoist_HomePos);
	pIniFile->WriteDouble("Setting", "dHoist_TeachingPos", JigCtrl->m_JigParam.dHoist_TeachingPos);
	pIniFile->WriteInteger("Setting", "nHoist_Interval_ms", JigCtrl->m_JigParam.nHoist_Interval_ms);
	pIniFile->WriteInteger("Setting", "nHoist_CycleCnt", JigCtrl->m_JigParam.nHoist_CycleCnt);
	pIniFile->WriteInteger("Setting", "nHoist_CurrentCnt", JigCtrl->m_JigParam.nHoist_CurrentCnt);

	delete pIniFile;
}

void THoistUnit::LoadParam()
{
	LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

//	pIniFile->ReadDouble("Setting", "dHoist_HomePos", 0, &JigCtrl->m_JigParam.dHoist_HomePos);
	pIniFile->ReadDouble("Setting", "dHoist_TeachingPos", 0, &JigCtrl->m_JigParam.dHoist_TeachingPos);
	pIniFile->ReadInteger("Setting", "nHoist_Interval_ms", 0, &JigCtrl->m_JigParam.nHoist_Interval_ms);
	pIniFile->ReadInteger("Setting", "nHoist_CycleCnt", 0, &JigCtrl->m_JigParam.nHoist_CycleCnt);
	pIniFile->ReadInteger("Setting", "nHoist_CurrentCnt", 0, &JigCtrl->m_JigParam.nHoist_CurrentCnt);

	delete pIniFile;

//	UpdateParam(false);
}

void __fastcall THoistUnit::tmr1Timer(TObject *Sender)
{
	static CStepTimer tmrEnable;

	//	dValue  = edt_RelMoveVal->Text.ToDouble(); //왜 쓰는지 모르겠음

	// 처음시작 몇 초동안은 타이머 실행시키지 않음
	if(tmrEnable.OnDelay(true, 1000) == false)
		return;

	if(gSeq == NULL)
	{
//		LOG_JIG("gSeq is NULL");
		return;
	}

	// 프로그래스 바 세팅
   //progress_Hoist->Position = RANGE_VAL(0, 100, ((double)JigCtrl->m_JigParam.nHoist_CurrentCnt / JigCtrl->m_JigParam.nHoist_CycleCnt) * 100);

   edt_HoistStepMsg->Text = edt_HoistStepMsg->Text.sprintf(L"%d: %s, %d", JigCtrl->m_StepJig_Hoist.Step, JigCtrl->m_StepJig_Hoist.StepComment, gSeq->m_pHWSet->m_pHoistAxis->IsAMPEnable());
   pnl_HoistServoStat->Color = gSeq->m_pHWSet->m_pHoistAxis->IsAMPEnable() ? COLOR_LIME : COLOR_FUCHSIA;
   pnl_HoistCurPos->Caption = (int)(((int)(gSeq->m_pHWSet->m_pHoistAxis->GetCurrentPosition()*1000))/1000.0);
   pnl_HoistVel->Caption = ((int)(gSeq->m_pHWSet->m_pHoistAxis->GetCurrentVelocity()*1000))/1000.0;
   pnl_HoistBusy->Color = gSeq->m_pHWSet->m_pHoistAxis->IsRunning() ? COLOR_LIME : COLOR_FUCHSIA;
//   pnl_HoistHome->Color = gSeq->m_pHWSet->m_pHoistAxis->IsHoistHome() ? COLOR_LIME : COLOR_FUCHSIA;
   pnl_HoistHome->Color =  ((gSeq->m_pHWSet->m_pHoistAxis->GetCurrentPosition() < Hoist_Home+2) &&
							 (gSeq->m_pHWSet->m_pHoistAxis->GetCurrentPosition() > Hoist_Home-2)) ? COLOR_LIME : COLOR_FUCHSIA;
   pnl_HoistReady->Color = JigCtrl->m_StepJig_Hoist.bSignal ? COLOR_LIME : COLOR_FUCHSIA;

   //에러 메시지 보여주기
   static AnsiString prevErrMsg_Hoist = "";

   if(prevErrMsg_Hoist != JigCtrl->m_StepJig_Hoist.ErrMsg)
   {
		edt_HoistErrMsg->Text = edt_HoistErrMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Hoist.nErrorCode, JigCtrl->m_StepJig_Hoist.ErrMsg);
		prevErrMsg_Hoist = JigCtrl->m_StepJig_Hoist.ErrMsg;
   }

	edt_HoistRealCount->Text = edt_HoistRealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nHoist_CurrentCnt);


}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btnHoist_LoadParamClick(TObject *Sender)
{
	LoadParam();
	UpdateParam(false);
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btnHoistAxis_RunClick(TObject *Sender)
{
//	UpdateParam();
	LoadParam();


	if(JigCtrl->m_StepJig_Hoist.Step != 0)
	{
		Application->MessageBox(L"Hoist Step isn't Initialized...", L"Error", MB_OK + MB_ICONSTOP);
		return;
	}

	int value = (int)gSeq->m_pHWSet->m_pHoistAxis->GetCurrentPosition();     //현재 위치값


	if((value < Hoist_Home+2) &&
							 (value > Hoist_Home-2))
//		(value < 1 &&
//		value > -1 )
//		gSeq->m_pHWSet->m_pHoistAxis->IsHoistHome())

	{
		JigCtrl->m_StepJig_Hoist.Step = 100;
		GetLocalTime(&CurTime);
		edtStartDate->Text = edtStartDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
	}
	else
	{
		edt_HoistErrMsg->Text = edt_HoistErrMsg->Text.sprintf(L"Hoist isn't in home");
	}
}
//---------------------------------------------------------------------------


void __fastcall THoistUnit::chkHoistLongrunClick(TObject *Sender)
{
	if(chkHoistLongrun->Checked == true)
		JigCtrl->m_StepJig_Hoist.bLongrun = true;
	else
		JigCtrl->m_StepJig_Hoist.bLongrun = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btn_Servo_OnClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.bSignal == true)
		JigCtrl->m_StepJig_Hoist.Step = 1;
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btn_Servo_OffClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.bSignal == true)
		JigCtrl->m_StepJig_Hoist.Step = 2;
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btn_HoistHomeClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.Step == 0 && JigCtrl->m_StepJig_Hoist.bSignal == true)
		JigCtrl->m_StepJig_Hoist.Step = 3;
}
//---------------------------------------------------------------------------


void __fastcall THoistUnit::btnHoistSetHomeClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.Step == 0 && JigCtrl->m_StepJig_Hoist.bSignal == true)
		JigCtrl->m_StepJig_Hoist.Step = 4;
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btn_HoistJog_PClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.Step == 0 && JigCtrl->m_StepJig_Hoist.bSignal == true)
	{
		int nVal = edt_RelMoveVal->Text.ToIntDef(1);
		JigCtrl->nJogVal = nVal;
		JigCtrl->m_StepJig_Hoist.Step = 5;
	}
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btn_HoistJog_NClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Hoist.Step == 0 && JigCtrl->m_StepJig_Hoist.bSignal == true)
	{
		int nVal = edt_RelMoveVal->Text.ToIntDef(1);
		JigCtrl->nJogVal = nVal;
		JigCtrl->m_StepJig_Hoist.Step = 6;
	}
}
//---------------------------------------------------------------------------


void __fastcall THoistUnit::btnHoist_EStopClick(TObject *Sender)
{

	gSeq->m_pHWSet->m_pHoistAxis->MoveStop(0.0065);      // Move Stop으로 변경

	if(JigCtrl->m_StepJig_Hoist.Step>=100 &&
	JigCtrl->m_StepJig_Hoist.Step<200)
	{
		GetLocalTime(&CurTime);
		edtEndDate->Text = edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
	}
	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_STOP);    // E-Stop

	JigCtrl->m_StepJig_Hoist.Step = 200;
	JigCtrl->m_StepJig_Hoist.StepComment = "E-Stop";
	JigCtrl->m_StepJig_Hoist.bSignal = false;

}
//---------------------------------------------------------------------------


void __fastcall THoistUnit::btn_HoistResetClick(TObject *Sender)
{
	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);

	if(JigCtrl->m_StepJig_Hoist.bSignal == false)
	{
		gSeq->m_pHWSet->m_pHoistAxis->Reset();
		JigCtrl->m_StepJig_Hoist.bSignal = true;
		JigCtrl->m_StepJig_Hoist.Step = 0;
		JigCtrl->m_StepJig_Hoist.StepComment = "Thread Reset";
		GetLocalTime(&CurTime);
		edtStartDate->Text ="0";
		edtEndDate->Text ="0";

	}
}
//---------------------------------------------------------------------------




void __fastcall THoistUnit::btnHoist_Main_ResetClick(TObject *Sender)
{
	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
}
//---------------------------------------------------------------------------



void __fastcall THoistUnit::Save_ParamClick(TObject *Sender)
{
	SaveParam();
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::chkHomePosChangeClick(TObject *Sender)
{
	if(chkHomePosChange->Checked == true)
		JigCtrl->m_StepJig_Hoist.bHomeChange = true;
	else
		JigCtrl->m_StepJig_Hoist.bHomeChange = false;
}
//---------------------------------------------------------------------------




void __fastcall THoistUnit::btnOnClick(TObject *Sender)
{
	gSeq->m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(true);
}
//---------------------------------------------------------------------------

void __fastcall THoistUnit::btnOffClick(TObject *Sender)
{
	gSeq->m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(false);
}
//---------------------------------------------------------------------------

