// ---------------------------------------------------------------------------



#include <vcl.h>
#include <math.h>
#include <DateUtils.hpp>
#pragma hdrstop

#include "ReticleOnlineCMDUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "OHTInfoUnit.h"
#include "Define.h"
#include "SocketUnit.h"
#include "SpeedHelpUnit.h"
#include "StatusUnit.h"
#include "WarnningUnit.h"

#include "LocMSave.h"
#include "CmdWinUnit.h"
#include "ChkExecUnit.h"
#include "RotateOriginUnit.h"
#include "ConversionUnit.h"
#include "MultiJogDistUnit.h"
#include "ConnectUnit.h"
#include "HandOpenUnit.h"
#include "HoistParamUnit.h"
#include "TransUnit.h"
//#include "TeachingSaveUnit.h"

#include "Def_TP.h"

// #include "AutoJob.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

// #include "ResponsePasspermitOCS.h"
// #include "OcsPassConfirmResponse.h"
// #include "RunningStatus.h"

#include "Protocol_Define_Modify.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TReticleOnlineCMDForm *ReticleOnlineCMDForm;
TCHKOHT CmdReticleOHTCheck;

// by zzang9un 2013.04.24 : Timer Interval Setting
#define TIMER_INTV_TRANSLONGRUN     50  ///< 이적재 롱런 타이머 Interval

// ---------------------------------------------------------------------------
__fastcall TReticleOnlineCMDForm::TReticleOnlineCMDForm(TComponent *Owner) : TForm(Owner)
{
    timerUpdateDisplay->Enabled   = false;
    nCount                        = 0;
    nCMDCount                     = 0;
    nKeyIndex                     = 0;
	CurJogKey                     = 0;

    pgcMainControl->ActivePageIndex = 0;
    pgcPosVel->ActivePageIndex = 0;
    pgcMainControl->OwnerDraw       = true;

    // by zzang9un 2014.04.07 : 불필요한 UI 숨김
    lbLongrunCount->Visible = false;
    lbLongrunInterval->Visible = false;
    lbTransTT->Visible = false;
    edtLongRunInterval->Visible = false;
}

// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::WMEXITSIZEMOVE(TMessage &msg)
{
    int nTmpLeft, nTmpTop, nTmpRight;

    nTmpLeft  = ReticleOnlineCMDForm->Left;
    nTmpTop   = ReticleOnlineCMDForm->Top;
    nTmpRight = nTmpLeft + ReticleOnlineCMDForm->Width;

    // StatusForm-> MainForm의 왼쪽,,나머지는 모두 아래..
    CmdWinForm->Top  = nTmpTop;
    CmdWinForm->Left = nTmpRight;
}

// ---------------------------------------------------------------------------
TTextAttributes *__fastcall TReticleOnlineCMDForm::CurrText(void)
{
    return PMACMemo->SelAttributes;
}

// ---------------------------------------------------------------------------
/**
 @brief   ReticleOnlineCMDForm의 Show 발생시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::FormShow(TObject *Sender)
{
	Label6->Caption  = "Accel/Decel[m/sec2]";

    timerUpdateDisplay->Enabled = true;

	panName->Caption = "["+SocketForm->m_strOHTName+"]" + " Connect...:" + SocketForm->OHTSocket->Address;

    if(SocketForm->m_strOHTNum.Length() > 2)
        panVer->Font->Size = 32;
    else
        panVer->Font->Size = 48;

    panVer->Caption = SocketForm->m_strOHTNum;

	if(gm_OHTStatus->nProgram_Mode == MODE_OPERATOR)// 근무자
            edtSpeed->ReadOnly = true;
    else
		edtSpeed->ReadOnly = false;

    // by zzang9un 2013.02.08 : Jog 컴포넌트 초기화
    pgcJog->ActivePageIndex = 0;

    // -------------------------------------------
    // by zzang9un 2013.01.22 : Driving 탭 컴포넌트 초기화
    edtStopNodeID->Text           = "";
    chkGoStation->Checked         = false;
    edtStopStationID->Text        = "";
    edtStopStationsOffset->Text   = "";
    edtNextNodeID->Text           = "";
    cbbStopStationType->ItemIndex = -1;

    edtStopStationID->Enabled      = false;
    edtStopStationsOffset->Enabled = false;
    edtNextNodeID->Enabled         = false;
    cbbStopStationType->Enabled    = false;
	// -------------------------------------------
}

// ---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::FormClose(TObject *Sender,
    TCloseAction &Action)
{
    timerAllDisable();
}

// ---------------------------------------------------------------------------
int TReticleOnlineCMDForm::Check_Invalid_Command(String strTmpCMD)
{
    if(strTmpCMD.Length() == 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= ");
        return 1;
    }

    if(strTmpCMD.Pos("{") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= {");
        return 1;
    }

    if(strTmpCMD.Pos("}") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= }");
        return 1;
    }

    if(strTmpCMD.Pos(",") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= ,");
        return 1;
    }

    if(strTmpCMD.Pos("'") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= '");
        return 1;
    }

    if(strTmpCMD.Pos("<") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= <");
        return 1;
    }

    if(strTmpCMD.Pos(">") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= >");
        return 1;
    }

    if(strTmpCMD.Pos("@") != 0)
    {
        Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= @");
        return 1;
    }

    if((strTmpCMD.Pos("[") != 0) || (strTmpCMD.Pos("]") != 0))
    {
        if(gm_OHTStatus->nProgram_Mode == MODE_OPERATOR)// 근무자
        {
            Warnning_Display(COLOR_FUCHSIA, "[Invalid Command]= MOVE CMD[근무자]");
            PMACDataDisplay(COLOR_FUCHSIA, "Command ERROR...");
            return 1;
        }
        else
        {
            if(CmdReticleOHTCheck.Is_Executable
                (EXCLUDE_DRIVING_RUNNING EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) != NO_ERR)
            {
                PMACDataDisplay(COLOR_FUCHSIA, "Command ERROR...");
                return 1;
            }
        }
    }
    return 0;
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMemoClearClick(TObject *Sender)
{
    PMACMemo->Clear();
}

// ---------------------------------------------------------------------------
void TReticleOnlineCMDForm::PMACDataDisplay(TColor TmpFontColor, String strDisplayData)
{
    if(PMACMemo->Lines->Count > 500)
        PMACMemo->Clear();

    CurrText()->Color = TmpFontColor;
    PMACMemo->Lines->Add(strDisplayData);
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand Close버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHandCloseClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_CLOSE);
        MainForm->DataDisplay(COLOR_BLACK, "Hand Close");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand Open버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHandOpenClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

            if(gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == OFF &&
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == OFF)
            {
                m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_OPEN);
                MainForm->DataDisplay(COLOR_BLACK, "Hand Open");
            }
            else if(MessageDlg("FOUP is detected. Really open?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel,
                0) == IDOK)
            {
                m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_OPEN);
                MainForm->DataDisplay(COLOR_BLACK, "Hand Open");
            }

            delete m_CMDControl;
        }
        else
        {
            HandOpenForm->ShowModal();
        }
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Go버튼 클릭 이벤트 발생 시 호출되는 함수
 @author  zzang9un
 @date    2012.12.01
 @param   Sender : TObject
 @note    UI의 값을 Go Command 구조체에 직접 넣어서 생성하도록 변
 */
void __fastcall TReticleOnlineCMDForm::btnGoClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // by zzang9un 2012.12.01 : Go Command 구조체를 직접 전달
        GO_CMD SndGoCmd;
        memset(&SndGoCmd, 0x00, sizeof(GO_CMD));

        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID == 0)
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Current Node is 0");
            return;
        }
        else if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > 100.0)
        {
            Warnning_Display(COLOR_RED, "ERROR : Hoist Not Origin");
            return;
        }
		/* shkim. 2016-03-14 skip modify
        else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
        {
            // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
            Warnning_Display(COLOR_RED, "ERROR : OHT Current Node is zero...");
            return;
        }
        */

        // by zzang9un 2013.02.05 : TransNode로 주행하는 경우만 이적재Node 정보 반영
        if(chkGoStation->Checked)
        {
			SndGoCmd.StopNodeID        = StrToIntDef("0x" + edtStopNodeID->Text, 0);
            SndGoCmd.StopStationID     = StrToIntDef("0x" + edtStopStationID->Text, 0);
            SndGoCmd.StopStationOffset = StrToIntDef(edtStopStationsOffset->Text, 0);
            SndGoCmd.NextNodeID        = StrToIntDef("0x" + edtNextNodeID->Text, 0);
            SndGoCmd.StopStationType   = GetTransNodeType(cbbStopStationType->ItemIndex);
        }
        else
        {
            SndGoCmd.StopNodeID = StrToIntDef("0x" + edtStopNodeID->Text, 0);
        }

        // by zzang9un 2013.04.03 : 선조향 정보 추가 전송(임태웅 요청)
        SndGoCmd.PreSteeringNodeID = StrToIntDef("0x" + edtPreSteeringNodeID->Text, 0);

        if(SndGoCmd.StopNodeID == 0)
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Target Node is 0");
        }
        else if((SndGoCmd.StopStationID != 0) &&
            (SndGoCmd.StopStationID == gm_OHTStatus->m_OHTStatus.Data.CurStationID))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Target Trans Node == Current Trans Node");
        }
        else
        {
            m_CMDControl->SendGoCmd2OHT(TPCMD_GO_TARGETNODE, &SndGoCmd);
        }

        delete m_CMDControl;
	}
}

// ---------------------------------------------------------------------------
/**
 @brief   Driving Stop버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 @param   EStop
 */
void __fastcall TReticleOnlineCMDForm::btnStopClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_ESTOP);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_ESTOP);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Trans(이적재) Mark Front버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnTransMarkForwardClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpCMD;
        int nMarkNum;

        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
		/* shkim. 2016-03-14 skip modify
        else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
        {
            // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
            Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
        }
        */
        else
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_TRANS_MARK_FORWARD);
            delete m_CMDControl;
        }
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Trans(이적재) Mark Back버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnTransMarkBackwardClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpCMD;
        int nMarkNum;

        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
        else
        {
			if((gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left == gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left) &&
				(gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right == gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right))
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_TRANS_MARK_BACKWARD);
                delete m_CMDControl;
            }
            else
            {
                Warnning_Display(COLOR_RED, "ERROR : Steering not same status...");
			}
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::memoJogKeyControlExit(TObject *Sender)
{
    memoJogKeyControl->Color = clMenuHighlight;
    timerDrivingJog->Enabled = false;
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::memoJogKeyControlClick(TObject *Sender)
{
    memoJogKeyControl->Color = COLOR_YELLOW;
}
//---------------------------------------------------------------------------

/**
@brief   JogControl KeyDown시 발생하는 이벤트 함수
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TReticleOnlineCMDForm::memoJogKeyControlKeyDown(TObject *Sender,
    WORD &Key, TShiftState Shift)
{
    if((Key == VK_RIGHT) || (Key == VK_LEFT))
    {
        // 조그 명령 Timer로 반복 실시
		CurJogKey = Key;
//		if((chbBackWard->Checked ==true)&&(Key == VK_LEFT))
//	   {
//		CurJogKey = 0x0000;
//		timerDrivingJog->Enabled = false;
//		}
//		else
//		{
//        timerDrivingJog->Enabled = true;
//		}
		timerDrivingJog->Enabled = true;
    }
    else
    {
        CurJogKey = 0x0000;
        timerDrivingJog->Enabled = false;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   JogControl KeyUP시 발생하는 이벤트 함수
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TReticleOnlineCMDForm::memoJogKeyControlKeyUp(TObject *Sender,
    WORD &Key, TShiftState Shift)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

    // by zzang9un 2013.02.19 : Jog 구조체 사용
    uTP_CMD TPCmd; // TP Jog Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TPCmd.CmdTP_Jog_DRV.Accel    = StrToFloat(edtAcceleration->Text) * 1000;
    TPCmd.CmdTP_Jog_DRV.Decel    = StrToFloat(edtDeceleration->Text) * 1000;
	TPCmd.CmdTP_Jog_DRV.Velocity = StrToFloat(panMPerSec->Caption) * 1000;
	TPCmd.CmdTP_Jog_DRV.bJogInterlockIgnore = true;

    if((Key == VK_RIGHT) || (Key == VK_LEFT))
    {
        MainForm->DataDisplay(COLOR_BLACK, "Driving Stop");
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_STOP, &TPCmd);
        CurJogKey = 0x0000;

        // 반복 명령 Timer 정지
        timerDrivingJog->Enabled = false;
    }
    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

/**
@brief   Diving Jog Timer에 의해 계속 호출되는 이벤트 함수
@author  zzang9un
@date    2013.
@note    Jog 동작을 위해 Key를 입력할 때만 timer가 동작한다.
*/
void __fastcall TReticleOnlineCMDForm::timerDrivingJogTimer(TObject *Sender)
{
    bool bError = NO_ERR;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD SndTPCmd; // TP Command
    memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

    // memo에 focus가 됐는지를 판단하는 함수
    if(memoJogKeyControl->Focused())
    {
        SndTPCmd.CmdTP_Jog_DRV.Accel    = StrToFloat(edtAcceleration->Text) * 1000;
		SndTPCmd.CmdTP_Jog_DRV.Decel    = StrToFloat(edtDeceleration->Text) * 1000;
		SndTPCmd.CmdTP_Jog_DRV.Velocity = StrToFloat(panMPerSec->Caption) * 1000;
		SndTPCmd.CmdTP_Jog_DRV.bJogInterlockIgnore = true;

        if((SndTPCmd.CmdTP_Jog_DRV.Accel == 0.0) ||
            (SndTPCmd.CmdTP_Jog_DRV.Decel == 0.0) ||
            (SndTPCmd.CmdTP_Jog_DRV.Velocity == 0.0))
        {
            Warnning_Display(COLOR_RED, "ERROR : Accel OR Speed is Zero...");
            timerDrivingJog->Enabled = false;
            return;
        }

		if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
		{
			Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
			timerDrivingJog->Enabled = false;
			return;
		}

		if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
			|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
			|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
			|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
		{
			Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
			timerDrivingJog->Enabled = false;
			return;
		}

		if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
			((gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left) ||
			(gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right)))
		{
			Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
			timerDrivingJog->Enabled = false;
			return;
		}

		if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
        {
            if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
            {
                Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
                bError = true;
            }
            else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
            {
                Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
                bError = true;
            }
			/* shkim. 2016-03-14 skip modify
            else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
            {
                // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
                Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
                bError = true;
            }
            */
        }

		if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71)
			|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == HOIST_Home_V85H))
		{
			if(gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect == true)
			{
				if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == true ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == false ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == false ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
					timerDrivingJog->Enabled = false;
					return;
				}

				if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Close == false ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Close == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
					timerDrivingJog->Enabled = false;
					return;
				}
			}
			else
			{
				if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == false ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == true ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == true ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == true)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
					timerDrivingJog->Enabled = false;
					return;

				}

				if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == false ||
					gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
					timerDrivingJog->Enabled = false;
					return;
				}
			}
		}
		else
		{
            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == true ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == true ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                timerDrivingJog->Enabled = false;
                return;
            }

            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                timerDrivingJog->Enabled = false;
                return;
            }
		}

		// 에러 체크
        if(bError)
        {
            timerDrivingJog->Enabled = false;
            return;
        }

        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        switch(CurJogKey)
        {
        case VK_RIGHT:
            MainForm->DataDisplay(COLOR_BLACK, "Driving Forward");
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_FORWARD, &SndTPCmd);
            break;

		case VK_LEFT:
			///300msec 이상 속도의 경우 300msec로 동작 제한, 그 이하는 본래 속도로 동작
			 if((SndTPCmd.CmdTP_Jog_DRV.Velocity > 300))
				  SndTPCmd.CmdTP_Jog_DRV.Velocity = 300;
            MainForm->DataDisplay(COLOR_BLACK, "Driving Backward");
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_BACKWARD, &SndTPCmd);
            break;

        default:
            MainForm->DataDisplay(COLOR_BLACK, "Driving Stop");
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_STOP, &SndTPCmd);
            CurJogKey = 0x0000;

            // 반복 명령 Timer 정지
			timerDrivingJog->Enabled = false;
        }

        delete m_CMDControl;
    }
    else
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        MainForm->DataDisplay(COLOR_BLACK, "Driving Stop");
		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_STOP, &SndTPCmd);
        CurJogKey = 0x0000;

        // 반복 명령 Timer 정지
        timerDrivingJog->Enabled = false;

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   Hand Origin버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHandOriginClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_HOMING);
        MainForm->DataDisplay(COLOR_BLACK, "Hand Origin");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand Reset버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHandResetClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_RESET);
        MainForm->DataDisplay(COLOR_BLACK, "Hand Reset");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand Free버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHandFreeClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(btnHandFree->Caption == "[Hand Free]")
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_FREE);
            MainForm->DataDisplay(COLOR_BLACK, "Hand Reset");
        }
        else
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_ENABLE);
            MainForm->DataDisplay(COLOR_BLACK, "Hand Enable");
        }

        delete m_CMDControl;
    }

}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnSteeringLeftClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        // by zzang9un 2013.06.21 : Steering Left가 Traction으로 되어 있어 변경함
        m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
        MainForm->DataDisplay(COLOR_BLACK, "Steering Left");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnSteeringRightClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        // by zzang9un 2013.06.21 : Steering Left가 Traction으로 되어 있어 변경함
        m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_RIGHT);
        MainForm->DataDisplay(COLOR_BLACK, "Steering Right");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shutter Open버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnShutterOpenClick(TObject *Sender)
{
    // by zzang9un 2013.02.28 : 변수명이 길어서 지역변수로 대입한 후 사용
    bool Grip_R     = gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip;
    bool Grip_L     = gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip;
    bool FoupDetect = gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect;

    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == ON)
        {
            if((Grip_L == OFF) && (Grip_R == OFF) && (FoupDetect == OFF))
            {
                // by zzang9un 2013.02.28 : Hand가 Open되고 반송물이 없는 경우는 무조건 Open
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_OPEN);
                MainForm->DataDisplay(COLOR_BLACK, "Shutter Open");
                delete m_CMDControl;
            }
            else if(MessageDlg("Check Foup!! Really open?",
                mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_OPEN);
                MainForm->DataDisplay(COLOR_BLACK, "Shutter Open");
                delete m_CMDControl;
            }
        }
        else if(MessageDlg("Hand Close!! Check Foup!!\nReally open?",
            mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_OPEN);
            MainForm->DataDisplay(COLOR_BLACK, "Shutter Open");
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shutter Close버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnShutterCloseClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...Shutter can't Close...");
        }
        else if(fabs(StrToFloat(gm_OHTStatus->Motor_Shift->dPosition_cts)) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...Shutter can't Close...");
        }
        else if((fabs(StrToFloat(gm_OHTStatus->Motor_Rotate->dPosition_mm)) != 0) &&
                (fabs(StrToFloat(gm_OHTStatus->Motor_Rotate->dPosition_mm)) != 1800))
        {
            Warnning_Display(COLOR_RED, "Rotation Not 0 or 180 degree...Shutter can't Close...");
        }
        else
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_CLOSE);
            MainForm->DataDisplay(COLOR_BLACK, "Shutter Close");
        }

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
@brief   Shutter Origin버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnShutterOriginClick(TObject *Sender)
{
    // by zzang9un 2013.04.04 : Origin 명령 시에도 경고창 뜨도록 변경(천광현 책임 요청)
    bool Grip_R     = gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip;
    bool Grip_L     = gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip;
    bool FoupDetect = gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect;

    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == ON)
        {
            if((Grip_L == OFF) && (Grip_R == OFF) && (FoupDetect == OFF))
            {
                // by zzang9un 2013.04.04 : Hand가 Open되고 반송물이 없는 경우는 무조건 Origin
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_ORIGIN);
                MainForm->DataDisplay(COLOR_BLACK, "Shutter Origin");
                delete m_CMDControl;
            }
            else if(MessageDlg("Check Foup!! Really Origin?",
                mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_ORIGIN);
                MainForm->DataDisplay(COLOR_BLACK, "Shutter Origin");
                delete m_CMDControl;
            }
        }
        else if(MessageDlg("Hand Close!! Check Foup!!\nReally Origin?",
            mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_ORIGIN);
            MainForm->DataDisplay(COLOR_BLACK, "Shutter Origin");
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shutter Free버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnShutterFreeClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(btnShutterFree->Caption == "[Free]")
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_FREE);
            MainForm->DataDisplay(COLOR_BLACK, "Shutter Free");
        }
        else
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_ENABLE);
            MainForm->DataDisplay(COLOR_BLACK, "Shutter Enable");
        }
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
int TReticleOnlineCMDForm::Set_UBGColor(int nOutput)
{
    int nError = 0;

    if(nOutput == -1)// 고장.
    {
        panUBG_FAIL->Color = COLOR_RED;
    }
    else if(nOutput == 0)// 미감지
    {
        panUBG_FAIL->Caption = 8;
        panUBG_FAIL->Color   = COLOR_WHITE;
        panUBG_OUT1->Color   = COLOR_WHITE;
        panUBG_OUT2->Color   = COLOR_WHITE;
        panUBG_OUT3->Color   = COLOR_WHITE;
        panUBG_OUT4->Color   = COLOR_WHITE;
        panUBG_OUT5->Color   = COLOR_WHITE;
        panUBG_OUT6->Color   = COLOR_WHITE;

    }
    else
    {
        panUBG_FAIL->Color = COLOR_WHITE;
        panUBG_OUT1->Color = (nOutput == 1) ? COLOR_LIME : COLOR_WHITE;
        panUBG_OUT2->Color = (nOutput == 2) ? COLOR_LIME : COLOR_WHITE;
        panUBG_OUT3->Color = (nOutput == 3) ? COLOR_LIME : COLOR_WHITE;
        panUBG_OUT4->Color = (nOutput == 4) ? COLOR_LIME : COLOR_WHITE;
        panUBG_OUT5->Color = (nOutput == 5) ? COLOR_LIME : COLOR_WHITE;
        panUBG_OUT6->Color = (nOutput == 6) ? COLOR_LIME : COLOR_WHITE;

        if(panUBG_OUT1->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 7;
        else if(panUBG_OUT2->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 6;
        else if(panUBG_OUT3->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 5;
        else if(panUBG_OUT4->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 4;
        else if(panUBG_OUT5->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 3;
        else if(panUBG_OUT6->Color == COLOR_LIME)
            panUBG_FAIL->Caption = 2;
    }

    return nError;
}
// ---------------------------------------------------------------------------

/**
 @brief   ReticleOnlineCMDForm UI를 주기적으로 갱신하는 Timer 이벤트 함수
 @author  zzang9un
 @date    2013.03.29
 */
void __fastcall TReticleOnlineCMDForm::timerUpdateDisplayTimer(TObject *Sender)
{
    String strVelData;
    double dMotor4Pos, dMotor1Pos;
    String strM4Data, strM1Data;
    String strLR, strUR, strRE, strHO, strES, strEQPIO, strAB;
    String strCS, strVA, strTR, strBU, strCO, strCS1, strCS2, strCS3;
	String strData, strStrTorque;

	//TabSheet1->Visible = false;	// 구 driving 탭을 숨김 (새 프로토콜 적용)
	//TabSheet2->Visible = false; // 구 Trans 탭을 숨김 (새 프로토콜 적용)

    if(panUBGMiddle->Visible == true)
        panUBGMiddle->Visible = false;

    if(panUBGShort->Visible == true)
        panUBGShort->Visible = false;

    if(panUBGFail->Visible == true)
        panUBGFail->Visible = false;

    if(gm_OHTStatus->nIsUBGSenUse == 0)
    {
        if(panUBG->Visible == true)
            panUBG->Visible = false;
        if(panUBG_FAIL->Visible == true)
            panUBG_FAIL->Visible = false;
        if(panDetect->Visible == false)
            panDetect->Visible = true;
        if(lblDetect->Visible == false)
            lblDetect->Visible = true;
    }
    else
    {
        if(panUBG->Visible == false)
            panUBG->Visible = true;
        if(panUBG_FAIL->Visible == false)
            panUBG_FAIL->Visible = true;
        Set_UBGColor(gm_OHTStatus->nUBGDetectLevel);
        if(panDetect->Visible == false)
            panDetect->Visible = true;
        if(lblDetect->Visible == true)
            lblDetect->Visible = false;
    }

	if(gm_OHTStatus->nIsRotateExist == 1)
    {
        if(tabRotate->TabVisible == false)
            tabRotate->TabVisible = true;
		if(tabRotate2->TabVisible == true)
			tabRotate2->TabVisible = false;
        if(lblM4Origin->Visible == false)
            lblM4Origin->Visible = true;
        if(lblM4Caption->Visible == false)
            lblM4Caption->Visible = true;
        if(panAx4_ServoON->Visible == false)
            panAx4_ServoON->Visible = true;
        if(panPosition_Rotate->Visible == false)
            panPosition_Rotate->Visible = true;
        if(panAx4_Origin->Visible == false)
            panAx4_Origin->Visible = true;
        if(RotateOriginLt->Visible == true)
            RotateOriginLt->Visible = false;
        if(Rotate0Lt->Visible == true)
            Rotate0Lt->Visible = false;
        if(Rotate90Lt->Visible == true)
            Rotate90Lt->Visible = false;
        if(Rotate180Lt->Visible == true)
            Rotate180Lt->Visible = false;
        if(Rotate270Lt->Visible == true)
            Rotate270Lt->Visible = false;
        if(RotateInposLt->Visible == true)
            RotateInposLt->Visible = false;
        if(RotateAlarmLt->Visible == true)
            RotateAlarmLt->Visible = false;
        if(Label57->Visible == true)
            Label57->Visible = false;

    }
    else
    {
        if(tabRotate->TabVisible == true)
            tabRotate->TabVisible = false;
		if(tabRotate2->TabVisible == true)
			tabRotate2->TabVisible = false;
        if(lblM4Origin->Visible == true)
            lblM4Origin->Visible = false;
        if(lblM4Caption->Visible == true)
            lblM4Caption->Visible = false;
        if(panAx4_ServoON->Visible == true)
            panAx4_ServoON->Visible = false;
        if(panPosition_Rotate->Visible == true)
            panPosition_Rotate->Visible = false;
        if(panAx4_Origin->Visible == true)
            panAx4_Origin->Visible = false;
        if(RotateOriginLt->Visible == true)
            RotateOriginLt->Visible = false;
        if(Rotate0Lt->Visible == true)
            Rotate0Lt->Visible = false;
        if(Rotate90Lt->Visible == true)
            Rotate90Lt->Visible = false;
        if(Rotate180Lt->Visible == true)
            Rotate180Lt->Visible = false;
        if(Rotate270Lt->Visible == true)
            Rotate270Lt->Visible = false;
        if(RotateInposLt->Visible == true)
            RotateInposLt->Visible = false;
        if(RotateAlarmLt->Visible == true)
            RotateAlarmLt->Visible = false;
        if(Label57->Visible == true)
            Label57->Visible = false;
    }

    if(gm_OHTStatus->nIsUBGSenUse == 1)
    {
        if(MainForm->btnPause_SetReset->Visible == false)
            MainForm->btnPause_SetReset->Visible = true;
        if(gm_OHTStatus->nIsPauseSet == 1)
        {
            if(MainForm->btnPause_SetReset->Caption != "Pause:Reset")
            {
                MainForm->btnPause_SetReset->Font->Color = COLOR_FUCHSIA;
                MainForm->btnPause_SetReset->Caption     = "Pause:Reset";
            }
        }
        else
        {
            if(MainForm->btnPause_SetReset->Caption != "Pause:Set")
            {
                MainForm->btnPause_SetReset->Font->Color = COLOR_BLACK;
                MainForm->btnPause_SetReset->Caption     = "Pause:Set";
            }
        }
    }
    else
	{
	   	// Pause button이 안보여 우선 주석 처리
		/*
		if(MainForm->btnPause_SetReset->Visible == true)
			MainForm->btnPause_SetReset->Visible = false;
		*/
    }

    if((gm_OHTStatus->strOHTVer != OHTVER_V10LP) && (gm_OHTStatus->strOHTVer != OHTVER_V20_MZ))
    {
        if(panLookdown->Visible == false)
            panLookdown->Visible = true;
    }
    else
    {
        if(panLookdown->Visible == true)
            panLookdown->Visible = false;
    }

    // by zzang9un 2012.11.30 : 모터 엔코더값 출력 부분
    if(pgcPosVel->ActivePageIndex == 0)
    {
        panPosition_Driving_F->Caption = gm_OHTStatus->Motor_Driving_F->dPosition_mm;
        panPosition_Driving_R->Caption = gm_OHTStatus->Motor_Driving_R->dPosition_mm;
        panPosition_Hoist->Caption     = gm_OHTStatus->Motor_Hoist->dPosition_mm;
        panPosition_Shift->Caption     = gm_OHTStatus->Motor_Shift->dPosition_mm;
        panPosition_Rotate->Caption    = gm_OHTStatus->Motor_Rotate->dPosition_mm;

        shpRun_Driving_F->Brush->Color = (gm_OHTStatus->Motor_Driving_F->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
        shpRun_Driving_R->Brush->Color = (gm_OHTStatus->Motor_Driving_R->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
        shpRun_Hoist->Brush->Color     = (gm_OHTStatus->Motor_Hoist->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
        shpRun_Shift->Brush->Color     = (gm_OHTStatus->Motor_Shift->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
        shpRun_Rotate->Brush->Color    = (gm_OHTStatus->Motor_Rotate->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;

        panAx1_F_ServoON->Color = (gm_OHTStatus->Motor_Driving_F->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAx1_R_ServoON->Color = (gm_OHTStatus->Motor_Driving_R->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAx2_ServoON->Color   = (gm_OHTStatus->Motor_Hoist->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAx3_ServoON->Color   = (gm_OHTStatus->Motor_Shift->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAx4_ServoON->Color   = (gm_OHTStatus->Motor_Rotate->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAxis4_Enable->Color  = (gm_OHTStatus->Motor_Rotate->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;

        panAx4_Origin->Color   = (gm_OHTStatus->Motor_Rotate->bIs_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;
        panAxis4_Origin->Color = (gm_OHTStatus->Motor_Rotate->bIs_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;
    }
    else
    {
        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_F->dVelocity_rpm);
        lblVel1_F_rpm->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec);
        lblVel1_F_mpersec->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_R->dVelocity_rpm);
        lblVel1_R_rpm->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_R->dVelocity_mpersec);
        lblVel1_R_mpersec->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Hoist->dVelocity_rpm);
        lblVel2_rpm->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Shift->dVelocity_rpm);
        lblVel3_rpm->Caption = strData;

        strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Rotate->dVelocity_rpm);
        lblVel4_rpm->Caption = strData;
    }

    // **  Tag **// puting 2012.11.28

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_RETICLE) && 
		((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == HOIST_Home_V85H)))
	    shpF_DrivingSen->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STOP_Driving_Front == ON) ? COLOR_LIME : COLOR_WHITE;
	else
	    shpF_DrivingSen->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STOP_Driving_Front == ON) ? COLOR_LIME : COLOR_WHITE;

    shpBCRDriving->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.BCR_Driving_OK == ON) ? COLOR_LIME : COLOR_WHITE;
    shpBCRDriving->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.BCR_Driving_NG == ON) ? COLOR_RED :
        shpBCRDriving->Brush->Color;

    shpF_TransSen->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STOP_Trans_Front == ON) ? COLOR_LIME : COLOR_WHITE;
    shpR_TransSen->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STOP_Trans_Rear == ON) ? COLOR_LIME : COLOR_WHITE;
    shpBCRTransLt->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.BCR_Trans_OK == ON) ? COLOR_LIME : COLOR_WHITE;
    shpBCRTransLt->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.BCR_Trans_NG == ON) ? COLOR_RED :
        shpBCRTransLt->Brush->Color;

    // by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
    // RStopDetectPan->Brush->Color = (gm_OHTStatus->IOInput->nRear_StopSensor== 1) ? COLOR_LIME : COLOR_WHITE;

    // ** Hand ***
    // input
    HandOpenLt->Brush->Color    = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == ON) ? COLOR_LIME : COLOR_WHITE;
    HandInPosLt->Brush->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
    HandCloseLt->Brush->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == ON) ? COLOR_LIME : COLOR_WHITE;
    shpHandPushLt->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Push == ON) ? COLOR_LIME : COLOR_WHITE;
    HandAlarmLt->Brush->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
    GLLt->Brush->Color          = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == ON) ? COLOR_LIME : COLOR_WHITE;
    GRLt->Brush->Color          = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == ON) ? COLOR_LIME : COLOR_WHITE;
    HandOriginLt->Color         = (gm_OHTStatus->bIs_Hand_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;
    HandGoLt->Color             = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_PIO_Go == 1) ? COLOR_LIME : COLOR_FUCHSIA;
    // output
	grpHand->Color       = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.HAND_Free == ON) ? COLOR_RED : (TColor)0x00FFDFDF;
	btnHandFree->Caption = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.HAND_Free == ON) ? "[Hand Enable]" : "[Hand Free]";

    // ** Steering **
    // input
    FStrLeftLt->Brush->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left == ON) ? COLOR_LIME : COLOR_WHITE;
	FStrRightLt->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right == ON) ? COLOR_LIME : COLOR_WHITE;
    RStrLeftLt->Brush->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left == ON) ? COLOR_LIME : COLOR_WHITE;
    RStrRightLt->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right == ON) ? COLOR_LIME : COLOR_WHITE;
    // output
	strStrTorque.sprintf(L"%d %d", gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_F_TRQ_0,
		gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_F_TRQ_1);
    panFStrTorque->Caption = strStrTorque;
	strStrTorque.sprintf(L"%d %d", gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_R_TRQ_0,
		gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_R_TRQ_1);
    panRStrTorque->Caption = strStrTorque;

    // ** Shutter **
    shpShutterOpen_Front->Brush->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == ON) ? COLOR_LIME : COLOR_WHITE;
    shpShutterOpen_Rear->Brush->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == ON) ? COLOR_LIME : COLOR_WHITE;
    shpShutterClose_Front->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Close == ON) ? COLOR_LIME : COLOR_WHITE;
    shpShutterClose_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Close == ON) ? COLOR_LIME : COLOR_WHITE;
    shpShutterAlarm_Front->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
    shpShutterAlarm_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
    shpShutterInposition_Front->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
    shpShutterInposition_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
    panShtrOrigin->Color      = (gm_OHTStatus->bIs_Shutter_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;

    grpShutter->Color = ((gm_OHTStatus->IOOutput->uOutput.Output_Reticle.SHUTTER_F_Free == ON) ||
        (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.SHUTTER_R_Free == ON)) ? COLOR_RED : (TColor)0x00FFDFDF;
    btnShutterFree->Caption = ((gm_OHTStatus->IOOutput->uOutput.Output_Reticle.SHUTTER_F_Free == ON) ||
        (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.SHUTTER_R_Free == ON)) ? "[Enable]" : "[Free]";

	// ---- shkim. 2016-03-11 I/O info. add.
#if 0
	// (1) Out trigger
	shpOuttrigger_F_Alarm->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Outtrigger_F_Alarm_IN == ON) ? COLOR_LIME : COLOR_WHITE;
	shpOuttrigger_F_Home->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Outtrigger_F_Home_IN  == ON) ? COLOR_LIME : COLOR_WHITE;
	shpOuttrigger_F_Work->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Outtrigger_F_Work_IN  == ON) ? COLOR_LIME : COLOR_WHITE;

	shpOuttrigger_R_Alarm->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Outtrigger_R_Alarm_IN == ON) ? COLOR_LIME : COLOR_WHITE;
	shpOuttrigger_R_Home->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home_V81  == ON) ? COLOR_LIME : COLOR_WHITE;
	shpOuttrigger_R_Work->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Outtrigger_R_Work_IN  == ON) ? COLOR_LIME : COLOR_WHITE;

	// (2) CID
	shpCID_Link_Compt->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Link_Compt_IN == ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Interlock->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Interlock_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Pass_OK->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Pass_OK_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;

	shpCID_Status->Brush->Color		= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Status_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_1->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Reserved_IN1 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_2->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Reserved_IN2 	== ON) ? COLOR_LIME : COLOR_WHITE;

	// (3) Rotate
	shpRotate_L_Limit->Brush->Color		= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Rotate_L_Limit 		== ON) ? COLOR_LIME : COLOR_WHITE;
	shpRotate_R_Limit->Brush->Color		= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Rotate_R_Limit 		== ON) ? COLOR_LIME : COLOR_WHITE;
	shpRotate_Inposition->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Rotate_Inposition 		== ON) ? COLOR_LIME : COLOR_WHITE;
	shpRotate_Alarm->Brush->Color		= (gm_OHTStatus->IOInput->uInput.Input_Reticle.Rotate_Alarm 			== ON) ? COLOR_LIME : COLOR_WHITE;
#endif

	// (2) CID
	shpCID_Link_Compt->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Link_Compt_IN == ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Interlock->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Interlock_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Pass_OK->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Pass_OK_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;

	shpCID_Status->Brush->Color		= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Status_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_1->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Reserved_IN1 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_2->Brush->Color	= (gm_OHTStatus->IOInput->uInput.Input_Reticle.CID_Reserved_IN2 	== ON) ? COLOR_LIME : COLOR_WHITE;

    // [END] by zzang9un 2013.09.06 : Track Buffer UI
    // -------------------------------------------

    // ** ETC **
    panHoist_Home->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home == ON) ? COLOR_LIME : COLOR_WHITE;
    panOscillation->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OSCILLATION == ON) ? COLOR_LIME : COLOR_WHITE;
    // if((gm_OHTStatus->strOHTVer!=OHTVER_V10LP)&&(gm_OHTStatus->strOHTVer!=OHTVER_V20_MZ))
    // //수정 puting 2012.11.27
    // {
    panLookdown->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.LOOKDOWN_Detect == ON) ? COLOR_LIME : COLOR_WHITE;
    panLookdown->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.LOOKDOWN_Error == ON) ? COLOR_RED : panLookdown->Color;

    // }
    panOBSLong->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OBSTACLE_Long == ON) ? COLOR_LIME : COLOR_WHITE;
    panOBSMiddle->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OBSTACLE_Middle == ON) ? COLOR_LIME : COLOR_WHITE;
    panOBSShort->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OBSTACLE_Short == ON) ? COLOR_LIME : COLOR_WHITE;
    panOBSFail->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OBSTACLE_Fail == ON) ? COLOR_RED : COLOR_WHITE;

    // 추가 puting 2012.11.27
    panHand_TP_Con->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.HANDTP_Connect == ON) ? COLOR_LIME : COLOR_WHITE;

//	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_RETICLE) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
//	    panMC_Status->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.MC_Status == ON) ? COLOR_LIME : COLOR_WHITE;
//	else
//	    panMC_Status->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.MC_Status == ON) ? COLOR_LIME : COLOR_WHITE;

    // 추가 puting 2012.11.27
    if(panUBGMiddle->Visible == false)
        panUBGMiddle->Visible = true;
    if(panUBGShort->Visible == false)
        panUBGShort->Visible = true;
    if(panUBGFail->Visible == false)
        panUBGFail->Visible = true;
    panUBGLong->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OHTDETECT_Long == ON) ? COLOR_LIME : COLOR_WHITE;
    panUBGMiddle->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OHTDETECT_Middle == ON) ? COLOR_LIME : COLOR_WHITE;
    panUBGShort->Color  = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OHTDETECT_Short == ON) ? COLOR_LIME : COLOR_WHITE;
    panUBGFail->Color   = (gm_OHTStatus->IOInput->uInput.Input_Reticle.OHTDETECT_Fail == ON) ? COLOR_RED : COLOR_WHITE;

    // 수정 puting 2012.11.27
    if(panFoupDetect->Visible == false)
        panFoupDetect->Visible = true;
    panFoupDetect->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect == ON) ? COLOR_LIME : COLOR_WHITE;

    if(panShift_Home->Visible == false)
        panShift_Home->Visible = true;
    panShift_Home->Color = (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHIFT_Home == ON) ? COLOR_LIME : COLOR_WHITE;


    // HID 정보 수정
	lblP1->Caption = IntToStr(gm_OHTStatus->m_OHTStatus.HID_PickupTemper); // pickup온도
	lblP2->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.CPUTemperature); // IPC온도
    panEdc->Caption  = IntToStr(gm_OHTStatus->m_OHTStatus.HID_Edc);
    panErec->Caption = IntToStr(gm_OHTStatus->m_OHTStatus.HID_Erec);

    panM1_F->Color = (gm_OHTStatus->Motor_Driving_F->nError == ON) ? COLOR_RED : COLOR_WHITE;
    panM1_R->Color = (gm_OHTStatus->Motor_Driving_R->nError == ON) ? COLOR_RED : COLOR_WHITE;
    panM2->Color   = (gm_OHTStatus->Motor_Hoist->nError == ON) ? COLOR_RED : COLOR_WHITE;
    panM3->Color   = (gm_OHTStatus->Motor_Shift->nError == ON) ? COLOR_RED : COLOR_WHITE;
    panM4->Color   = (gm_OHTStatus->Motor_Rotate->nError == ON) ? COLOR_RED : COLOR_WHITE;

    if((gm_OHTStatus->strOHTVer == OHTVER_V65_U) ||
		(gm_OHTStatus->strOHTVer == OHTVER_V65_S) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RU) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RS) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_FOSB))
    {
        strData               = gm_OHTStatus->HIDData->strErrString;
        lblErrString->Caption = strData.Delete(1, 1); // V6.0과 호환성을위해서 3자리로 맞춘것을 다시 2자리로 바꿈(맨 앞자리 == 0)
        if(lblKw->Caption != "Ecp:")
            lblKw->Caption = "Ecp:";
		panKw->Caption = IntToStr(gm_OHTStatus->HIDData->nKw); // Ecap사용
    }
    else
    {
        if(lblKw->Caption != "Kw :")
            lblKw->Caption = "Kw :";
		panKw->Caption = FloatToStr((float)gm_OHTStatus->m_OHTStatus.HID_Kw * 0.001);
        strData.sprintf(L"%d %d%d%d%d %d%d%d%d",
            gm_OHTStatus->HIDData->nOverOutput_KW,
            gm_OHTStatus->HIDData->nErrStorage,
            gm_OHTStatus->HIDData->nErecUnderVoltage,
            gm_OHTStatus->HIDData->nErecOverVoltage,
            gm_OHTStatus->HIDData->nIdcOverCurrent,
            gm_OHTStatus->HIDData->nEdcUnderVoltage,
            gm_OHTStatus->HIDData->nEdcOverVoltage,
            gm_OHTStatus->HIDData->nOverTemperature,
            gm_OHTStatus->HIDData->nFuseOpen
            );
        lblErrString->Caption = strData;
    }
    panHIDError->Color = (strData.Pos("1") != 0) ? COLOR_RED : clBtnFace;

    // ** Power ON/OFF
    int j                 = 0;
    PWGrid->Cells[1][j++] = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.HAND_Power_Reset == ON) ? "OFF" : "ON";
    // *** Hand ***
    if(gm_OHTStatus->IOOutput->uOutput.Output_Reticle.HAND_Power_Reset == 1)
    {
        btnHaON->Enabled  = true;
        btnHaOFF->Enabled = false;
    }
    else
    {
        btnHaON->Enabled  = false;
        btnHaOFF->Enabled = true;
    }

    int nTr1, nTr2;
	nTr1 = gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_F_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_F_TRQ_1;

	// 2017.01.04.  TRQ_1?? ?????Ϡǘ?ߠǑ?ٍ
    // if((nTr1 == 1) && (nTr2 == 1))
    if(nTr2 == 0)
    {
        btnFSteerON->Enabled  = true;
        btnFSteerOFF->Enabled = false;
    }
    else
    {
        btnFSteerON->Enabled  = false;
        btnFSteerOFF->Enabled = true;
    }
	nTr1 = gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_R_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutput.Output_Reticle.STEER_R_TRQ_1;

	// 2017.01.04.  TRQ_1?? ?????Ϡǘ?ߠǑ?ٍ
    // if((nTr1 == 1) && (nTr2 == 1))
    if (nTr2 == 0)
    {
        btnRSteerON->Enabled  = true;
        btnRSteerOFF->Enabled = false;
    }
    else
    {
        btnRSteerON->Enabled  = false;
        btnRSteerOFF->Enabled = true;
    }

    strLR = (gm_OHTStatus->IOInput->uInput.Input_Reticle.EQPIO_Load_REQ == ON) ? "L" : "0";
    strUR = (gm_OHTStatus->IOInput->uInput.Input_Reticle.EQPIO_Unload_REQ == ON) ? "U" : "0";
    strRE = (gm_OHTStatus->IOInput->uInput.Input_Reticle.EQPIO_Ready == ON) ? "R" : "0";
    if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E84)
    {
        strHO = (gm_OHTStatus->IOInput->uInput.Input_Reticle.EQPIO_HO_AVBL == ON) ? "H" : "0";
        strES = (gm_OHTStatus->IOInput->uInput.Input_Reticle.EQPIO_ES == ON) ? "E" : "0";

        strEQPIO.sprintf(L"%s %s %s %s %s", strLR, strUR, strRE, strHO, strES);
	}
    else
	{
        // by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
        // strAB = (gm_OHTStatus->IOInput->nPIO_ABORT==ON) ? "A" : "0";
        // strEQPIO.sprintf(L"%s %s %s %s", strLR, strUR, strAB, strRE);
    }

    panEQPIOIn->Caption = strEQPIO;

    strCS = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_CS_0 == ON) ? "C" : "0";
    strVA = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_Valid == ON) ? "V" : "0";
	strTR = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_TR_Req == ON) ? "T" : "0";
    strBU = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_Busy == ON) ? "B" : "0";
    strCO = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_Complete == ON) ? "C" : "0";
    if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E23)
    {
		strCS1 = (gm_OHTStatus->IOOutput->uOutput.Output_Reticle.EQPIO_CS_1 == ON) ? "1" : "0";
        // strCS2 = (gm_OHTStatus->IOOutput->nPIO_CS_2==ON) ? "2" : "0";
        // strCS3 = (gm_OHTStatus->IOOutput->nPIO_CS_3==ON) ? "3" : "0";
    }

    if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E84)
        strEQPIO.sprintf(L"%s %s %s %s %s", strCS, strVA, strTR, strBU, strCO);
    else
        strEQPIO.sprintf(L"%s %s %s %s %s %s %s %s", strCS, strVA, strTR, strBU, strCO, strCS1, strCS2, strCS3);
    panEQPIOOut->Caption = strEQPIO;

    panState_F->Caption = String().sprintf(L"%d", gm_OHTStatus->Motor_Driving_F->nAxis_state_code);
    panState_R->Caption = String().sprintf(L"%d", gm_OHTStatus->Motor_Driving_R->nAxis_state_code);
    panState_H->Caption = String().sprintf(L"%d", gm_OHTStatus->Motor_Hoist->nAxis_state_code);
    panState_S->Caption = String().sprintf(L"%d", gm_OHTStatus->Motor_Shift->nAxis_state_code);

    panSource_F->Caption = String().sprintf(L"0x%04x", gm_OHTStatus->Motor_Driving_F->nAxis_source_code);
    panSource_R->Caption = String().sprintf(L"0x%04x", gm_OHTStatus->Motor_Driving_R->nAxis_source_code);
    panSource_H->Caption = String().sprintf(L"0x%04x", gm_OHTStatus->Motor_Hoist->nAxis_source_code);
    panSource_S->Caption = String().sprintf(L"0x%04x", gm_OHTStatus->Motor_Shift->nAxis_source_code);

    if(gm_OHTStatus->IOInput->uInput.Input_Reticle.FAN_Fail)
    {
        panName->Color   = COLOR_RED;
        panName->Caption = "[Warnning] Fan Fail";
    }
    else
    {
        panName->Color   = COLOR_BLUE;
		panName->Caption = "["+SocketForm->m_strOHTName+"]" + " Connect...:" + SocketForm->OHTSocket->Address;
    }

    // by zzang9un 2013.03.29 : Mark 정보 표시
    if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == 1)
    {
        panQRMark->Color   = COLOR_LIME;
        panQRMark->Caption = "Driving Marked";
    }
    else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag == 1)
    {
        panQRMark->Color   = COLOR_LIME;
        panQRMark->Caption = "Station Marked";
    }
    else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L == 1)
    {
        panQRMark->Color   = COLOR_LIME;
        panQRMark->Caption = "QR Left Marked";
    }
    else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R == 1)
    {
        panQRMark->Color   = COLOR_LIME;
        panQRMark->Caption = "QR Right Marked";
    }
    else
    {
        // Mark가 안된 경우 표시
        panQRMark->Color   = COLOR_WHITE;
        panQRMark->Caption = "";
    }

    // by zzang9un 2013.11.21 : Jog key Memo 색상 변경
    if(memoJogKeyControl->Focused())
        memoJogKeyControl->Color = COLOR_YELLOW;
    else
        memoJogKeyControl->Color = clMenuHighlight;

    // 2017-01-10, Add by Labeler, Pause 상태 표시 추가
    panTpPause->Color = (gm_OHTStatus->m_OHTStatus.IsTPPause == ON) ? COLOR_LIME : COLOR_WHITE;
    panOcsPause->Color = (gm_OHTStatus->m_OHTStatus.IsOCSPause == ON) ? COLOR_LIME : COLOR_WHITE;
    panCidPause->Color = (gm_OHTStatus->m_OHTStatus.IsCIDPause == ON) ? COLOR_LIME : COLOR_WHITE;

	if(gm_OHTStatus->m_OHTStatus.IsOCSPause == ON)
		MainForm->btnPause_SetReset->Caption = "Pause:Reset";
	else
		MainForm->btnPause_SetReset->Caption = "Pause:Set";

    // 2017-01-10, Add by Labeler, 통과 허가 상태 표시 추가
    panPassPermit->Color = (gm_OHTStatus->m_OHTStatus.IsPassPermit > 0) ? COLOR_LIME : COLOR_WHITE;
}
// ---------------------------------------------------------------------------

/**
@brief   Trans(이적재) Mark BCR버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnStopTagBCRClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_TRANS_SETCURNODE);
        delete m_CMDControl;

    }

}

// ---------------------------------------------------------------------------
/**
@brief   Driving #2HOME버튼 클릭시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::btnMotor2HomeClick(TObject *Sender)
{
    CmdReticleOHTCheck.Axis_Hoist_HOME();
}
// ---------------------------------------------------------------------------

/*
i226 = 3000 * 16 값 대입 -> 만약 i226이 zero가 아니면 기존값에 + 또는 -함
i226 = (M262/3072) * 16
*/
void __fastcall TReticleOnlineCMDForm::mniHoistOriginSetClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_ORIGIN_SETTING);
        MainForm->DataDisplay(COLOR_BLACK, "CMD Hoist Origin Set");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniShiftOriginSetClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_ORIGIN_SETTING);
        MainForm->DataDisplay(COLOR_BLACK, "CMD Shift Origin Set");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Edit창(Speed) Change시 발생하는 이벤트 함수
@author  puting
@date    2012.12.14
*/
void __fastcall TReticleOnlineCMDForm::edtSpeedChange(TObject *Sender)
{
	double dVel_ctspermsec, dVel_mpersec, dVel_Cen_mpersec, dVel_mpermin;
    double d1RotateDist_mm = 0.0;
    if(edtSpeed->Text == "")
        return;

    if((gm_OHTStatus->strOHTVer == OHTVER_V5_U) || (gm_OHTStatus->strOHTVer == OHTVER_V5_S))
        d1RotateDist_mm = DIST_1ROTATE_MM;
    else if((gm_OHTStatus->strOHTVer == OHTVER_V65_U) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_S) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RU) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RS) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V10LP) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_FOSB) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V20_MZ))
        d1RotateDist_mm = DISTV65_1ROTATE_MM;
    else
        d1RotateDist_mm = DISTV6_1ROTATE_MM;

    TCONVERSION *m_Conversion = new TCONVERSION;

    dVel_ctspermsec = StrToFloat(edtSpeed->Text);

    if(dVel_ctspermsec < 0.0)
        dVel_ctspermsec = 0.0;

    dVel_mpersec = m_Conversion->CTSperMSEC_to_MperSEC(dVel_ctspermsec, COUNT_1ROTATE, d1RotateDist_mm);

    if(gm_OHTStatus->nProgram_Mode == MODE_OPERATOR)// 근무자
    {
        if(dVel_mpersec > JOG_MAX_SPEED_MODELEACHING)
        {
            dVel_mpersec   = 0.0;
            edtSpeed->Text = FloatToStr(m_Conversion->MperSEC_to_CTSperMSEC(dVel_mpersec, COUNT_1ROTATE,
                d1RotateDist_mm));
        }
    }
    else
    {
        if(dVel_mpersec > JOG_MAX_SPEED_MODE_SETUP)
        {
            dVel_mpersec   = 0.0;
            edtSpeed->Text = FloatToStr(m_Conversion->MperSEC_to_CTSperMSEC(dVel_mpersec, COUNT_1ROTATE,
                d1RotateDist_mm));
        }
    }

    dVel_Cen_mpersec = dVel_mpersec * 1.377;
    dVel_mpermin = dVel_mpersec * 60.0;

	panMPerSec->Caption = String().sprintf(L"%.02f", dVel_mpersec);
    panMPerMin->Caption = String().sprintf(L"%.02f", dVel_mpermin);
	panCenter->Caption  = String().sprintf(L"%.02f", dVel_Cen_mpersec);

    delete m_Conversion;
}

// ---------------------------------------------------------------------------
/**
 @brief   MOVE TA #2,3,4 STOP버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnMoveStopClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ALLROBOT_STOP);
        MainForm->DataDisplay(COLOR_BLACK, "JOG_ALLROBOT_STOP");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::FormCreate(TObject *Sender)
{
    int j                         = 0;
    ReticleOnlineCMDForm->Top            = MainForm->Top;
    ReticleOnlineCMDForm->Left           = MainForm->Left + MainForm->Width + StatusForm->Width;
    PWGrid->Cells[0][j++]         = "Steering Power[M221]";
    PWGrid->Cells[0][j++]         = "Shutter Power[M222]";
    PWGrid->Cells[0][j++]         = "Hand Power[M223]";
    chkLongRun->Checked           = false;
    chkDist->Checked              = false;
    pgcMainControl->ActivePageIndex = 0;

    timerAllDisable();
}

// ---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::pgcMainControlDrawTab(
    TCustomTabControl *Control, int TabIndex, const TRect &Rect,
    bool Active)
{

    TSize extent;

    String caption = pgcMainControl->Pages[TabIndex]->Caption;

    if(Active)
    {
        Control->Canvas->Brush->Color = COLOR_BLACK;
        Control->Canvas->Font->Color  = COLOR_WHITE;
        Control->Canvas->FillRect(Rect);

        extent = Control->Canvas->TextExtent(caption);
    }
    else
    {
        Control->Canvas->Font->Style = TFontStyles() >> fsBold;
        extent                       = Control->Canvas->TextExtent(caption);
    }

    int dx = (Rect.Width() - extent.cx) / 2;
    int dy = (Rect.Height() - extent.cy) / 2;

    Control->Canvas->TextRect(Rect, Rect.Left + dx, Rect.Top + dy, caption);

}

// ---------------------------------------------------------------------------
void TReticleOnlineCMDForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::PWGridDrawCell(TObject *Sender, int ACol,
    int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        PWGrid->Canvas->Brush->Color = clBtnFace;
        PWGrid->Canvas->Font->Color  = clWindowText;
        PWGrid->Canvas->FillRect(Rect);
        Frame3D(PWGrid->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 1)
    {
        if(PWGrid->Cells[ACol][ARow] == "ON")
        {
            PWGrid->Canvas->Brush->Color = (TColor)0x00C080FF;
            PWGrid->Canvas->FillRect(Rect);
        }
        else
        {
            PWGrid->Canvas->Brush->Color = COLOR_WHITE;
            PWGrid->Canvas->FillRect(Rect);
        }
    }
    else
    {
        PWGrid->Canvas->Brush->Color = COLOR_WHITE;
        PWGrid->Canvas->FillRect(Rect);

    }
    strText = PWGrid->Cells[ACol][ARow];
    PWGrid->Canvas->Font->Color = COLOR_BLACK;
    PWGrid->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);

}

// ---------------------------------------------------------------------------
/**
 @brief   Hand ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHaONClick(TObject *Sender)
{
	if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpMessage;
        strTmpMessage = "Hand Power ON...OK?";
        if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
		{
			PowerOnOff(AXIS_HAND, PWR_ON);

#if 0
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_PWR_ON);
			delete m_CMDControl;
#endif
        }
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHaOFFClick(TObject *Sender)
{
	if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpMessage;
        strTmpMessage = "Hand Power OFF...OK?";
        if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
		{
			PowerOnOff(AXIS_HAND, PWR_OFF);

#if 0
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_PWR_OFF);
			delete m_CMDControl;
#endif
        }
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Hand ReSet버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnHaRstClick(TObject *Sender)
{
	if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpMessage;
        strTmpMessage = "Hand Power Reset...OK?";
        if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
		{
			PowerOnOff(AXIS_HAND, PWR_RST);

#if 0
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_PWR_RST);
			delete m_CMDControl;
#endif
        }
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   이적재 롱런을 수행하는 timer 함수
 @author  zzang9un
 @date    2013.02.05
 @param   Sender : TObject
 */
void __fastcall TReticleOnlineCMDForm::timerTransLongRunTimer(TObject *Sender)
{
	BYTE CurStatus = gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status;

    if(CurStatus == STS_ERROR)
    {
        timerTransLongRun->Enabled = false;
        chkLongRun->Font->Color    = COLOR_BLUE;
        chkLongRun->Caption        = "Trans LongRun";
        gm_OHTStatus->PreStatus    = STS_ERROR;
        return;
    }

    if((CurStatus == STS_LOADED) && (gm_OHTStatus->PreStatus == STS_LOADED) &&
        (chkLongRun->Caption == "Loading"))
    {
        gm_OHTStatus->PreStatus = STS_LOADING;
    }
    else if((CurStatus == STS_UNLOADED) && (gm_OHTStatus->PreStatus == STS_UNLOADED) &&
        (chkLongRun->Caption == "Unloading"))
    {
        gm_OHTStatus->PreStatus = STS_UNLOADING;
    }

    if((CurStatus == STS_LOADED) && (gm_OHTStatus->PreStatus == STS_LOADING))// Loaded
    {
        chkLongRun->Font->Color = COLOR_RED;
        chkLongRun->Caption     = "Loaded";

        gm_OHTStatus->nTransCount++;
        lbLongrunCount->Caption = "Count:" + IntToStr(gm_OHTStatus->nTransCount);

        // Unload 명령 전송
        TransForm->SendTransCmd(CMD_UNLOAD, gm_OHTStatus->PrePortType);
        MainForm->DataDisplay(COLOR_BLUE, "MANUAL_UNLOAD : " + String(PortTypeToStr(gm_OHTStatus->PrePortType)));
    }
    else if((CurStatus == STS_UNLOADED) && (gm_OHTStatus->PreStatus == STS_UNLOADING))// Unloaded
    {
        chkLongRun->Font->Color = COLOR_RED;
        chkLongRun->Caption     = "Unloaded";

        gm_OHTStatus->nTransCount++;
        lbLongrunCount->Caption = "Count:" + IntToStr(gm_OHTStatus->nTransCount);

        // Load 명령 전송
        TransForm->SendTransCmd(CMD_LOAD, gm_OHTStatus->PrePortType);
        MainForm->DataDisplay(COLOR_BLUE, "MANUAL_LOAD : " + String(PortTypeToStr(gm_OHTStatus->PrePortType)));
    }
    else if(CurStatus == STS_LOADING)// Loading
    {
        chkLongRun->Font->Color = COLOR_RED;
        chkLongRun->Caption     = "Loading";
    }
    else if(CurStatus == STS_UNLOADING)// Unloading
    {
        chkLongRun->Font->Color = COLOR_RED;
        chkLongRun->Caption     = "Unloading";
    }

    // by zzang9un 2013.04.23 : 이적재 시간 체크를 위해 추가
    if((gm_OHTStatus->PreStatus == STS_LOADED) || (gm_OHTStatus->PreStatus == STS_UNLOADED) ||
        (gm_OHTStatus->PreStatus == STS_INIT))
    {
        if((CurStatus == STS_LOADING) || (CurStatus == STS_UNLOADING))
        {
            // 시작 시간 기록
            LongrunStartTime = Time();
        }
    }

    // by zzang9un 2013.04.23 : 이적재 완료 시간 체크
    if((gm_OHTStatus->PreStatus == STS_LOADING) || (gm_OHTStatus->PreStatus == STS_UNLOADING))
    {
        if((CurStatus == STS_LOADED) || (CurStatus == STS_UNLOADED))
        {
            // 완료 시간 기록
            LongrunEndTime = Time();
            lbTransTT->Caption = "T/T:" + FormatFloat(L"##.000", SecondSpan(LongrunStartTime, LongrunEndTime)) + " ms";
        }
    }

    gm_OHTStatus->PreStatus = CurStatus;

}

// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::chkLongRunClick(TObject *Sender)
{
    if(chkLongRun->Checked == false)
    {
        timerTransLongRun->Enabled = false;
        chkLongRun->Font->Color    = COLOR_BLUE;
        chkLongRun->Caption        = "Trans LongRun";
        gm_OHTStatus->PreStatus = STS_INIT;

        // by zzang9un 2014.04.07 : 불필요한 UI 숨김
        lbLongrunCount->Visible = false;
        lbLongrunInterval->Visible = false;
        lbTransTT->Visible = false;
        edtLongRunInterval->Visible = false;
    }
    else
    {
        lbLongrunCount->Visible = true;
        lbLongrunInterval->Visible = true;
        lbTransTT->Visible = true;
        edtLongRunInterval->Visible = true;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Shutter Alarm RST버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnShutterAlarmResetClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_ALARM_RST);
        MainForm->DataDisplay(COLOR_BLACK, "Shutter Alarm Reset");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   Driving #3,4HOME버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 @param   EStop
 */
void __fastcall TReticleOnlineCMDForm::btnMotor34HomeClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    int nTmpRotateAccel, nTmpRotateSpeed;
    String strTmpData;

    if(gm_OHTStatus->Motor_Rotate->bIs_Origin == true)
    {
        if((gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home == ON) &&
            (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) < HOIST_ORIGIN_LIMIT))
        {
            if((gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == ON) &&
                (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == ON))
            {
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFTROTATE_HOME);
                MainForm->DataDisplay(COLOR_BLACK, "All Robot Homing[#3,4J=0]");
            }
            else
            {
                Warnning_Display(COLOR_RED, "Shutter Not Open....");
            }
        }
        else
            Warnning_Display(COLOR_RED, "Hoist Not Origin....");
    }
    else
    {
        Warnning_Display(COLOR_RED, "Rotate Not Origin....");
    }




    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief  Trans Load버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnLoadMouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint P, Q;
    if(Button == mbLeft)
    {
        P.x = X; // P is the TPoint record for the form
        P.y = Y;
        Q   = btnLoad->ClientToScreen(P); // Q is the TPoint for the screen
        pmLoad->Popup(Q.x, Q.y);
    }

}

// ---------------------------------------------------------------------------
/**
 @brief  Trans UnLoad버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnUnloadMouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint P, Q;
    if(Button == mbLeft)
    {
        P.x = X; // P is the TPoint record for the form
        P.y = Y;
        Q   = btnUnload->ClientToScreen(P); // Q is the TPoint for the screen
        pmUnload->Popup(Q.x, Q.y);
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::pmDrivingFServoOnOffPopup(TObject *Sender)
{
    if(gm_OHTStatus->Motor_Driving_F->bIs_ServoON == true)
    {
        mniDrvFServoOn->Visible  = false;
        mniDrvFServoOff->Visible = true;
    }
    else
    {
        mniDrvFServoOn->Visible  = true;
        mniDrvFServoOff->Visible = false;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::pmDrivingRServoOnOffPopup(TObject *Sender)
{
    if(gm_OHTStatus->Motor_Driving_R->bIs_ServoON == true)
    {
        mniDrivingRServoOn->Visible  = false;
        mniDrivingRServoOff->Visible = true;
    }
    else
    {
        mniDrivingRServoOn->Visible  = true;
        mniDrivingRServoOff->Visible = false;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::pmHoistServoOnOffPopup(TObject *Sender)
{
    if(gm_OHTStatus->Motor_Hoist->bIs_ServoON == true)
    {
        mniHoistServoOn->Visible  = false;
        mniHoistServoOff->Visible = true;
    }
    else
    {
        mniHoistServoOn->Visible  = true;
        mniHoistServoOff->Visible = false;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::pmShiftServoOnOffPopup(TObject *Sender)
{
    if(gm_OHTStatus->Motor_Shift->bIs_ServoON == true)
    {
        mniShiftServoOn->Visible  = false;
        mniShiftServoOff->Visible = true;
    }
    else
    {
        mniShiftServoOn->Visible  = true;
        mniShiftServoOff->Visible = false;
    }

}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::pmRotatePopup(TObject *Sender)
{
    if(gm_OHTStatus->Motor_Rotate->bIs_ServoON == true)
    {
        mniRotateServoOn->Visible  = false;
        mniRotateServoOff->Visible = true;
    }
    else
    {
        mniRotateServoOn->Visible  = true;
        mniRotateServoOff->Visible = false;
    }

}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniDrvFServoOnClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniDrvFServoOffClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniDrivingRServoOnClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniDrivingRServoOffClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniHoistServoOnClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniHoistServoOffClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniShiftServoOnClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniShiftServoOffClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniRotateServoOnClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniRotateServoOffClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniRotateOriginSetClick(TObject *Sender)
{
    RotateOrgForm->ShowModal();
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Origin버튼 클릭시 발생하는 이벤트 함수(GO_ORIGIN)
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate_OrignClick(TObject *Sender)
{
    bool bExec = false;

    // if((CmdReticleOHTCheck.Is_Executable()==NO_ERR)&&(CmdReticleOHTCheck.Is_MoveAxis_Rotate()==NO_ERR))
    if((gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home == ON) &&
       (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) < HOIST_ORIGIN_LIMIT))

    {
        if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
        {
            if((gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == OFF) &&
                (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == OFF))
                bExec = true;
            else
            {
                if((gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == ON) &&
                    (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == ON))
                    bExec = true;
                else
                    bExec = false;
            }

            if(bExec == true)
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_GO_ORIGIN);
                MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..");
                delete m_CMDControl;
            }
            else
                MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hand or Shutter]");
        }
    }
    else
    {
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
        MainForm->DataDisplay(COLOR_BLACK, gm_OHTStatus->Motor_Hoist->dPosition_mm);
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Enable버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate_EnableClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_ON);
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Disable버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate_DisableClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_OFF);
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate STOP버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate_StopClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_STOP);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Steering Front ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnFSteerONClick(TObject *Sender)
{
	PowerOnOff(AXIS_FRONT, PWR_ON);

#if 0
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_F_ENABLE);
	delete m_CMDControl;
#endif
}

// ---------------------------------------------------------------------------
/**
 @brief   Steering Front OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnFSteerOFFClick(TObject *Sender)
{
	PowerOnOff(AXIS_FRONT, PWR_OFF);

#if 0
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_F_FREE);
    delete m_CMDControl;
#endif
}

// ---------------------------------------------------------------------------
/**
 @brief   Steering Rear ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRSteerONClick(TObject *Sender)
{
	PowerOnOff(AXIS_REAR, PWR_ON);

#if 0
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_R_ENABLE);
	delete m_CMDControl;
#endif
}

// ---------------------------------------------------------------------------
/**
 @brief   Steering Rear OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRSteerOFFClick(TObject *Sender)
{
	PowerOnOff(AXIS_REAR, PWR_OFF);

#if 0
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_R_FREE);
    delete m_CMDControl;
#endif
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::chkDistClick(TObject *Sender)
{
    if(chkDist->Checked == true)
    {
        pgcJog->ActivePageIndex = 1;
        DistChang_mm_2();
    }
    else
        pgcJog->ActivePageIndex = 0;
}

// ---------------------------------------------------------------------------
int TReticleOnlineCMDForm::DistChang_mm_2(void)
{
    int nError = 0;
    float dDist_mm, dDist_M, dDist_cts;
    String strdd;
    double d1RotateDist_mm = 0.0;

    if(edtDistcts->Focused() == true)
        return nError;

    strdd = edtDistmm->Text;
    if((strdd.Length() == 0) || (strdd == "") || (strdd[1] == '.') || (strdd[1] == ','))
        return nError;

    if((gm_OHTStatus->strOHTVer == OHTVER_V5_U) || (gm_OHTStatus->strOHTVer == OHTVER_V5_S))
        d1RotateDist_mm = DIST_1ROTATE_MM;
    else if((gm_OHTStatus->strOHTVer == OHTVER_V65_U) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_S) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RU) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RS) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V10LP) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_FOSB) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V20_MZ))
        d1RotateDist_mm = DISTV65_1ROTATE_MM;
    else
        d1RotateDist_mm = DISTV6_1ROTATE_MM;

    dDist_mm           = StrToFloat(edtDistmm->Text);
    dDist_M            = dDist_mm / 1000.0;
    panDist_M->Caption = String().sprintf(L"%.02f[m]", dDist_M);

    TCONVERSION *m_Conversion = new TCONVERSION;
    dDist_cts                 = m_Conversion->MM_to_CTS(dDist_mm, COUNT_1ROTATE, d1RotateDist_mm);
    delete m_Conversion;

    edtDistcts->Text = String().sprintf(L"%.0f", dDist_cts);

    return nError;
}

// ---------------------------------------------------------------------------
int TReticleOnlineCMDForm::DistChang_cts_2(void)
{
    int nError = 0;
    float dDist_mm, dDist_M, dDist_cts;
    String strdd;
    double d1RotateDist_mm = 0.0;

    if(edtDistmm->Focused() == true)
        return nError;

    strdd = edtDistcts->Text;
    if((strdd.Length() == 0) || (strdd == "") || (strdd[1] == '.') || (strdd[1] == ','))
        return nError;

    if((gm_OHTStatus->strOHTVer == OHTVER_V5_U) || (gm_OHTStatus->strOHTVer == OHTVER_V5_S))
        d1RotateDist_mm = DIST_1ROTATE_MM;
    else if((gm_OHTStatus->strOHTVer == OHTVER_V65_U) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_S) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RU) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V30_RS) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V10LP) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V65_FOSB) ||
        (gm_OHTStatus->strOHTVer == OHTVER_V20_MZ))
        d1RotateDist_mm = DISTV65_1ROTATE_MM;
    else
        d1RotateDist_mm = DISTV6_1ROTATE_MM;

    dDist_cts = StrToFloat(edtDistcts->Text);

    TCONVERSION *m_Conversion = new TCONVERSION;
    dDist_mm                  = m_Conversion->CTS_to_MM(dDist_cts, COUNT_1ROTATE, d1RotateDist_mm);
    delete m_Conversion;

    dDist_M            = dDist_mm / 1000.0;
    panDist_M->Caption = String().sprintf(L"%.02f[m]", dDist_M);
    edtDistmm->Text    = String().sprintf(L"%.0f", dDist_mm);

    return nError;
}

// ---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::edtDistmmChange(TObject *Sender)
{
    DistChang_mm_2();
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::edtDistctsChange(TObject *Sender)
{
    DistChang_cts_2();
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnMoveClick(TObject *Sender)
{
    // by zzang9un 2012.11.29 : 7.0 단위 통일을 위해 변경
    int Accel    = StrToFloat(edtAcceleration->Text) * 1000;
    int Decel    = StrToFloat(edtDeceleration->Text) * 1000;
	int Velocity = StrToFloat(panMPerSec->Caption) * 1000;
    int Distance = StrToInt(edtDistmm->Text); ;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((Accel == 0) || (Decel == 0) || (Velocity == 0) || (Distance == 0))
	{
		Warnning_Display(COLOR_RED, "ERROR : Accel OR Speed OR Distance is Zero...");
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return;
	}

	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
	{
		Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
		return;
	}

	if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
		return;
	}

	if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
	{
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
		{
			Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
			return;
		}
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
		{
			Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
			return;
		}
		/* shkim. 2016-03-14 skip modify
		else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
		{
			// by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
			Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
			return;
		}
		*/
	}

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71)
		|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == HOIST_Home_V85H))
	{
		if(gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect == true)
		{
			if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
				return;
			}

			if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Close == false ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Close == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
				return;
			}
		}
		else
		{
			if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == true)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
				return;
			}

			if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == false ||
				gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
				return;
			}
		}
	}
	else
	{
        if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == false ||
            gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == true ||
            gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == true ||
            gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == true)
        {
            Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
            return;
        }

        if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == false ||
            gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == false)
        {
            Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
            return;
        }
	}

	// by zzang9un 2013.02.19 : Jog(Drivng) Command를 이용하여 전송
	TPCmd.CmdTP_Jog_DIST.Accel    = Accel;
	TPCmd.CmdTP_Jog_DIST.Decel    = Decel;
	TPCmd.CmdTP_Jog_DIST.Velocity = Velocity;
	TPCmd.CmdTP_Jog_DIST.Distance = Distance;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	MainForm->DataDisplay(COLOR_BLACK, "Driving Forward");
	m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_DIST, &TPCmd);
	MainForm->DataDisplay(COLOR_BLACK, "JOG_DRIVING_DIST:" + String(Distance));
	delete m_CMDControl;
}
// ---------------------------------------------------------------------------

/**
 @brief  Trans Scan[UTB]버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnScanUTBClick(TObject *Sender)
{
    String strTmpMessage;

    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID == 0)
            Warnning_Display(COLOR_RED, "ERROR : OHT Current Node is 0");
        else
        {
            strTmpMessage = "Start UTB Scan Motion... OK?";
            if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_MANUAL_SCAN_UTB);
                MainForm->DataDisplay(COLOR_BLACK, "Send [MANUAL_SCAN_UTB] Command");
                MainForm->panDataStatus->Caption = "";

                lbLongrunCount->Caption      = "Count : 0";
                gm_OHTStatus->nTransCount    = 0;
                gm_OHTStatus->nTransLongMode = TRMODE_SCAN_UTB;

                if(ReticleOnlineCMDForm->chkLongRun->Checked == true)
                    timerTransLongRun->Enabled = true;
                else
                    timerTransLongRun->Enabled = false;

                delete m_CMDControl;
            }
        }
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   증감 범위에 대해 설정하는 함수(1 or 5)
 @author  puting
 @date    2012.12.14
 */

void __fastcall TReticleOnlineCMDForm::btnTractionLeftClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_TRACTION_STEER_LEFT);
        MainForm->DataDisplay(COLOR_BLACK, "Traction Mode Steering Left");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnTractionRightClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_TRACTION_STEER_RIGHT);
        MainForm->DataDisplay(COLOR_BLACK, "Traction Mode Steering Right");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::chkTractionModeClick(TObject *Sender)
{
    if(chkTractionMode->Checked == true)
    {
        btnTractionLeft->Visible  = true;
        btnTractionRight->Visible = true;
        btnSteeringLeft->Visible  = false;
        btnSteeringRight->Visible = false;
    }
    else
    {
        btnTractionLeft->Visible  = false;
        btnTractionRight->Visible = false;
        btnSteeringLeft->Visible  = true;
        btnSteeringRight->Visible = true;
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   Rotate Origin버튼 클릭시 발생하는 이벤트 함수(HOMING)
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotateOriginClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_HOMING);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Origin");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Reset버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotateResetClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_RESET);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Reset");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate 0버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate0Click(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_0);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate 0");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate 90버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate90Click(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_90);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate 90");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate 180버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate180Click(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_180);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate 180");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate 270버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotate270Click(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_270);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate 270");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Free버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRotateFreeClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(btnRotateFree->Caption == "[Rotate Free]")
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_FREE);
            MainForm->DataDisplay(COLOR_BLACK, "Rotate Free");
        }
        else
        {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_ENABLE);
            MainForm->DataDisplay(COLOR_BLACK, "Rotate Enable");
        }

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   MC Control ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnMCControlOnClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_MC_CONTROL_ON);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   MC Control OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnMCControlOffClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_MC_CONTROL_OFF);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   STB Right Work ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnSTB_R_WrokOnClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STB_R_WORK_ON);
    delete m_CMDControl;

}

// ---------------------------------------------------------------------------
/**
 @brief   STB Right Work OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnSTB_R_WorkOffClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STB_R_WORK_OFF);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   STB Left Work ON버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnSTB_L_WrokOnClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STB_L_WORK_ON);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   STB Left Work OFF버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnSTB_L_WrokOffClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STB_L_WORK_OFF);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Rail Support Work버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRailWorkClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_RAILSUPPORT_WORK);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Rail Support Home버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRailHomeClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_RAILSUPPORT_HOME);
    delete m_CMDControl;
}


// ---------------------------------------------------------------------------
/**
 @brief   Rail Support Free버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRailFreeClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_RAILSUPPORT_FREE);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Rail Support Enable버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnRailEnableClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_RAILSUPPORT_ENABLE);
    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief   Driving(주행) MaRK BCR버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnDrivingBCRClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_SETCURNODE);
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Driving(주행) Mark Back버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnDrivingMarkBackwardClick(
    TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpCMD;
        int nMarkNum;

        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
        else
        {
            if((gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left == gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left) &&
                (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right == gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right))
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_MARK_BACKWARD);
                delete m_CMDControl;
            }
            else
            {
                Warnning_Display(COLOR_RED, "ERROR : Steering not same status...");
            }
        }
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   Driving(주행) Mark Front버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TReticleOnlineCMDForm::btnDrivingMarkForwardClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        String strTmpCMD;
        int nMarkNum;

        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
		/* shkim. 2016-03-14 skip modify
        else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
        {
            // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
            Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
        }
        */
        else
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_MARK_FORWARD);
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   콤보박스에서 Trans Node Type 값을 알아오는 함수
@author  zzang9un
@date    2012.12.11
@param   index : Combobox의 index
@return  변환된 값
*/
BYTE TReticleOnlineCMDForm::GetTransNodeType(int index)
{
    BYTE rtnVal = 0x00;

    switch(index)
    {
    case 0:
        rtnVal = TAG_TYPE_STOP;
        break;

    case 1:
        rtnVal = TAG_TYPE_QR_LEFT;
        break;

    case 2:
        rtnVal = TAG_TYPE_QR_RIGHT;
        break;

    default:
        rtnVal = 0x00;
        break;
    }

    return rtnVal;
}
// ---------------------------------------------------------------------------

/**
@brief   체크박스 클릭 이벤트 함수

         - 체크 유무에 따라 이적재 관련 Edit, Combo 박스 활성화/비활성화
         - 이적재 Node로 주행하려고 할 경우 체크해야 함
@author  zzang9un
@date    2013.01.22
@param   Sender : TObject
*/
void __fastcall TReticleOnlineCMDForm::chkGoStationClick(TObject *Sender)
{
/*
    if(chkGoStation->Checked)
    {
        edtStopStationID->Enabled      = true;
        edtStopStationsOffset->Enabled = true;
        edtNextNodeID->Enabled         = true;
        cbbStopStationType->Enabled    = true;
    }
    else
    {
        edtStopStationID->Enabled      = false;
        edtStopStationsOffset->Enabled = false;
        edtNextNodeID->Enabled         = false;
        cbbStopStationType->Enabled    = false;
	}
	*/
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadEQClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadEQ_XClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadSTOCKERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadSTB_LClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadSTB_RClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadUTBClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_UTB);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniLoadLOADERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadEQClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    // shkim  임시 주의!!!!!  if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadEQ_XClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadSTOCKERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadLOADERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadSTB_LClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadSTB_RClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniUnloadUTBClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_UTB);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   QR Left Mark 버튼 클릭 시 호출되는 함수
 @author  zzang9un
 @date    2013.03.26
 */
void __fastcall TReticleOnlineCMDForm::btnQRTagLeftMarkClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
		/* shkim. 2016-03-14 skip modify
        else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
        {
            // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
            Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
        }
        */
        else
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_QR_LEFT_MARK_FORWARD);
			delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
 @brief   QR Right Mark 버튼 클릭 시 호출되는 함수
 @author  zzang9un
 @date    2013.03.26
 */
void __fastcall TReticleOnlineCMDForm::btnQRTagRightMarkClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        if((gm_OHTStatus->Motor_Driving_F->nError == 1) ||
            (gm_OHTStatus->Motor_Driving_R->nError == 1) ||
            (gm_OHTStatus->Motor_Hoist->nError == 1) ||
            (gm_OHTStatus->Motor_Shift->nError == 1) ||
            (gm_OHTStatus->Motor_Rotate->nError == 1))
        {
            Warnning_Display(COLOR_RED, "ERROR : OHT Motor is Alarm Status...");
        }
		/* shkim. 2016-03-14 skip modify
		else if(gm_OHTStatus->IOInput->uInput.Input_Reticle.RailSupport_Home == OFF)
        {
            // by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
            Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
        }
        */
        else
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_QR_RIGHT_MARK_FORWARD);
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::panTeachingSaveClick(TObject *Sender)
{
    TeachingForm->Show();
    TeachingForm->pgcTeaching->ActivePageIndex = 1;
}
//---------------------------------------------------------------------------

/**
@brief   ReticleOnlineCMDForm의 Timer를 Enable하는 함수
@author  zzang9un
@date    2013.11.20
*/
void TReticleOnlineCMDForm::timerAllDisable()
{
    timerUpdateDisplay->Enabled = false;
    timerDrivingJog->Enabled = false;
    timerTransLongRun->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnMoveTeachingPointMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    TPoint P, Q;
    if(Button == mbLeft)
    {
        P.x = X; // P is the TPoint record for the form
        P.y = Y;
        Q   = btnMoveTeachingPoint->ClientToScreen(P); // Q is the TPoint for the screen
        pmMoveTP->Popup(Q.x, Q.y);
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPEQClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPEQ_XClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPSTOCKERClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPLOADERClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPSTB_LClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPSTB_RClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::mniMoveTPUTBClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_UTB);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------


void __fastcall TReticleOnlineCMDForm::btnHoistHomeAutoSetClick(TObject *Sender)
{
	if((CmdReticleOHTCheck.Is_Executable() == NO_ERR) && (CmdReticleOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		// 이적재 명령 구조체
		TRANS_CMD SndTransCmd;

		// 구조체 초기화
		memset(&SndTransCmd, 0x00, sizeof(TRANS_CMD));

		m_CMDControl->SendTransCmd2OHT(TPCMD_TRANS_HOME_AUTOSET, &SndTransCmd);
		MainForm->DataDisplay(COLOR_BLACK, "[TPCMD_TRANS_HOME_AUTOSET]");
		delete m_CMDControl;
	}
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidLeftSelClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidLeftSel_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidRightSelClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidRightSel_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidOccupReqClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidOccupReq_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidPassComptClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidPassCompt_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidManualClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidManual_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidOperationClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidOperation_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidRev1Click(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidRev1_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidRev2Click(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TReticleOnlineCMDForm::btnCidRev2_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2_OFF);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------





// ---------------------------------------------------------------------------
/**
 @brief   Go버튼 클릭 이벤트 발생 시 호출되는 함수
 @author  zzang9un
 @date    2012.12.01
 @param   Sender : TObject
 @note    UI의 값을 Go Command 구조체에 직접 넣어서 생성하도록 변
 */
void __fastcall TReticleOnlineCMDForm::BitBtn1Click(TObject *Sender)
{
#if 0
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	// int iStatusSize = sizeof(PROTOCOL_STATUS_OCS);
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;
	AnsiString strMachineID = "TP001";


	iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
	PKT_JOB_ORDER_ID_121	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	// OCS상태 초기화 - 이전값을 읽어온다
	// TP에서 초기화 시킬 값이 없음.
//	memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
//	memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

	// --- Status
	// 상태지시 - 기동 ON
	SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;

	// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

	// =============================== Body ===============================
	SEND_PACKET.BODY.CmdID = StrToInt(Edit1->Text);	// 지시 ID

	iTemp = ORDERDIV::JobOrder;						// 지시구분 - 작업지시
	memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

	// 이적재 Buffer 위치
	SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

	iTemp = CYCLE::MovingToStation;  				// 지시사이클 - 이동 사이클
	memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

	// 우선도
	RIV tmpRv;
	tmpRv.Riv_Value = 64;							// 7비트니까
	tmpRv.Hotlot = 0;								// Hotlot - off
	memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

	// 좌우 우선방향
	if(CheckBox1->Checked==1)
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
	else
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

	SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

	// 시작 NODE
	Int2Uchar3((unsigned int)StrToInt (Edit4->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

	// 목적 NODE
	Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

	// STOP NODE
	Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

	// 목적지 STATION
	Int2Uchar3((unsigned int)StrToInt (Edit5->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

	// ===== 12/10 add ......
	// 모두 OFF 이면 주행Tag로 이동
	SEND_PACKET.BODY.MarkType = StrToInt(Edit6->Text);	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)


	// SationData_Target_Rundist
	SEND_PACKET.BODY.StationData_RunDist = StrToInt(Edit7->Text);

	// NextNodeID (item deleted!!)
	/*
	Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
	SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
	*/

	// TeachingData_Hoist - Hoist
	SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(Edit8->Text);

	// TeachingData_Shift - Shift
	SEND_PACKET.BODY.TeachingData_Shift = StrToInt(Edit9->Text);

	// TeachingData_Rotate - Rotate
	SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(Edit10->Text);

	// ============ fixed value set ============
	SEND_PACKET.BODY.TeachingData_FinalPortType = 1;	// TeachingData_FinalPortType - EQ set
														// ver 29-5 : bit field, ver 37 : unsigned char (Changed)
	SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_Left = 1;
	SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_RF = 1;
	SEND_PACKET.BODY.TeachingData_PIOInfo.PIO_CS0 = 1;

	SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
	SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
	// =========================================


	// ================= Path Data Parsing Start ===================
	AnsiString strTmp = StrToInt(Edit3->Text);
	AnsiString strDim[1000];
	int iCnt = 0;

	strTmp = strTmp.Trim();
	// if((strTmp.Trim().Length() != 0)
	if((strTmp != "") && (strTmp != "(null)"))
	{
		AnsiString strWork;
		int iPos = 0;

		while(strTmp.AnsiPos(",") != 0)
		{
			iPos = strTmp.AnsiPos(",");					// 콤마 위치탐색
			strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
			strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
			strTmp = strTmp.Delete(1,iPos);
		}

		strDim[iCnt++] = strTmp.Trim();					// 마지막 원소 입력
	}

	// ----------------------------------------------------------------------
	SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

	for(int i = 0; i < iCnt; i++)
	{
		Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);

		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
	}
	// ================= Path Data Parsing End ===================
	 // shkim
	strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);


	strLog.sprintf("작업지시22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
					SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);

	// 다이후쿠 문서에 있는 실행모드 = 통상모드 는 어느건지 모르겠다

	// 데이터 전송
	// 불필요  Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");

#endif

//	memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다
//	tmrRetry->Enabled = true;

//	frmViewLog->Log("작업지시 - 이동사이클 (정상) 명령 송신");
}
//---------------------------------------------------------------------------

void TReticleOnlineCMDForm::Display_CID_INFO(PKT_OHT2TP_CID_INFO PKT)
{
	char cDisplay[4];
	AnsiString strTmp;

	memset(&cDisplay, '\0', 4);
	memcpy(&cDisplay, PKT.BODY.CIDR_FirstLinkLevel, 3);	// CID-O
	strTmp = cDisplay;
	strTmp = strTmp + "0";
	
	if (iCidInfo_Direct == 0)
		edtCid_O_Link->Text = strTmp;
	else
		edtCid_O_Right->Text = strTmp;


	memset(&cDisplay, '\0', 4);
	memcpy(&cDisplay, PKT.BODY.CIDO_FirstLinkLevel, 3);	// CID-R
	strTmp = cDisplay;
	strTmp = strTmp + "0";
	
	if (iCidInfo_Direct == 0)
		edtCid_R_Link->Text = strTmp;
	else
        edtCid_R_Right->Text = strTmp;

}

void TReticleOnlineCMDForm::Display_MapMove_Result(PKT_OHT2TP_MOVE_BY_MAP_RESULT PKT)
{
	switch(PKT.BODY.iResult)
	{
		case MAP_MOVE_NO_ERR:
			edtMoveResult->Text = "정상 완료";
		break;

		case MAP_MOVE_ERR_NOT_MARKED:
			edtMoveResult->Text = "마크 안된 상태. 확인하세요";
		break;

		case MAP_MOVE_ERR_MAP_SEARCH_FAIL:
			edtMoveResult->Text = "맵 탐색 실패. 해당 노드가 존재하는지 확인요망";
		break;
	}
}

void TReticleOnlineCMDForm::Display_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT)
{
	AnsiString strResult;	
	strResult.sprintf("[Map Make] %s", PKT.BODY.Log);	
	mmCom->Lines->Add(strResult);

}

void TReticleOnlineCMDForm::Display_Com(PKT_OHT2OCS PKT)
{
	unsigned char cVar1, cVar2, cVar3;
	AnsiString strTmp;

	memcpy(&cVar1, &PKT.STATUS.WorkingStatus, 1);
	memcpy(&cVar2, &PKT.STATUS.RunningStatus, 1);
	memcpy(&cVar3, &PKT.STATUS.Carriage, 1);
	strTmp.sprintf("CLW가 TP로 작업지시 응답  [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
					PKT.HEADER.MachineType, PKT.HEADER.MachineID, PKT.TAG.Value,
					Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));

	mmCom->Lines->Add(strTmp);

}

void __fastcall TReticleOnlineCMDForm::btnForceOpenClick(TObject *Sender)
{
/*
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP);
	delete m_CMDControl;
*/

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;
	AnsiString strMachineID = "TP001";

	iPacketSize = sizeof(PKT_OCS2OHT);
	PKT_OCS2OHT	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP;	// 강제통과 개방

	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

int TReticleOnlineCMDForm::TransLoad(void)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;
	AnsiString strMachineID = "TP001";


	iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
	PKT_JOB_ORDER_ID_121	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	// OCS상태 초기화 - 이전값을 읽어온다
	// TP에서 초기화 시킬 값이 없음.
//	memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
//	memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

	// --- Status
	// 상태지시 - 기동 ON
	SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
	SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 0;
	SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 1;	// 내림 이재 허가


	// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

	// =============================== Body ===============================
	SEND_PACKET.BODY.CmdID = GetOrderID();			// 지시 ID를 자동으로 생성한다

	iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
	memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

	// 이적재 Buffer 위치
	SEND_PACKET.BODY.BufferType.Buffer_1 = 1;		// 제1 수하대

	iTemp = CYCLE::CraneDown;						// 지시사이클 - 내림 사이클 (LOAD)
	memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

	// 우선도
	RIV tmpRv;
	tmpRv.Riv_Value = 64;							// 7비트니까
	tmpRv.Hotlot = 0;								// Hotlot - off
	memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

	// 좌우 우선방향 (의미가 없다고 판단되면 삭제 - 왼쪽으로 고정한다)
	if(chkDefaultTurn_Left->Checked == 1)
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
	else
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

	SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

	// ====================================================================================================
	// D사양서 p34 - "목적 스테이션이 없는 경우는 0을 설정한다. 이 경우, 경로 데이터도 모두 0으로 채운다"
	// shkim.  올리고 내리는 것이므로 노드번호, 목적지 스테이션 번로는 모두 0으로 채워도 될듯 (확인필요)
	// ==> 일단 검증을 해 보고 적용하도록 하자
#if 0
	// 시작 NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit4->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = 0;	// cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = 0;	// cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = 0;	// cLow;

	// 목적 NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = 0; // cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = 0; // cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = 0; // cLow;

	// STOP NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = 0; // cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = 0; // cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = 0; // cLow;

	// 목적지 STATION
	Int2Uchar3((unsigned int)StrToInt (Edit5->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = 0;	// cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = 0;	// cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = 0;	// cLow;
#endif

	// 시작 NODE
	Int2Uchar3((unsigned int)StrToInt (edtStartNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

	// 목적 NODE
	Int2Uchar3((unsigned int)StrToInt (edtTargetNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

	// STOP NODE
	Int2Uchar3((unsigned int)StrToInt (edtTargetNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

	// 목적지 STATION
	Int2Uchar3((unsigned int)StrToInt (edtTargetStation->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;
	// ====================================================================================================

// 2017.01.04 - 작업지시 패킷이 변경되어 일단 막아둔다
#if 0
	// ===== 12/10 add ......
	// 모두 OFF 이면 주행Tag로 이동
	// 마크타입도 고정 또는 콤보박스 등으로 대체할 필요가 있다
	SEND_PACKET.BODY.MarkType = StrToInt(edtTrans_MarkType->Text);	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	// SationData_Target_Rundist
	SEND_PACKET.BODY.StationData_RunDist = StrToInt(edtTrans_RunDist->Text);

	// TeachingData_Hoist - Hoist
	SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(edtTrans_Hoist->Text);

	// TeachingData_Shift - Shift
	SEND_PACKET.BODY.TeachingData_Shift = StrToInt(edtTrans_Shift->Text);

	// TeachingData_Rotate - Rotate
	SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(edtTrans_Rotate->Text);


	// ============ fixed value set ============
	SEND_PACKET.BODY.TeachingData_FinalPortType = 2;	// TeachingData_FinalPortType - EQ set
														// ver 29-5 : bit field, ver 37 : unsigned char (Changed)
	SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	// SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_Left = 1;
	// SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_RF = 1;
	// SEND_PACKET.BODY.TeachingData_PIOInfo.PIO_CS0 = 1;

	SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;
	SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1; 	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
	SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
	// =========================================
#endif

	// ================= Path Data Parsing Start ===================
#if 0
	AnsiString strTmp = StrToInt(Edit3->Text);
	AnsiString strDim[1000];
	int iCnt = 0;

	strTmp = strTmp.Trim();
	// if((strTmp.Trim().Length() != 0)
	if((strTmp != "") && (strTmp != "(null)"))
	{
		AnsiString strWork;
		int iPos = 0;

		while(strTmp.AnsiPos(",") != 0)
		{
			iPos = strTmp.AnsiPos(","); 				// 콤마 위치탐색
			strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
			strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
			strTmp = strTmp.Delete(1,iPos);
		}

		strDim[iCnt++] = strTmp.Trim(); 				// 마지막 원소 입력
	}

	// ----------------------------------------------------------------------
	SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

	for(int i = 0; i < iCnt; i++)
	{
		Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);

		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
	}
	// ================= Path Data Parsing End ===================
#endif
	SEND_PACKET.BODY.PathData_ExtraNum = 0; 	//
	// ----------------------------------------------------------------------

// 2017.01.04 - 작업지시 패킷이 변경되어 일단 막아둔다
#if 0
	 // shkim
	strLog.sprintf("MOVE ORDER (Trans - Load) - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
	mmCom->Lines->Add(strLog);
#endif

	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	return iReturn;
}

//---------------------------------------------------------------------------


int TReticleOnlineCMDForm::TransUnLoad(void)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;
	AnsiString strMachineID = "TP001";


	iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
	PKT_JOB_ORDER_ID_121	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	// OCS상태 초기화 - 이전값을 읽어온다
	// TP에서 초기화 시킬 값이 없음.
//	memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
//	memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::JOB_ORDER_TP;	// 작업지시

	// --- Status
	// 상태지시 - 기동 ON
	SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
	SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 1;	// 올림 이재 허가
	SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 0;


	// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

	// =============================== Body ===============================
	SEND_PACKET.BODY.CmdID = GetOrderID();			// 지시 ID를 자동으로 생성한다

	iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
	memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

	// 이적재 Buffer 위치
	SEND_PACKET.BODY.BufferType.Buffer_1 = 1;		// 제1 수하대

	iTemp = CYCLE::CraneUp; 					// 지시사이클 - 올림 사이클 (UNLOAD)
	memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

	// 우선도
	RIV tmpRv;
	tmpRv.Riv_Value = 64;							// 7비트니까
	tmpRv.Hotlot = 0;								// Hotlot - off
	memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

	// 좌우 우선방향 (의미가 없다고 판단되면 삭제 - 왼쪽으로 고정한다)
	if(chkDefaultTurn_Left->Checked == 1)
	SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
	else
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

	SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

	// ====================================================================================================
	// D사양서 p34 - "목적 스테이션이 없는 경우는 0을 설정한다. 이 경우, 경로 데이터도 모두 0으로 채운다"
	// shkim.  올리고 내리는 것이므로 노드번호, 목적지 스테이션 번로는 모두 0으로 채워도 될듯 (확인필요)
	// ==> 일단 검증을 해 보고 적용하도록 하자
#if 0
	// 시작 NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit4->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = 0;	// cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = 0;	// cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = 0;	// cLow;

	// 목적 NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = 0; // cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = 0; // cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = 0; // cLow;

	// STOP NODE
	// Int2Uchar3((unsigned int)StrToInt (Edit2->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = 0; // cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = 0; // cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = 0; // cLow;

	// 목적지 STATION
	Int2Uchar3((unsigned int)StrToInt (Edit5->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = 0;	// cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = 0;	// cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = 0;	// cLow;
#endif
	// 시작 NODE
	Int2Uchar3((unsigned int)StrToInt (edtStartNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;
	// 목적 NODE
	Int2Uchar3((unsigned int)StrToInt (edtTargetNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;
	// STOP NODE
	Int2Uchar3((unsigned int)StrToInt (edtTargetNode->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;
	// 목적지 STATION
	Int2Uchar3((unsigned int)StrToInt (edtTargetStation->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;
	// ====================================================================================================


// 2017.01.04 - 작업지시 패킷이 변경되어 일단 막아둔다
#if 0
	// ===== 12/10 add ......
	// 모두 OFF 이면 주행Tag로 이동
	// 마크타입도 고정 또는 콤보박스 등으로 대체할 필요가 있다
	SEND_PACKET.BODY.MarkType = StrToInt(edtTrans_MarkType->Text);	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	// SationData_Target_Rundist
	SEND_PACKET.BODY.StationData_RunDist = StrToInt(edtTrans_RunDist->Text);

	// TeachingData_Hoist - Hoist
	SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(edtTrans_Hoist->Text);

	// TeachingData_Shift - Shift
	SEND_PACKET.BODY.TeachingData_Shift = StrToInt(edtTrans_Shift->Text);

	// TeachingData_Rotate - Rotate	// Rotate는 현재 기구가 없으므로 의미 없지 않은가
	SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(edtTrans_Rotate->Text);


	// ============ fixed value set ============
	// FinalPortType ==> OCS에뮬 자동에는 이 값이 2로 고정되어 있다 (참고). Running is 1.
	SEND_PACKET.BODY.TeachingData_FinalPortType = 2;	// TeachingData_FinalPortType - EQ set
														// ver 29-5 : bit field, ver 37 : unsigned char (Changed)
	SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	// SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_Left = 1;
	// SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_RF = 1;
	// SEND_PACKET.BODY.TeachingData_PIOInfo.PIO_CS0 = 1;

	SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;
	SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1; 	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

	SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
	SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
	// =========================================
#endif

	// ================= Path Data Parsing Start ===================
#if 0
	AnsiString strTmp = StrToInt(Edit3->Text);
	AnsiString strDim[1000];
	int iCnt = 0;

	strTmp = strTmp.Trim();
	// if((strTmp.Trim().Length() != 0)
	if((strTmp != "") && (strTmp != "(null)"))
	{
		AnsiString strWork;
		int iPos = 0;

		while(strTmp.AnsiPos(",") != 0)
		{
			iPos = strTmp.AnsiPos(","); 				// 콤마 위치탐색
			strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
			strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
			strTmp = strTmp.Delete(1,iPos);
		}

		strDim[iCnt++] = strTmp.Trim(); 				// 마지막 원소 입력
	}

	// ----------------------------------------------------------------------
	SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

	for(int i = 0; i < iCnt; i++)
	{
		Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);

		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
	}
	// ================= Path Data Parsing End ===================
#endif
	SEND_PACKET.BODY.PathData_ExtraNum = 0;
	// ----------------------------------------------------------------------

// 2017.01.04 - 작업지시 패킷이 변경되어 일단 막아둔다
#if 0
	 // shkim
	strLog.sprintf("MOVE ORDER (Trans - Load) - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
	mmCom->Lines->Add(strLog);
#endif




	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	return iReturn;



}



//---------------------------------------------------------------------------
// 이 함수는 필요하다면 TReticleOnlineCMDForm::Display_Com(PKT_OHT2OCS PKT)
// 을 참고하여 보강한다

void __fastcall TReticleOnlineCMDForm::tmrTransRunTimer(TObject *Sender)
{
#if 0
	int iReturn;

	// ----------- 불필요하면 바로 빠져나간다 -----------
	if(chkTransLongRun->Checked == false)
		return;		// 할일 없으므로 바로 나간다

	// -------------- 로드하도록 함 ---------------
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_UNLOADED)
	{
		mmCom->Lines->Add("이적재 롱런 - LOAD명령 전송");
		iReturn = TransLoad();

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		{
			mmCom->Lines->Add("이적재 롱런중 (LOAD시도) 통신오류로 동작 정지함!");
			chkTransLongRun->Checked = false;
		}
	}

	// -------------- 언로드하도록 함 ---------------
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_LOADED)
	{
		mmCom->Lines->Add("이적재 롱런 - UNLOAD명령 전송");
		iReturn = TransUnLoad();

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		{
			mmCom->Lines->Add("이적재 롱런중 (UNLOAD시도) 통신오류로 동작 정지함!");
			chkTransLongRun->Checked = false;
		}
	}
#endif
}
//---------------------------------------------------------------------------



void __fastcall TReticleOnlineCMDForm::btnRotate_0Click(TObject *Sender)
{
    bool bHoistHome = gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((CmdReticleOHTCheck.Is_Executable() == NO_ERR) && (CmdReticleOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            TPCmd.CmdTP_Jog_TR.Distance = 0;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "MOVE ROTATE 0º");
            delete m_CMDControl;
        }
    }
    else
    {
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Move ERROR[check Hoist Origin]");
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnRotate_90Click(TObject *Sender)
{
    bool bHoistHome = gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((CmdReticleOHTCheck.Is_Executable() == NO_ERR) && (CmdReticleOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            TPCmd.CmdTP_Jog_TR.Distance = 900;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "MOVE ROTATE 90º");
            delete m_CMDControl;
        }
    }
    else
    {
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Move ERROR[check Hoist Origin]");
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnRotate_180Click(TObject *Sender)
{
    bool bHoistHome = gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((CmdReticleOHTCheck.Is_Executable() == NO_ERR) && (CmdReticleOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            TPCmd.CmdTP_Jog_TR.Distance = 1800;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "MOVE ROTATE 180º");
            delete m_CMDControl;
        }
    }
    else
    {
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Move ERROR[check Hoist Origin]");
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnTransLoadClick(TObject *Sender)
{
	//if(TransLoad() == UNICAST_SEND_ADDRESS_NOT_RESISTED)
	//	ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnTransUnLoadClick(TObject *Sender)
{
	//if(TransUnLoad() == UNICAST_SEND_ADDRESS_NOT_RESISTED)
	//	ShowMessage(L"Address not registed!!, try first");


//	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
//		ShowMessage(L"Address not registed!!, try first");

//	memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다
//	tmrRetry->Enabled = true;

//	frmViewLog->Log("작업지시 - 이동사이클 (정상) 명령 송신");

}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::tmrCidInfoReadTimer(TObject *Sender)
{
	tmrCidInfoRead->Enabled = false;	// timer off
	btnCidMonitoring->Enabled = true;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidMonitoringClick(TObject *Sender)
{
	// btnCidMonitoring->Enabled = false;

#if 0
	btnCidRightSel_OffClick(NULL);
	Sleep(100);

	btnCidLeftSelClick(NULL);
	Sleep(100);

	iCidInfo_Direct = 0;	// left

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	delete m_CMDControl;

	tmrCidInfoRead->Enabled = true;
#endif

	btnCidOperationClick(NULL);
	Sleep(100);

	btnCidRightSel_OffClick(NULL);
	Sleep(100);

	btnCidLeftSelClick(NULL);
	Sleep(100);

	iCidInfo_Direct = 0;	// left

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	Sleep(300);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnMapBaseMoveClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn;

	AnsiString strMachineID = "TP001";

	edtMoveResult->Text = "";

    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
        return;
    }

    if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
    }

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
        ((gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Left != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Left) ||
        (gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_F_Right != gm_OHTStatus->IOInput->uInput.Input_Reticle.STEER_R_Right)))
    {
        Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdReticleOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
        else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71)
    	|| (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == HOIST_Home_V85H))
    {
        if(gm_OHTStatus->IOInput->uInput.Input_Reticle.FOUP_Detect == true)
        {
            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == true ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Open == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Close == true ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Right_Grip == true ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

            if (gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInput.Input_Reticle.SHUTTER_R_Open == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
        }
    }

	// 새로운 TP 이동명령 패킷을 이용한다.
	iPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
	PKT_TP2OHT_MOVE_BY_MAP	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_BY_MAP; // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)

	// ---- Body Set
	// 도달점 노드 타입 입력
    if(cbCheckGoStation->Checked == true)
		SEND_PACKET.BODY.IsGoStation = TRUE;
	else
		SEND_PACKET.BODY.IsGoStation = FALSE;

    /*
	if (chkGoStation->Checked == true)
		SEND_PACKET.BODY.IsGoStation = TRUE;
	else
		SEND_PACKET.BODY.IsGoStation = FALSE;
    */

	// 시작점 번호 입력 (디버깅을 위한 것으로, 추후 삭제예정)
	SEND_PACKET.BODY.StartNodeID = StrToInt (edtMoveStart->Text);

	// 목적지 번호 입력
	SEND_PACKET.BODY.StopNodeID = StrToInt (edtMoveEnd->Text);

	// FOUB 타입 (추가해야 한다)
	// 0x01 : EQ, 0x02 : EQ_X, 0x03 : STK, 0x04 : Loader,
	// 0x05 : STB_L, 0x06 : STB_R, 0x07 : UTB
	if(ChkMapMakeAll->Checked == true)
	{
        SEND_PACKET.BODY.PortType = 0x01;
        if (chkGoStation->Checked == true)
        {
            Warnning_Display(COLOR_RED, "ERROR : No Driving Tag");
            return ;
        }
        else
            SEND_PACKET.BODY.PortType += 0x64;
	}

	// 데이터 전송
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");

}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnCidMonitoring_RightClick(TObject *Sender)
{
#if 0
	btnCidLeftSel_OffClick(NULL);
	Sleep(100);

	btnCidRightSelClick(NULL);
	Sleep(100);

	iCidInfo_Direct = 1;	// right

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	delete m_CMDControl;

	btnCidMonitoring->Enabled = false;
	tmrCidInfoRead->Enabled = true;
#endif

	btnCidOperationClick(NULL);
	Sleep(100);

	btnCidLeftSel_OffClick(NULL);
	Sleep(100);

	btnCidRightSelClick(NULL);
	Sleep(100);

	iCidInfo_Direct = 1;	// right

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	Sleep(300);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void TReticleOnlineCMDForm::PowerOnOff(int iAxis, int iOnOff)
{

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

	SEND_PACKET.TAG.Value = TagID::CMD_TP_POWER_ONOFF;
	SEND_PACKET.BODY.iVar1 = iAxis;
	SEND_PACKET.BODY.iVar2 = iOnOff;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;
}

void __fastcall TReticleOnlineCMDForm::btnMoveAllHomeClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
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

        SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_ALL_HOME;

        m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

    	delete m_CMDControl;
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::btnAutoChangeClick(TObject *Sender)
{
    if(CmdReticleOHTCheck.Is_Executable() == NO_ERR)
    {
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

        SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOMODE_CHANGE;

        m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

        delete m_CMDControl;
    }
}
//---------------------------------------------------------------------------

void __fastcall TReticleOnlineCMDForm::Button1Click(TObject *Sender)
{
    bool bHoistHome = gm_OHTStatus->IOInput->uInput.Input_Reticle.HOIST_Home;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((CmdReticleOHTCheck.Is_Executable() == NO_ERR) && (CmdReticleOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
			TPCmd.CmdTP_Jog_TR.Distance = StrToInt(edtRotateMove->Text);;
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "MOVE ROTATE Move Cmd");
            delete m_CMDControl;
        }
    }
    else
    {
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Move ERROR[check Hoist Origin]");
    }
}
//---------------------------------------------------------------------------

