// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LongRunUnit.h"
//#include "MainUnit.h"
#include "OHTInfoUnit.h"
#include "Define.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Def_TP.h"
#include <DateUtils.hpp>
#include "ChkExecUnit.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLongRunForm *LongRunForm;

#define SHIFT_MOVE_ENDPOSITION      0   ///< Shift 끝 포인트로 이동
#define SHIFT_MOVE_STARTPOSITION    1   ///< Shift 시작 포인트로 이동
#define SHIFT_CHECKRANGE            10.0  ///< Shift 정지 시 체크하는 범위

#define ROTATE_MOVE_ENDPOSITION      0   ///< ROTATE 끝 포인트로 이동
#define ROTATE_MOVE_STARTPOSITION    1   ///< ROTATE 시작 포인트로 이동
#define ROTATE_CHECKRANGE            20  ///< ROTATE 정지 시 체크하는 범위 2도
TCHKOHT CmdEUVOHTCheck;
// ---------------------------------------------------------------------------
__fastcall TLongRunForm::TLongRunForm(TComponent *Owner)
    :
    TForm(Owner)
{
    nHandCount      = 0;
    nShutterCount   = 0;
    nRSteeringCount = 0;
	nLSteeringCount = 0;

	dStartPosition = 0;
	dEndPosition = 0;

	dStartPositionRotate = 0;
	dEndPositionRotate = 0;
	nRotateCount = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TLongRunForm::FormShow(TObject *Sender)
{
//    LongRunForm->Top  = MainForm->Top + MainForm->Height;
//    LongRunForm->Left = MainForm->Left;

    if(HandTimer->Enabled == false)
        panHand->Color = clBtnFace;
    else
        panHand->Color = COLOR_LIME;

    if(ShutterTimer->Enabled == false)
        panShutter->Color = clBtnFace;
    else
        panShutter->Color = COLOR_LIME;

	if(SteeringTimer->Enabled == false)
		panSteering->Color = clBtnFace;
	else
		panSteering->Color = COLOR_LIME;


	// Rotate 유무에 따라 UI 변경
	if(gm_OHTStatus->nIsRotateExist == 1)
		grpTransAxisLongrunRotate->Visible = true;
	else
		grpTransAxisLongrunRotate->Visible = false;

}

// ---------------------------------------------------------------------------
void __fastcall TLongRunForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    timerAllDisable();
	panHand->Color         = clBtnFace;
	panShutter->Color      = clBtnFace;
	panSteering->Color     = clBtnFace;
	nHandCount             = 0;
	nShutterCount          = 0;
	nRSteeringCount        = 0;
	nLSteeringCount        = 0;
	nRotateCount = 0;

}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStart_HandClick(TObject *Sender)
{
   bool bHandRGrip = false;
   bool bHandLGrip = false;


	bHandRGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
	bHandLGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;


	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		HandTimer->Enabled = false;
		panHand->Color     = clBtnFace;
		ShowMessage("Auto Mode..Please Change Manual Mode");
	}
	else if((bHandLGrip == ON) ||
		(bHandRGrip == ON))
	{
		HandTimer->Enabled = false;
		panHand->Color     = clBtnFace;
		ShowMessage("FOSB Exists..Please Check Again.");
	}
	else
	{
		if(MessageDlg("Hand LongRun Test Start?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
		{
			HandTimer->Enabled       = true;
			panHand->Color           = COLOR_LIME;
			Hand_Stop_Time_Memo->Text = 0;
			nHandCount               = 0;
		}
		else
		{
			HandTimer->Enabled = false;
			panHand->Color     = clBtnFace;
		}
	}
	SYSTEMTIME cst;
	GetLocalTime(&cst);
	String s;
	s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
	Hand_Start_Time_Memo->Text = s;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::HandTimerTimer(TObject *Sender)
{
	static int nOCount = 0;
	static int nCCount = 0;

	bool bHandInpos = false;
	bool bHandClose = false;
	bool bHandOpen = false;

 
		bHandInpos = gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition;
		bHandClose = gm_OHTStatus->IOInput->uInputCommon.HAND_Close;
		bHandOpen = gm_OHTStatus->IOInput->uInputCommon.HAND_Open;


	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        HandTimer->Enabled = false;
		panHand->Color     = clBtnFace;
        nOCount            = 0;
        nCCount            = 0;
        return;
    }

	if(bHandInpos == OFF)
    {
        nOCount = 0;
        nCCount = 0;
        return;
    }

	if(bHandClose == ON)
    {
        nCCount = 0;
        nOCount++;

		if(nOCount >= 5)
		{
			nOCount                   = 0;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_OPEN);
			delete m_CMDControl;
		}
	}
	else if(bHandOpen == ON)
    {
		nOCount = 0;
		nCCount++;

		if(nCCount >= 5)
		{
			nCCount                   = 0;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_CLOSE);
			delete m_CMDControl;
			nHandCount++;
		}
	}
	else
	{
		nOCount = 0;
		nCCount = 0;
	}
	Hand_Count_Memo->Text = nHandCount;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStop_HandClick(TObject *Sender)
{
    HandTimer->Enabled = false;
    panHand->Color     = clBtnFace;

    SYSTEMTIME cst;
    GetLocalTime(&cst);
    String s;
	s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
	Hand_Stop_Time_Memo->Text = s;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStart_ShutterClick(TObject *Sender)
{
    bool bHandRGrip = false;
    bool bHandLGrip = false;


		bHandRGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
		bHandLGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;



	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		ShutterTimer->Enabled = false;
		panShutter->Color     = clBtnFace;
		ShowMessage("Auto Mode..Please Change Manual Mode");

	}
	else if((bHandRGrip == ON) ||
		(bHandLGrip == ON))
	{
		ShutterTimer->Enabled = false;
		panShutter->Color     = clBtnFace;
		ShowMessage("FOSB Exists..Please Check Again.");
	}
	else
	{
		if(MessageDlg("Shutter LongRun Test Start?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
		{
			ShutterTimer->Enabled       = true;
			panShutter->Color           = COLOR_LIME;
			nShutterCount               = 0;
			Shutter_Stop_Time_Memo->Text = 0;
		}
		else
		{
			ShutterTimer->Enabled = false;
			panShutter->Color     = clBtnFace;
		}
	}
	SYSTEMTIME cst;
	GetLocalTime(&cst);
	String s;
	s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
	Shutter_Start_Time_Memo->Text = s;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStop_ShutterClick(TObject *Sender)
{
	ShutterTimer->Enabled = false;
	panShutter->Color     = clBtnFace;

	SYSTEMTIME cst;
	GetLocalTime(&cst);
	String s;
	s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
	Shutter_Stop_Time_Memo->Text = s;

}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::ShutterTimerTimer(TObject *Sender)
{
	static int nOCount = 0;
	static int nCCount = 0;
    bool bShutterFOpen = false;
    bool bShutterROpen = false;
    bool bShutterFClose = false;
    bool bShutterRClose = false;
    bool bShutterFInpos = false;
    bool bShutterRInpos = false;


		bShutterFOpen = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open;
		bShutterROpen = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open;
		bShutterFClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close;
		bShutterRClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close;
		bShutterFInpos = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition;
		bShutterRInpos = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition;



	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		ShutterTimer->Enabled = false;
		panShutter->Color     = clBtnFace;
		nOCount               = 0;
		nCCount               = 0;
		return;
	}

	if(bShutterFInpos == OFF)
		return;
	if(bShutterRInpos == OFF)
        return;

	if((bShutterFClose == ON) && (bShutterRClose == ON))
    {
        nCCount = 0;
        nOCount++;

        if(nOCount >= 5)
		{
            nOCount                   = 0;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_OPEN);
			delete m_CMDControl;
        }
	}
	else if((bShutterFOpen == ON) && (bShutterROpen == ON))
	{
		nOCount = 0;
		nCCount++;

		if(nCCount >= 5)
		{
			nCCount                   = 0;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_CLOSE);
			delete m_CMDControl;
			nShutterCount++;
		}

	}
	else
	{
		nOCount = 0;
		nCCount = 0;
	}
	Shutter_Count_Memo->Text = nShutterCount;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStart_SteeringClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        SteeringTimer->Enabled = false;
        panSteering->Color     = clBtnFace;
        ShowMessage("Auto Mode..Please Change Manual Mode");
    }
    else
    {
        if(MessageDlg("Steering LongRun Test Start?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
        {
            SteeringTimer->Enabled = true;
            panSteering->Color     = COLOR_LIME;

            Steering_Stop_Time_Memo->Text = 0;
            nRSteeringCount              = 0;
            nLSteeringCount              = 0;
        }
        else
        {
            SteeringTimer->Enabled = false;
            panSteering->Color     = clBtnFace;
        }
    }
	SYSTEMTIME cst;
    GetLocalTime(&cst);
    String s;
    s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
    Steering_Start_Time_Memo->Text = s;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStop_SteeringClick(TObject *Sender)
{
    SteeringTimer->Enabled = false;
    panSteering->Color     = clBtnFace;

    SYSTEMTIME cst;
    GetLocalTime(&cst);
    String s;
    s.sprintf(L"%04d-%02d-%02d (%02d:%02d:%02d)", cst.wYear, cst.wMonth, cst.wDay, cst.wHour, cst.wMinute, cst.wSecond);
    Steering_Stop_Time_Memo->Text = s;

}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::SteeringTimerTimer(TObject *Sender)
{
    static int nRCount = 0;
    static int nLCount = 0;

    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        SteeringTimer->Enabled = false;
        panSteering->Color     = clBtnFace;
        nRCount                = 0;
        nLCount                = 0;
        return;
    }

	if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) &&
		(gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON))
    {
        nLCount = 0;
        nRCount++;
        if(nRCount >= 5)
        {
            nRCount                   = 0;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_RIGHT);
            delete m_CMDControl;
            nLSteeringCount++;
        }
    }
	else if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == ON) &&
		(gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == ON))
    {
        nRCount = 0;
        nLCount++;
        if(nLCount >= 5)
        {
            nLCount                   = 0;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
            delete m_CMDControl;
            nRSteeringCount++;
        }
    }
    else
    {
        nRCount = 0;
        nLCount = 0;
    }
    SteeringL_Count_Memo->Text = nLSteeringCount;
    SteeringR_Count_Memo->Text = nRSteeringCount;
}
// ---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStartShiftClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        timerShift->Enabled = false;
        ShowMessage("Auto Mode..Please Change Manual Mode");
    }
    else
    {
        if(MessageDlg("Shift LongRun Test Start?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
        {
            timerShift->Enabled = true;
            timerShift->Interval = 1000;

            grpTransAxisLongrun->Color = COLOR_LIME;

            // by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
            dStartPosition = edtStartPosition->Text.ToDouble();
            dEndPosition = edtEndPosition->Text.ToDouble();
        }
        else
        {
            timerShift->Enabled = false;
            grpTransAxisLongrun->Color = clBtnFace;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TLongRunForm::timerShiftTimer(TObject *Sender)
{
    uTP_CMD TPCmd;     // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    int CurStep = SHIFT_MOVE_STARTPOSITION;
    static TDateTime LastMoveTime = 0;

    // by zzang9un 2013.11.07 : Delay 시간 체크
    if(MilliSecondsBetween(Now(), LastMoveTime) <= edtDelay->Text.ToInt())
        return;

    if(gm_OHTStatus->Motor_Shift->nAxis_Running == false)
    {
        // Shift가 정지한 경우에만 CurStep 체크        
        if((gm_OHTStatus->Motor_Shift->dPosition_mm > (dStartPosition - SHIFT_CHECKRANGE)) &&
            (gm_OHTStatus->Motor_Shift->dPosition_mm < (dStartPosition + SHIFT_CHECKRANGE)))
        {
            // Start Position에 있는 경우
            CurStep = SHIFT_MOVE_ENDPOSITION;
        }
        else if((gm_OHTStatus->Motor_Shift->dPosition_mm > (dEndPosition - SHIFT_CHECKRANGE)) &&
            (gm_OHTStatus->Motor_Shift->dPosition_mm < (dEndPosition + SHIFT_CHECKRANGE)))
        {
            // Start Position에 있는 경우
            CurStep = SHIFT_MOVE_STARTPOSITION;
        }
    }
    else
        return;

    // Step에 맞는 거리값 반영
    switch(CurStep)
    {
    case SHIFT_MOVE_ENDPOSITION:
        // by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
        TPCmd.CmdTP_Jog_TR.Distance = (int)(edtEndPosition->Text.ToDouble() * 10.0);
        break;

    case SHIFT_MOVE_STARTPOSITION:
        // by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
        TPCmd.CmdTP_Jog_TR.Distance = (int)(edtStartPosition->Text.ToDouble() * 10.0);
        break;
    }

    // 속도는 edit에 있는 값을 사용
    TPCmd.CmdTP_Jog_TR.Velocity = edtVelocity->Text.ToInt();

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_ABS, &TPCmd);
    MainForm->DataDisplay(COLOR_BLACK,"JOG_SHIFT_ABS:" + String(TPCmd.CmdTP_Jog_TR.Distance));
    delete m_CMDControl;

    // 최종 이동 시간
    LastMoveTime = Now();
}
//---------------------------------------------------------------------------

void __fastcall TLongRunForm::btnStopShiftClick(TObject *Sender)
{
	timerShift->Enabled = false;
	grpTransAxisLongrun->Color = clBtnFace;
}
//---------------------------------------------------------------------------

/**
@brief   LongRunForm의 Timer를 Disable하는 함수
@author  zzang9un
@date    2013.11.19
*/
void TLongRunForm::timerAllDisable()
{
    HandTimer->Enabled = false;
    ShutterTimer->Enabled = false;
    SteeringTimer->Enabled = false;
	timerShift->Enabled = false;
	tmrRotate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TLongRunForm::btnStartRoateClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        tmrRotate->Enabled = false;
        ShowMessage("Auto Mode..Please Change Manual Mode");
    }
    else
    {
		if(MessageDlg("Rotate LongRun Test Start?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
		{

		// by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
			dStartPositionRotate = edtStartPositionRotate->Text.ToDouble();
			dEndPositionRotate = edtEndPositionRotate->Text.ToDouble();


			if((dStartPositionRotate < 0.0) || (dStartPositionRotate > 270.0) ||
			   (dEndPositionRotate < 0.0) || (dEndPositionRotate > 270.0))
			   {
                	tmrRotate->Enabled = false;
					grpTransAxisLongrunRotate->Color = clBtnFace;
			   }
			   else
			   {
			   		nRotateCount = 0;
					tmrRotate->Enabled = true;
					tmrRotate->Interval = 1000;
               		grpTransAxisLongrunRotate->Color = COLOR_LIME;
               }



        }
        else
        {
			tmrRotate->Enabled = false;
			grpTransAxisLongrunRotate->Color = clBtnFace;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TLongRunForm::btnStopRotateClick(TObject *Sender)
{
	tmrRotate->Enabled = false;
	grpTransAxisLongrunRotate->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void __fastcall TLongRunForm::tmrRotateTimer(TObject *Sender)
{
	uTP_CMD TPCmd;     // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	int CurStep = ROTATE_MOVE_STARTPOSITION;
	static TDateTime LastMoveTime = 0;

    // by zzang9un 2013.11.07 : Delay 시간 체크
    if(MilliSecondsBetween(Now(), LastMoveTime) <= edtDelayRotate->Text.ToInt())
        return;

	if(gm_OHTStatus->Motor_Rotate->nAxis_Running == false)
	{
        // Shift가 정지한 경우에만 CurStep 체크
		if((gm_OHTStatus->Motor_Rotate->dPosition_mm > ((dStartPositionRotate*10) - ROTATE_CHECKRANGE)) &&
			(gm_OHTStatus->Motor_Rotate->dPosition_mm < ((dStartPositionRotate*10) + ROTATE_CHECKRANGE)))
		{
			// Start Position에 있는 경우
			CurStep = ROTATE_MOVE_ENDPOSITION;
        }
		else if((gm_OHTStatus->Motor_Rotate->dPosition_mm > ((dEndPositionRotate*10) - ROTATE_CHECKRANGE)) &&
			(gm_OHTStatus->Motor_Rotate->dPosition_mm < ((dEndPositionRotate*10) + ROTATE_CHECKRANGE)))
        {
            // Start Position에 있는 경우
			CurStep = ROTATE_MOVE_STARTPOSITION;
		}else
			return;
	}
    else
        return;

    // Step에 맞는 거리값 반영
    switch(CurStep)
    {
    case ROTATE_MOVE_ENDPOSITION:
        // by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
		TPCmd.CmdTP_Jog_TR.Distance = (int)(dEndPositionRotate* 10.0);
        break;

	case ROTATE_MOVE_STARTPOSITION:
        // by zzang9un 2013.11.07 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
		TPCmd.CmdTP_Jog_TR.Distance = (int)(dStartPositionRotate * 10.0);
        break;
    }

	// 속도는 edit에 있는 값을 사용
	if((gm_OHTStatus->IOInput->uInput.Input_EUV.HOIST_Home == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) < HOIST_ORIGIN_LIMIT))
	{
		if((CmdEUVOHTCheck.Is_Executable() == NO_ERR) && (CmdEUVOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
		{
           	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
			MainForm->DataDisplay(COLOR_BLACK,"JOG_ROTATE_ABS:" + String(TPCmd.CmdTP_Jog_TR.Distance));
			delete m_CMDControl;

			nRotateCount++;
			Rotate_Count_Memo->Text = nRotateCount;
		}
		else
		{
			tmrRotate->Enabled = false;
			grpTransAxisLongrunRotate->Color = clBtnFace;
		}
	}
	else
	{
		tmrRotate->Enabled = false;
		grpTransAxisLongrunRotate->Color = clBtnFace;
    }


    // 최종 이동 시간
    LastMoveTime = Now();
}
//---------------------------------------------------------------------------
