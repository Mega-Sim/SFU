//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JigRotateUnit.h"
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

TRotateUnit *RotateUnit;
JigThread *JigCtrl = NULL;
TaskControl *gSeq = NULL;



//---------------------------------------------------------------------------
__fastcall TRotateUnit::TRotateUnit(TComponent* Owner)
	: TForm(Owner)
{
	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pCmdManager = OHTMainForm->GetCmdManager();

	gSeq = NULL;
	JigCtrl = NULL;
	Init();


	JigCtrl->m_StepJig_Rotate.bSignal = true;  //로테이트 시그널 On



}

//---------------------------------------------------------------------------
void  TRotateUnit::Init()
{
	gSeq = OHTMainForm->GetTaskControl();
	JigCtrl = OHTMainForm->GetJigCtrl();
//	JigCtrl->m_StepJig_Rotate.bRunning = true; // Rotate Unit 구동 시작
//	bCtrlFlag = false;
//	bStartFlag = false;


}
//---------------------------------------------------------------------------


void __fastcall TRotateUnit::btnRotateInitClick(TObject *Sender)   // Origin
{
	if(JigCtrl->m_StepJig_Rotate.Step == 0 && JigCtrl->m_StepJig_Rotate.bSignal == true)

		JigCtrl->m_StepJig_Rotate.Step = 1;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnRotate_ResetClick(TObject *Sender)    // Reset
 //리셋기능 추가 필요.
{
	if(JigCtrl->m_StepJig_Rotate.bSignal == false)
	{
		JigCtrl->m_StepJig_Rotate.bSignal = true;
		JigCtrl->m_StepJig_Rotate.Step = 2;
		JigCtrl->m_StepJig_Rotate.StepComment = "Thread Reset";
		edt_RotateErrMsg->Text = edt_RotateErrMsg->Text.sprintf(L"");
	}

}
//---------------------------------------------------------------------------



void __fastcall TRotateUnit::btn_RotateEStopClick(TObject *Sender)       // E-Stop
{
//	UpdateParam(true);
//	SaveParam();

	if(JigCtrl->m_StepJig_Rotate.Step>=100 && JigCtrl->m_StepJig_Rotate.Step<200)
	{
		GetLocalTime(&CurTime);
		edt_RotateErrTime->Text = edt_RotateErrTime->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	}

	JigCtrl->m_StepJig_Rotate.bSignal = false;
	JigCtrl->m_StepJig_Rotate.Step = 200;
	JigCtrl->m_StepJig_Rotate.StepComment = "Emergency Stop";
}
//---------------------------------------------------------------------------
void __fastcall TRotateUnit::btnRotate_RunClick(TObject *Sender)      // Long Run
{
//	UpdateParam(false);
	LoadParam();

	if(JigCtrl->m_StepJig_Rotate.Step != 0) // Init 상태 아니면 못하게 막음
	{
		Application->MessageBox(L"Rotate Step isn't Initialized...", L"Error", MB_OK + MB_ICONSTOP);
		return;
	}

	if((gSeq->m_pHWSet-> m_pRotate->Is0Pos())&&(gSeq->m_pHWSet->m_pRotate->IsInposition()))   // 0도하고 인포지션 들어와야 시작
	{
		JigCtrl->m_StepJig_Rotate.Step = 100;
		GetLocalTime(&CurTime);
		edt_RotateDate->Text = edt_RotateDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
		edt_RotateErrMsg->Text = edt_RotateErrMsg->Text.sprintf(L"");
	}
	else
	{
		edt_RotateErrMsg->Text = edt_RotateErrMsg->Text.sprintf(L"Rotate isn't Origin");
	}


}
//---------------------------------------------------------------------------


void __fastcall TRotateUnit::btnOut_Rotate0Click(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Rotate.Step == 0 && JigCtrl->m_StepJig_Rotate.bSignal == true)
		JigCtrl->m_StepJig_Rotate.Step = 4;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnOut_Rotate90Click(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Rotate.Step == 0 && JigCtrl->m_StepJig_Rotate.bSignal == true)
		JigCtrl->m_StepJig_Rotate.Step = 5;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnOut_Rotate180Click(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Rotate.Step == 0 && JigCtrl->m_StepJig_Rotate.bSignal == true)
		JigCtrl->m_StepJig_Rotate.Step = 6;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnOut_Rotate270Click(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Rotate.Step == 0 && JigCtrl->m_StepJig_Rotate.bSignal == true)
		JigCtrl->m_StepJig_Rotate.Step = 7;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnOut_AlarmResetClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Rotate.bSignal == true)
		JigCtrl->m_StepJig_Rotate.Step = 8;
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::btnRotate_LoadParamClick(TObject *Sender)
{
	LoadParam();
	UpdateParam(false);
}
//---------------------------------------------------------------------------

void TRotateUnit::LoadParam()     // 파라미터 읽어오는거
{

	LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->ReadInteger("Setting", "Rotate_Interval_ms", 0, &JigCtrl->m_JigParam.nRotate_Interval_ms);
	pIniFile->ReadInteger("Setting", "Rotate_CycleCnt", 0, &JigCtrl->m_JigParam.nRotate_CycleCnt);
	pIniFile->ReadInteger("Setting", "Rotate_LastCnt", 0, &JigCtrl->m_JigParam.nRotate_CurrentCnt);
//	UpdateParam(false);

	delete pIniFile;

}
//---------------------------------------------------------------------------
void TRotateUnit::SaveParam()    // 파라미터 저장하는거
{
    LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->WriteInteger("Setting", "Rotate_Interval_ms", JigCtrl->m_JigParam.nRotate_Interval_ms);
	pIniFile->WriteInteger("Setting", "Rotate_CycleCnt", JigCtrl->m_JigParam.nRotate_CycleCnt);
	pIniFile->WriteInteger("Setting", "Rotate_LastCnt",  JigCtrl->m_JigParam.nRotate_CurrentCnt);

	delete pIniFile;

}
//---------------------------------------------------------------------------
void TRotateUnit::UpdateParam(bool bUI_toParam)
{
	// UI --> 파일로 업데이트
	if(bUI_toParam)
	{
//		JigCtrl->m_JigParam.nRotate_Interval_ms = StrToInt(edtRotate_Interval->Text.c_str());
//		JigCtrl->m_JigParam.nRotate_CycleCnt = StrToInt(edtRotate_Count->Text.c_str());
//		JigCtrl->m_JigParam.nRotate_CurrentCnt = StrToInt(edtRotate_Current_Count->Text.c_str());


		SaveParam();
	}
	// Parameter -> UI로 업데이트
	else
	{
		edtRotate_Interval->Text = edtRotate_Interval->Text.sprintf(L"%d", JigCtrl->m_JigParam.nRotate_Interval_ms);
		edtRotate_Count->Text = edtRotate_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nRotate_CycleCnt);
		edtRotate_Current_Count->Text = edtRotate_Current_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nRotate_CurrentCnt);
	}

}

void __fastcall TRotateUnit::chkRotateLongrunClick(TObject *Sender)
{

	if(chkRotateLongrun->Checked == true)
		JigCtrl->m_StepJig_Rotate.bLongrun = true;
	else
		JigCtrl->m_StepJig_Rotate.bLongrun = false;
}
//---------------------------------------------------------------------------


void __fastcall TRotateUnit::tmrRotateUITimer(TObject *Sender)
{
	static CStepTimer tmrEnable;

	// 처음시작 1초동안은 타이머 실행시키지 않음
	if(tmrEnable.OnDelay(true, 1000) == false)
	{
		return;
	}

	if(gSeq == NULL)
	{
		return;
	}

	// 프로그래스 바 세팅
	//이게 뭔지 모르겠음..
	//pb_Rotate->Position = RANGE_VAL(0, 100, ((double)JigCtrl->m_JigParam.nRotate_CurrentCnt / JigCtrl->m_JigParam.nRotate_CycleCnt) * 100);



	//Rotate Input
	if(gSeq->m_pHWSet->m_pRotate->ReadyToOperation() != NO_ERR)
		pnl_RotateReadyStatus->Color = COLOR_FUCHSIA;
	else
		pnl_RotateReadyStatus->Color = COLOR_LIME;


	pnl_RotateBusy->Color = JigCtrl->m_StepJig_Rotate.bSignal ? COLOR_LIME : COLOR_FUCHSIA;
	pnlIn_Rotate0->Color = gSeq->m_pHWSet-> m_pRotate->Is0Pos()? COLOR_LIME : COLOR_WHITE;
	pnlIn_Rotate90->Color = gSeq->m_pHWSet->m_pRotate->Is90Pos()? COLOR_LIME : COLOR_WHITE;
	pnlIn_Rotate180->Color = gSeq->m_pHWSet->m_pRotate->Is180Pos()? COLOR_LIME : COLOR_WHITE;
	pnlIn_Rotate270->Color = gSeq->m_pHWSet->m_pRotate->Is270Pos()? COLOR_LIME : COLOR_WHITE;
	pnlIn_RotateAlarm->Color = gSeq->m_pHWSet->m_pRotate->IsAlarm()? COLOR_FUCHSIA : COLOR_WHITE;
	pnlIn_RotateInpos->Color = gSeq->m_pHWSet->m_pRotate->IsInposition()? COLOR_LIME : COLOR_WHITE;



	edt_RotateStepMsg->Text = edt_RotateStepMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Rotate.Step, JigCtrl->m_StepJig_Rotate.StepComment);

	// 에러 메시지
	static AnsiString prevErrMsg_Rotate = "";

	if(prevErrMsg_Rotate != JigCtrl->m_StepJig_Rotate.ErrMsg)
	{
		edt_RotateErrMsg->Text = edt_RotateErrMsg->Text.sprintf(L"%d: %s",JigCtrl->m_StepJig_Rotate.nErrorCode, JigCtrl->m_StepJig_Rotate.ErrMsg);
		prevErrMsg_Rotate = JigCtrl->m_StepJig_Rotate.ErrMsg;
	}

	edt_Rotate_RealCount->Text = edt_Rotate_RealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nRotate_CurrentCnt);

}
//---------------------------------------------------------------------------



void __fastcall TRotateUnit::FormClose(TObject *Sender, TCloseAction &Action)
{
	//Param 저장
	SaveParam();


}
//---------------------------------------------------------------------------


//void __fastcall TRotateUnit::FormCloseQuery(TObject *Sender, bool &CanClose)
//{
//	SaveParam();
//}
////---------------------------------------------------------------------------
//
void __fastcall TRotateUnit::FormDestroy(TObject *Sender)
{
	SaveParam();
//	delete RotateUnit;
}

////---------------------------------------------------------------------------



void __fastcall TRotateUnit::btnRotate_Main_ResetClick(TObject *Sender)
{
	m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
}
//---------------------------------------------------------------------------

void __fastcall TRotateUnit::Save_ParamClick(TObject *Sender)
{
	SaveParam();
}
//---------------------------------------------------------------------------

