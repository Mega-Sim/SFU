// ---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>
#include <DateUtils.hpp>
#pragma hdrstop

#include <tlhelp32.h>
#include "FoupOnlineCMDUnit.h"
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

//#include "AutoJob.h"
#include "Option.h"
#include "../Common/PUtily.h"
#include "UDP.h"

//#include "ResponsePasspermitOCS.h"
//#include "OcsPassConfirmResponse.h"
// #include "RunningStatus.h"

#include "../Common/Protocol_Define_Modify.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFoupOnlineCMDForm *FoupOnlineCMDForm;
TCHKOHT CmdFoupOHTCheck;

// by zzang9un 2013.04.24 : Timer Interval Setting
#define TIMER_INTV_TRANSLONGRUN     50  ///< 이적재 롱런 타이머 Interval

// 2018.10.12 SHCHO : VHL에서 VNC 서버가 켜져있는 경우(원격 접속 당했을 때)
// WinVnc 프로세스의 스레드 개수로 원격 접속 중인지, 아닌지 알수 있다.
// thread 개수가 5개이면 원격 접속 중,  그 이하면 비접속 상태.
// return -1은 WinVnc가 실행중이지 않음
int GetVNCThreadCount()
{
	PROCESSENTRY32  procEntry={0};
	procEntry.dwSize = sizeof( PROCESSENTRY32 );

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(h,&procEntry);
	int nThreadCnt = -1;
	char szVNC[] = "WinVnc.exe"; // VNC 실행파일 이름

	// 실행중인 프로세스에서 VNC를 찾는다
	do
	{
		char exeName[128];
		ZeroMemory(exeName, sizeof(exeName));
		wcstombs(exeName, procEntry.szExeFile, sizeof(exeName));

		if(strcmpi(exeName, szVNC) == 0)
		{
			//
			nThreadCnt = procEntry.cntThreads;
			break;
		}
	}while(Process32Next(h, &procEntry));

	CloseHandle(h);

	return nThreadCnt;
}

// ---------------------------------------------------------------------------
__fastcall TFoupOnlineCMDForm::TFoupOnlineCMDForm(TComponent *Owner) : TForm(Owner)
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

void __fastcall TFoupOnlineCMDForm::WMEXITSIZEMOVE(TMessage &msg)
{
    int nTmpLeft, nTmpTop, nTmpRight;

    nTmpLeft  = FoupOnlineCMDForm->Left;
    nTmpTop   = FoupOnlineCMDForm->Top;
    nTmpRight = nTmpLeft + FoupOnlineCMDForm->Width;

    // StatusForm-> MainForm의 왼쪽,,나머지는 모두 아래..
    CmdWinForm->Top  = nTmpTop;
    CmdWinForm->Left = nTmpRight;
}

// ---------------------------------------------------------------------------
TTextAttributes *__fastcall TFoupOnlineCMDForm::CurrText(void)
{
    return PMACMemo->SelAttributes;
}

// ---------------------------------------------------------------------------
/**
 @brief   FoupOnlineCMDForm의 Show 발생시 호출되는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::FormShow(TObject *Sender)
{
	Label6->Caption  = "Accel/Decel[m/sec2]";

    timerUpdateDisplay->Enabled = true;

 //	panName->Caption = "["+SocketForm->m_strOHTName+"]" + " Connect...:" + SocketForm->OHTSocket->Address;

	if(SocketForm->m_strOHTNum.Length() > 4)
		panVer->Font->Size = 15;
//	else if(SocketForm->m_strOHTNum.Length() > 2)
//		panVer->Font->Size = 32;
//	else
//		panVer->Font->Size = 48;

  //기존 :panVer->Caption = SocketForm->m_strOHTNum;
   //20170710 호기명  추가
	   AnsiString strTmp;
	   strTmp.sprintf("%s", m_VHLNAME);
	   panVer->Caption = strTmp;          //FOUP TP
	   panName->Caption  = "["+strTmp+"]" + " Connect...:" + SocketForm->OHTSocket->Address;
	   MainForm -> panVer->Caption = strTmp;    //MAIN TP


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
void __fastcall TFoupOnlineCMDForm::FormClose(TObject *Sender,
    TCloseAction &Action)
{
	timerAllDisable();
}

// ---------------------------------------------------------------------------
int TFoupOnlineCMDForm::Check_Invalid_Command(String strTmpCMD)
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
            if(CmdFoupOHTCheck.Is_Executable
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

void __fastcall TFoupOnlineCMDForm::mniMemoClearClick(TObject *Sender)
{
    PMACMemo->Clear();
}

// ---------------------------------------------------------------------------
void TFoupOnlineCMDForm::PMACDataDisplay(TColor TmpFontColor, String strDisplayData)
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
void __fastcall TFoupOnlineCMDForm::btnHandCloseClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnHandOpenClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

			if(gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == OFF &&
                gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == OFF)
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
void __fastcall TFoupOnlineCMDForm::btnGoClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
        else if(gm_OHTStatus->IOInput->uInputCommon.RailSupport_Home == OFF)
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
void __fastcall TFoupOnlineCMDForm::btnStopClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnTransMarkForwardClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
			if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
			{
				if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                 	return;
				}

				if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                    return;
				}
			}

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
void __fastcall TFoupOnlineCMDForm::btnTransMarkBackwardClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) &&
				(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right))
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

void __fastcall TFoupOnlineCMDForm::memoJogKeyControlExit(TObject *Sender)
{
    memoJogKeyControl->Color = clMenuHighlight;
    timerDrivingJog->Enabled = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::memoJogKeyControlClick(TObject *Sender)
{
    memoJogKeyControl->Color = COLOR_YELLOW;
}
//---------------------------------------------------------------------------

/**
@brief   JogControl KeyDown시 발생하는 이벤트 함수
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TFoupOnlineCMDForm::memoJogKeyControlKeyDown(TObject *Sender,
	WORD &Key, TShiftState Shift)
{
	if((Key == VK_RIGHT) || (Key == VK_LEFT))
	{
		// 조그 명령 Timer로 반복 실시
		CurJogKey = Key;

//	   if((chbBackWard->Checked ==true)&&(Key == VK_LEFT))
//	   {
//		CurJogKey = 0x0000;
//		timerDrivingJog->Enabled = false;
//		}
//		else
//		{
//		timerDrivingJog->Enabled = true;
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
void __fastcall TFoupOnlineCMDForm::memoJogKeyControlKeyUp(TObject *Sender,
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
void __fastcall TFoupOnlineCMDForm::timerDrivingJogTimer(TObject *Sender)
{
	bool bError = NO_ERR;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD SndTPCmd; // TP Command
	memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

	// 2018.07.24 SHCHO
	// 만약 조그 이동중 VNC Viewer 꺼지면 조그가 계속 이동됨
	// 이유는 Key Up 이벤트가 발생하지 않기 때문에.
	// 하여 Jog Timer에서 키보드 상태를 계속 체크함
	bool bLeftKey_Down = GetAsyncKeyState(VK_LEFT);
	bool bRihtKey_Down = GetAsyncKeyState(VK_RIGHT);
	bool bJogKeyPressed = bLeftKey_Down || bRihtKey_Down;
	bool bVNCRemoteMode = GetVNCThreadCount() >= 5;
	bool bVNCServer = GetVNCThreadCount() >= 0;
	bool bLocalHostMode = SocketForm->OHTSocket->Address == "127.0.0.1";
	bool bVNCDiscon_inLocalMode = bVNCServer && !bVNCRemoteMode && bLocalHostMode;

	// memo에 focus가 됐는지를 판단하는 함수
	if(memoJogKeyControl->Focused() == false || bJogKeyPressed == false || bVNCDiscon_inLocalMode)
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

		MainForm->DataDisplay(COLOR_BLACK, "Driving Stop");
		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_STOP, &SndTPCmd);
		CurJogKey = 0x0000;

        // 반복 명령 Timer 정지
		timerDrivingJog->Enabled = false;

		delete m_CMDControl;
		return;
	}

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
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
		timerDrivingJog->Enabled = false;
		return;
	}

	if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
	{
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
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
		else if(gm_OHTStatus->IOInput->uInputCommon.RailSupport_Home == OFF)
		{
			// by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
			Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
			bError = true;
		}
		*/
	}


	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
	{
		if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
		{
			Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
			timerDrivingJog->Enabled = false;
			return;
		}

	}
	else
	{
		if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
			gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
		{
			Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
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
		////2초이하 & 이상에 따른 속도 조정 코드 추가위치
		//만약 2초보다 작을경우, tareget속도가 저속 속도보다 높을 경우만 적용

		 if((SndTPCmd.CmdTP_Jog_DRV.Velocity > 300))
			  SndTPCmd.CmdTP_Jog_DRV.Velocity = 300;

		//아닐  경우는 원래 속도로 이동
		////

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
// ---------------------------------------------------------------------------

/**
 @brief   Hand Origin버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::btnHandOriginClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnHandResetClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnHandFreeClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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

void __fastcall TFoupOnlineCMDForm::btnSteeringLeftClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        // by zzang9un 2013.06.21 : Steering Left가 Traction으로 되어 있어 변경함
        m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
        MainForm->DataDisplay(COLOR_BLACK, "Steering Left");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void __fastcall TFoupOnlineCMDForm::btnSteeringRightClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnShutterOpenClick(TObject *Sender)
{
    // by zzang9un 2013.02.28 : 변수명이 길어서 지역변수로 대입한 후 사용
	bool Grip_R     = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
	bool Grip_L     = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;
    bool FoupDetect = gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect;

	if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->IOInput->uInputCommon.HAND_Open == ON)
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
void __fastcall TFoupOnlineCMDForm::btnShutterCloseClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        if(fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_cts)) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...Shutter can't Close...");
        }
        else if(fabs(StrToFloat(gm_OHTStatus->Motor_Shift->dPosition_cts)) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...Shutter can't Close...");
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
void __fastcall TFoupOnlineCMDForm::btnShutterOriginClick(TObject *Sender)
{
    // by zzang9un 2013.04.04 : Origin 명령 시에도 경고창 뜨도록 변경(천광현 책임 요청)
	bool Grip_R     = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
	bool Grip_L     = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;
	bool FoupDetect = gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect;

    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        if(gm_OHTStatus->IOInput->uInputCommon.HAND_Open == ON)
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
void __fastcall TFoupOnlineCMDForm::btnShutterFreeClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
int TFoupOnlineCMDForm::Set_UBGColor(int nOutput)
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
 @brief   FoupOnlineCMDForm UI를 주기적으로 갱신하는 Timer 이벤트 함수
 @author  zzang9un
 @date    2013.03.29
 */
void __fastcall TFoupOnlineCMDForm::timerUpdateDisplayTimer(TObject *Sender)
{
    String strVelData;
    double dMotor4Pos, dMotor1Pos;
    String strM4Data, strM1Data;
    String strLR, strUR, strRE, strHO, strES, strEQPIO, strAB;
    String strCS, strVA, strTR, strBU, strCO, strCS1, strCS2, strCS3;
	String strData, strStrTorque;
	AnsiString strTmp;
	strTmp.sprintf("%s", m_VHLNAME);

	static int nVolt2_Count = 0;
	static double nVolt2_total = 0.0;

	//TabSheet1->Visible = false;	// 구 driving 탭을 숨김 (새 프로토콜 적용)
	//TabSheet2->Visible = false; // 구 Trans 탭을 숨김 (새 프로토콜 적용)

	// 조그 이동시 알림 화면 Blink 시킴 ******
	bool bOnJogMoving = timerDrivingJog->Enabled;
	if(bOnJogMoving)
	{
		static bool bPrevStat = false;
		static bool bToggle = false;
		static DWORD dwBlinkTick = 0;
		if(bOnJogMoving != bPrevStat)
		{
			bPrevStat = bOnJogMoving;
			dwBlinkTick = GetTickCount();
		}

		if(GetTickCount() - dwBlinkTick > 300)	// 300 msec
		{
			dwBlinkTick = GetTickCount();
			bToggle = !bToggle;
		}

		pnlJogMoveAlarm->Visible = bToggle;

	}
	else
		pnlJogMoveAlarm->Visible = false;
	// ************


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


	if(panLookdown->Visible == false)
		panLookdown->Visible = true;


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

	if(nVolt2_Count < 25)
	{
	   nVolt2_total = nVolt2_total + (double)((gm_OHTStatus->m_OHTStatus.VolatageMonitor[0]>>16)&0xFFFF)/100;
	   nVolt2_Count++;
	}
	else
	{
		strData.sprintf(L"(%.02f)", (double)(nVolt2_total/nVolt2_Count));
		InputVolt2_1->Caption = strData;
		nVolt2_total = 0.0;
		nVolt2_Count =0;
	}

//	RcvData.VolatageMonitor[0]&0xFFFF, (RcvData.VolatageMonitor[0]>>16)&0xFFFF, RcvData.VolatageMonitor[1]&0xFFFF, (RcvData.VolatageMonitor[1]>>16)&0xFFFF
	//2017.11.28 amc Voltage 확인
	strData.sprintf(L"%.02f", (double)(gm_OHTStatus->m_OHTStatus.VolatageMonitor[0]&0xFFFF)/100);
	InputVolt1->Caption = strData;
	strData.sprintf(L"%.02f", (double)((gm_OHTStatus->m_OHTStatus.VolatageMonitor[0]>>16)&0xFFFF)/100);
	InputVolt2->Caption = strData;
	strData.sprintf(L"%.02f", (double)(gm_OHTStatus->m_OHTStatus.VolatageMonitor[1]&0xFFFF)/100);
	InputVolt3->Caption = strData;
	strData.sprintf(L"%.02f", (double)((gm_OHTStatus->m_OHTStatus.VolatageMonitor[1]>>16)&0xFFFF)/100);
	InputVolt4->Caption = strData;

	shpF_DrivingSen->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front == ON) ? COLOR_LIME : COLOR_WHITE;

	shpBCRDriving->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK == ON) ? COLOR_LIME : COLOR_WHITE;
	shpBCRDriving->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG == ON) ? COLOR_RED :
        shpBCRDriving->Brush->Color;

	shpF_TransSen->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front == ON) ? COLOR_LIME : COLOR_WHITE;
	shpR_TransSen->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear == ON) ? COLOR_LIME : COLOR_WHITE;
	shpBCRTransLt->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK == ON) ? COLOR_LIME : COLOR_WHITE;
	shpBCRTransLt->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG == ON) ? COLOR_RED :
        shpBCRTransLt->Brush->Color;

    // by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
    // RStopDetectPan->Brush->Color = (gm_OHTStatus->IOInput->nRear_StopSensor== 1) ? COLOR_LIME : COLOR_WHITE;

    // ** Hand ***
    // input
	HandOpenLt->Brush->Color    = (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == ON) ? COLOR_LIME : COLOR_WHITE;
	HandInPosLt->Brush->Color   = (gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
	HandCloseLt->Brush->Color   = (gm_OHTStatus->IOInput->uInputCommon.HAND_Close == ON) ? COLOR_LIME : COLOR_WHITE;
	shpHandPushLt->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.HAND_Push == ON) ? COLOR_LIME : COLOR_WHITE;
	HandAlarmLt->Brush->Color   = (gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
    GLLt->Brush->Color          = (gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == ON) ? COLOR_LIME : COLOR_WHITE;
	GRLt->Brush->Color          = (gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == ON) ? COLOR_LIME : COLOR_WHITE;
    HandOriginLt->Color         = (gm_OHTStatus->bIs_Hand_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;
    HandGoLt->Color             = (gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go == 1) ? COLOR_LIME : COLOR_FUCHSIA;
    // output
	grpHand->Color       = (gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free == ON) ? COLOR_RED : (TColor)0x00FFDFDF;
	btnHandFree->Caption = (gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free == ON) ? "[Hand Enable]" : "[Hand Free]";

    // ** Steering **
    // input
	FStrLeftLt->Brush->Color  = (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) ? COLOR_LIME : COLOR_WHITE;
	FStrRightLt->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == ON) ? COLOR_LIME : COLOR_WHITE;
	RStrLeftLt->Brush->Color  = (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON) ? COLOR_LIME : COLOR_WHITE;
    RStrRightLt->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == ON) ? COLOR_LIME : COLOR_WHITE;
    // output
	strStrTorque.sprintf(L"%d %d", gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0,
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1);
    panFStrTorque->Caption = strStrTorque;
	strStrTorque.sprintf(L"%d %d", gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0,
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1);
    panRStrTorque->Caption = strStrTorque;

	// ** Shutter **
	shpShutterOpen_Front->Brush->Color  = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == ON) ? COLOR_LIME : COLOR_WHITE;
	shpShutterOpen_Rear->Brush->Color  = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == ON) ? COLOR_LIME : COLOR_WHITE;
	shpShutterClose_Front->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == ON) ? COLOR_LIME : COLOR_WHITE;
	shpShutterClose_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == ON) ? COLOR_LIME : COLOR_WHITE;
	shpShutterAlarm_Front->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
	shpShutterAlarm_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm == ON) ? COLOR_RED : COLOR_WHITE;
	shpShutterInposition_Front->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
	shpShutterInposition_Rear->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition == ON) ? COLOR_LIME : COLOR_WHITE;
	panShtrOrigin->Color      = (gm_OHTStatus->bIs_Shutter_Origin == true) ? COLOR_LIME : COLOR_FUCHSIA;

	grpShutter->Color = ((gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free == ON) ||
        (gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free == ON)) ? COLOR_RED : (TColor)0x00FFDFDF;
    btnShutterFree->Caption = ((gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free == ON) ||
        (gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free == ON)) ? "[Enable]" : "[Free]";


	// ---- shkim. 2016-03-11 I/O info. add.

	// (1) Out trigger
//	shpOuttrigger_F_Alarm->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_F_Alarm_IN == ON) ? COLOR_LIME : COLOR_WHITE;
//	shpOuttrigger_F_Home->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_F_Home_IN  == ON) ? COLOR_LIME : COLOR_WHITE;
//	shpOuttrigger_F_Work->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_F_Work_IN  == ON) ? COLOR_LIME : COLOR_WHITE;
//
//	shpOuttrigger_R_Alarm->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_R_Alarm_IN == ON) ? COLOR_LIME : COLOR_WHITE;
//	shpOuttrigger_R_Home->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_R_Home_IN  == ON) ? COLOR_LIME : COLOR_WHITE;
//	shpOuttrigger_R_Work->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Outtrigger_R_Work_IN  == ON) ? COLOR_LIME : COLOR_WHITE;

	// (2) CID
	shpCID_Link_Compt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN == ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Interlock->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Pass_OK->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;

	shpCID_Status->Brush->Color		= (gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_1->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpCID_Reserved_2->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2 	== ON) ? COLOR_LIME : COLOR_WHITE;

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == OFF)
		btnCidLeftSel->Caption = "Left Sel On";
	else
		btnCidLeftSel->Caption = "Left Sel Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == OFF)
		btnCidRightSel->Caption = "Right Sel On";
	else
		btnCidRightSel->Caption = "Right Sel Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy == OFF)
		btnCidOccupReq->Caption = "Occup Req On";
	else
		btnCidOccupReq->Caption = "Occup Req Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion == OFF)
		btnCidPassCompt->Caption = "PASS Compt On";
	else
		btnCidPassCompt->Caption = "PASS Compt Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal == OFF)
		btnCidManual->Caption = "Manual On";
	else
		btnCidManual->Caption = "Manual Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode == OFF)
		btnCidOperation->Caption = "Operation On";
	else
		btnCidOperation->Caption = "Operation Off";


	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag == OFF)
		btnCidRev1->Caption = "Common On";
	else
		btnCidRev1->Caption = "Common Off";

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2 == OFF)
		btnCidRev2->Caption = "Rev_OUT2 On";
	else
		btnCidRev2->Caption = "Rev_OUT2 Off";

	// (3) Rotate
//	shpRotate_L_Limit->Brush->Color		= (gm_OHTStatus->IOInput->uInputCommon.Rotate_L_Limit 		== ON) ? COLOR_LIME : COLOR_WHITE;
//	shpRotate_R_Limit->Brush->Color		= (gm_OHTStatus->IOInput->uInputCommon.Rotate_R_Limit 		== ON) ? COLOR_LIME : COLOR_WHITE;
//	shpRotate_Inposition->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.Rotate_Inposition 		== ON) ? COLOR_LIME : COLOR_WHITE;
//	shpRotate_Alarm->Brush->Color		= (gm_OHTStatus->IOInput->uInputCommon.Rotate_Alarm 			== ON) ? COLOR_LIME : COLOR_WHITE;


	// -------------------------------------------
    // [BEGIN] by zzang9un 2013.09.06 : Track Buffer UI
	shpSTBOHTL->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == ON) ? COLOR_LIME : COLOR_WHITE;
	shpSTBOHTR->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == ON) ? COLOR_LIME : COLOR_WHITE;

    // by zzang9un 2013.09.06 : Sation에 마크된 경우에만 색상을 표시
    if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag || gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L ||
        gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R)
    {
        // by zzang9un 2013.09.13 : Foup 여부에 따라 색상을 다르게 표시
        if(gm_OHTStatus->m_OHTStatus.Data.ExistFoup)
        {
			if(gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left)
                panSTBLeftTransAvailable->Color = COLOR_WHITE;
            else
                panSTBLeftTransAvailable->Color = COLOR_LIME;

			if(gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right)
                panSTBRightTransAvailable->Color = COLOR_WHITE;
            else
                panSTBRightTransAvailable->Color = COLOR_LIME;

            if(gm_OHTStatus->IOInput->uInputCommon.UTB_Detect)
                panUTBTransAvailable->Color = COLOR_WHITE;
            else
                panUTBTransAvailable->Color = COLOR_LIME;
        }
        else
        {
			if(gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left)
                panSTBLeftTransAvailable->Color = COLOR_LIME;
            else
                panSTBLeftTransAvailable->Color = COLOR_WHITE;

			if(gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right)
                panSTBRightTransAvailable->Color = COLOR_LIME;
            else
                panSTBRightTransAvailable->Color = COLOR_WHITE;

            if(gm_OHTStatus->IOInput->uInputCommon.UTB_Detect)
                panUTBTransAvailable->Color = COLOR_LIME;
            else
                panUTBTransAvailable->Color = COLOR_WHITE;
        }
    }
    else
    {
        panSTBLeftTransAvailable->Color = clBtnFace;
        panSTBRightTransAvailable->Color = clBtnFace;
        panUTBTransAvailable->Color = clBtnFace;
    }
    // [END] by zzang9un 2013.09.06 : Track Buffer UI
    // -------------------------------------------

    // ** ETC **
	panHoist_Home->Color = (gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == ON) ? COLOR_LIME : COLOR_WHITE;
    panOscillation->Color = (gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == ON) ? COLOR_LIME : COLOR_WHITE;

	panLookdown->Color = (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == ON) ? COLOR_LIME : COLOR_WHITE;
	panLookdown->Color = (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error == ON) ? COLOR_RED : panLookdown->Color;

	panOBSLong->Color   = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long == ON) ? COLOR_LIME : COLOR_WHITE;
	panOBSMiddle->Color = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle == ON) ? COLOR_LIME : COLOR_WHITE;
	panOBSShort->Color  = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short == ON) ? COLOR_LIME : COLOR_WHITE;
	panOBSFail->Color   = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail == ON) ? COLOR_RED : COLOR_WHITE;

    // 추가 puting 2012.11.27
//	panMAC_Detect->Color  = (gm_OHTStatus->IOInput->uInputCommon.MAC_Detect == ON) ? COLOR_LIME : COLOR_WHITE;
	panHand_TP_Con->Color = (gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect == ON) ? COLOR_LIME : COLOR_WHITE;

//	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
////	    panMC_Status->Color   = (gm_OHTStatus->IOInput->uInputCommon.MC_Status == ON) ? COLOR_LIME : COLOR_WHITE;
//	else
//		panMC_Status->Color   = (gm_OHTStatus->IOInput->uInputCommon.MC_Status == ON) ? COLOR_LIME : COLOR_WHITE;

    // 추가 puting 2012.11.27
    if(panUBGMiddle->Visible == false)
        panUBGMiddle->Visible = true;
    if(panUBGShort->Visible == false)
        panUBGShort->Visible = true;
    if(panUBGFail->Visible == false)
        panUBGFail->Visible = true;
	panUBGLong->Color   = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long == ON) ? COLOR_LIME : COLOR_WHITE;
	panUBGMiddle->Color = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle == ON) ? COLOR_LIME : COLOR_WHITE;
	panUBGShort->Color  = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short == ON) ? COLOR_LIME : COLOR_WHITE;
	panUBGFail->Color   = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail == ON) ? COLOR_RED : COLOR_WHITE;

    // 수정 puting 2012.11.27
    if(panFoupDetect->Visible == false)
        panFoupDetect->Visible = true;
    panFoupDetect->Color = (gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == ON) ? COLOR_LIME : COLOR_WHITE;

    if(panShift_Home->Visible == false)
        panShift_Home->Visible = true;
	panShift_Home->Color = (gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == ON) ? COLOR_LIME : COLOR_WHITE;

    if(panUTB->Visible == false)
        panUTB->Visible = true;
	panUTB->Color = (gm_OHTStatus->IOInput->uInputCommon.UTB_Detect == ON) ? COLOR_LIME : COLOR_WHITE;

	panHoist_Belt->Color = (gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1 == ON) ? COLOR_LIME : COLOR_WHITE;

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

    panHIDError->Color = (strData.Pos("1") != 0) ? COLOR_RED : clBtnFace;

	// ** Power ON/OFF
    int j                 = 0;
	PWGrid->Cells[1][j++] = (gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset == ON) ? "OFF" : "ON";

    // *** Hand ***
    // 2017.01.04 fix  if(gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset == 1)
	if(gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset == 1)
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

	// 2017.01.04.  TRQ_1만 보도록 해야 한다. I/O fix
	/*
	nTr1 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1;

	if((nTr1 == 1) && (nTr2 == 1))
	*/

	nTr1 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1;

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

	// 2017.01.04.  TRQ_1만 보도록 해야 한다. I/O fix
	/*
	nTr1 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1;

    if((nTr1 == 1) && (nTr2 == 1))
    */

	nTr1 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0;
	nTr2 = gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1;

	if(nTr2 == 0)
    {
        btnRSteerON->Enabled  = true;
        btnRSteerOFF->Enabled = false;
    }
    else
    {
        btnRSteerON->Enabled  = false;
        btnRSteerOFF->Enabled = true;
    }

	strLR = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ == ON) ? "L" : "0";
	strUR = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ == ON) ? "U" : "0";
	strRE = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready == ON) ? "R" : "0";
    if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E84)
    {
		strHO = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL == ON) ? "H" : "0";
        strES = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES == ON) ? "E" : "0";

        strEQPIO.sprintf(L"%s %s %s %s %s", strLR, strUR, strRE, strHO, strES);
	}
    else
	{
        // by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
        // strAB = (gm_OHTStatus->IOInput->nPIO_ABORT==ON) ? "A" : "0";
        // strEQPIO.sprintf(L"%s %s %s %s", strLR, strUR, strAB, strRE);
    }

    panEQPIOIn->Caption = strEQPIO;

    strCS = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0 == ON) ? "C" : "0";
    strVA = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid == ON) ? "V" : "0";
	strTR = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req == ON) ? "T" : "0";
    strBU = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy == ON) ? "B" : "0";
    strCO = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete == ON) ? "C" : "0";
    if(gm_OHTStatus->nIsRFPIOType == PIOTYPE_E23)
    {
		strCS1 = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1 == ON) ? "1" : "0";
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

    if(gm_OHTStatus->IOInput->uInputCommon.FAN_Fail)
	{
		panName->Color   = COLOR_RED;
		panName->Caption = "[Warnning] Fan Fail";
    }
    else
    {
		panName->Color   = COLOR_BLUE;
		panName->Font->Size = 18;
	   //	panName->Caption ="["+SocketForm->m_strOHTName+"]" + " Connect...:" + SocketForm->OHTSocket->Address;
	   panName->Caption  = "["+strTmp+"]" + " Connect...:" + SocketForm->OHTSocket->Address;
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
void __fastcall TFoupOnlineCMDForm::btnStopTagBCRClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnMotor2HomeClick(TObject *Sender)
{
    CmdFoupOHTCheck.Axis_Hoist_HOME();
}
// ---------------------------------------------------------------------------

/*
i226 = 3000 * 16 값 대입 -> 만약 i226이 zero가 아니면 기존값에 + 또는 -함
i226 = (M262/3072) * 16
*/
void __fastcall TFoupOnlineCMDForm::mniHoistOriginSetClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_ORIGIN_SETTING);
        MainForm->DataDisplay(COLOR_BLACK, "CMD Hoist Origin Set");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniShiftOriginSetClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::edtSpeedChange(TObject *Sender)
{
    double dVel_ctspermsec, dVel_mpersec, dVel_Cen_mpersec, dVel_mpermin;
    double d1RotateDist_mm = 0.0;
    if(edtSpeed->Text == "")
        return;


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
void __fastcall TFoupOnlineCMDForm::btnMoveStopClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ALLROBOT_STOP);
        MainForm->DataDisplay(COLOR_BLACK, "JOG_ALLROBOT_STOP");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::FormCreate(TObject *Sender)
{
    int j                         = 0;
    FoupOnlineCMDForm->Top            = MainForm->Top;
    FoupOnlineCMDForm->Left           = MainForm->Left + MainForm->Width + StatusForm->Width;
    PWGrid->Cells[0][j++]         = "Steering Power[M221]";
    PWGrid->Cells[0][j++]         = "Shutter Power[M222]";
    PWGrid->Cells[0][j++]         = "Hand Power[M223]";
    chkLongRun->Checked           = false;
    chkDist->Checked              = false;
    pgcMainControl->ActivePageIndex = 0;

    timerAllDisable();
}

// ---------------------------------------------------------------------------
void __fastcall TFoupOnlineCMDForm::pgcMainControlDrawTab(
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
void TFoupOnlineCMDForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::PWGridDrawCell(TObject *Sender, int ACol,
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
void __fastcall TFoupOnlineCMDForm::btnHaONClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnHaOFFClick(TObject *Sender)
{
	if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnHaRstClick(TObject *Sender)
{
	if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::timerTransLongRunTimer(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::chkLongRunClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnShutterAlarmResetClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnMotor34HomeClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    int nTmpRotateAccel, nTmpRotateSpeed;
    String strTmpData;

	if((gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == ON) &&
        (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_cts)) < HOIST_ORIGIN_LIMIT))
    {
		if((gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == ON) &&
            (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == ON))
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

    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
/**
 @brief  Trans Load버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::btnLoadMouseDown(TObject *Sender,
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
void __fastcall TFoupOnlineCMDForm::btnUnloadMouseDown(TObject *Sender,
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

void __fastcall TFoupOnlineCMDForm::pmDrivingFServoOnOffPopup(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::pmDrivingRServoOnOffPopup(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::pmHoistServoOnOffPopup(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::pmShiftServoOnOffPopup(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::pmRotatePopup(TObject *Sender)
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

void __fastcall TFoupOnlineCMDForm::mniDrvFServoOnClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniDrvFServoOffClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniDrivingRServoOnClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniDrivingRServoOffClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniHoistServoOnClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniHoistServoOffClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniShiftServoOnClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniShiftServoOffClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniRotateServoOnClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_ON);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniRotateServoOffClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_OFF);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniRotateOriginSetClick(TObject *Sender)
{
    RotateOrgForm->ShowModal();
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Origin버튼 클릭시 발생하는 이벤트 함수(GO_ORIGIN)
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::btnRotate_OrignClick(TObject *Sender)
{
    bool bExec = false;

    // if((CmdFoupOHTCheck.Is_Executable()==NO_ERR)&&(CmdFoupOHTCheck.Is_MoveAxis_Rotate()==NO_ERR))
	if((gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == ON) &&
        (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_cts)) < HOIST_ORIGIN_LIMIT))
    {
        if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
        {
			if((gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == OFF) &&
				(gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == OFF))
                bExec = true;
            else
            {
				if((gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == ON) &&
                    (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == ON))
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
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}

// ---------------------------------------------------------------------------
/**
 @brief   Rotate Enable버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::btnRotate_EnableClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate_DisableClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate_StopClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnFSteerONClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnFSteerOFFClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnRSteerONClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnRSteerOFFClick(TObject *Sender)
{
	PowerOnOff(AXIS_REAR, PWR_OFF);

#if 0
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_R_FREE);
    delete m_CMDControl;
#endif
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::chkDistClick(TObject *Sender)
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
int TFoupOnlineCMDForm::DistChang_mm_2(void)
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
int TFoupOnlineCMDForm::DistChang_cts_2(void)
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
void __fastcall TFoupOnlineCMDForm::edtDistmmChange(TObject *Sender)
{
    DistChang_mm_2();
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::edtDistctsChange(TObject *Sender)
{
    DistChang_cts_2();
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnMoveClick(TObject *Sender)
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
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
		return;
	}

	if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
	{
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
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
		else if(gm_OHTStatus->IOInput->uInputCommon.RailSupport_Home == OFF)
		{
			// by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
			Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
			return;
		}
		*/
	}

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
	{
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
		{
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
				return;
			}

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
				return;
			}
		}
		else
		{
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
				return;
			}

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
				return;
			}
		}
	}
	else
	{

			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
				return;
			}

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
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
void __fastcall TFoupOnlineCMDForm::btnScanUTBClick(TObject *Sender)
{
    String strTmpMessage;

    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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

                if(FoupOnlineCMDForm->chkLongRun->Checked == true)
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

void __fastcall TFoupOnlineCMDForm::btnTractionLeftClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_TRACTION_STEER_LEFT);
        MainForm->DataDisplay(COLOR_BLACK, "Traction Mode Steering Left");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnTractionRightClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

        m_CMDControl->MakeTPCmd2OHT(TPCMD_TRACTION_STEER_RIGHT);
        MainForm->DataDisplay(COLOR_BLACK, "Traction Mode Steering Right");

        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::chkTractionModeClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnRotateOriginClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotateResetClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate0Click(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate90Click(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate180Click(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotate270Click(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnRotateFreeClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnMCControlOnClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnMCControlOffClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnSTB_R_WrokOnClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnSTB_R_WorkOffClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnSTB_L_WrokOnClick(TObject *Sender)
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
void __fastcall TFoupOnlineCMDForm::btnSTB_L_WrokOffClick(TObject *Sender)
{
    ///추가 puting 2012.11.28
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_STB_L_WORK_OFF);
    delete m_CMDControl;
}

/**
 @brief   Driving(주행) MaRK BCR버튼 클릭시 발생하는 이벤트 함수
 @author  puting
 @date    2012.12.14
 */
void __fastcall TFoupOnlineCMDForm::btnDrivingBCRClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnDrivingMarkBackwardClick(
    TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) &&
				(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right))
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
void __fastcall TFoupOnlineCMDForm::btnDrivingMarkForwardClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
			if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
			{
				if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                 	return;
				}

			}

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
BYTE TFoupOnlineCMDForm::GetTransNodeType(int index)
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
void __fastcall TFoupOnlineCMDForm::chkGoStationClick(TObject *Sender)
{
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
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadEQClick(TObject *Sender)
{
	// by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
		TransForm->SetTrans(CMD_LOAD, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadEQ_XClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadSTOCKERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadSTB_LClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadSTB_RClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadUTBClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_UTB);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniLoadLOADERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_LOAD, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadEQClick(TObject *Sender)
{
	// by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    // shkim  임시 주의!!!!!  if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadEQ_XClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadSTOCKERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadLOADERClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadSTB_LClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadSTB_RClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_UNLOAD, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniUnloadUTBClick(TObject *Sender)
{
    // by zzang9un 2012.12.02 : 새로 변경한 TransForm을 이용하도록 수정
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
void __fastcall TFoupOnlineCMDForm::btnQRTagLeftMarkClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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
		else if(gm_OHTStatus->IOInput->uInputCommon.RailSupport_Home == OFF)
		{
			// by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
			Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
		}
		*/
		else
		{
			if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
			{
				if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                 	return;
				}

				if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                    return;
				}
			}

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
void __fastcall TFoupOnlineCMDForm::btnQRTagRightMarkClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
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
			if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
			{
				if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
					gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                 	return;
				}

				if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
					gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
				{
					Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                    return;
				}
			}

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_QR_RIGHT_MARK_FORWARD);
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------
void __fastcall TFoupOnlineCMDForm::panTeachingSaveClick(TObject *Sender)
{
    TeachingForm->Show();
    TeachingForm->pgcTeaching->ActivePageIndex = 1;
}
//---------------------------------------------------------------------------

/**
@brief   FoupOnlineCMDForm의 Timer를 Enable하는 함수
@author  zzang9un
@date    2013.11.20
*/
void TFoupOnlineCMDForm::timerAllDisable()
{
    timerUpdateDisplay->Enabled = false;
    timerDrivingJog->Enabled = false;
    timerTransLongRun->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnMoveTeachingPointMouseDown(TObject *Sender, TMouseButton Button,
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

void __fastcall TFoupOnlineCMDForm::mniMoveTPEQClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_EQ);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPEQ_XClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_EQ_X);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPSTOCKERClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STOCKER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPLOADERClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_LOADER);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPSTB_LClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STB_L);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPSTB_RClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_STB_R);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::mniMoveTPUTBClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
    {
        // TransForm을 Show하기 전 기본 이적재 정보를 Setting한다.
        TransForm->SetTrans(CMD_MOVETPOINT, PORT_TYPE_UTB);
        TransForm->ShowModal();
    }
}
//---------------------------------------------------------------------------


void __fastcall TFoupOnlineCMDForm::btnHoistHomeAutoSetClick(TObject *Sender)
{
   if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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

		SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTO_HOISTHOME;

		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

		delete m_CMDControl;
    }
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

void TFoupOnlineCMDForm::Display_CID_INFO(PKT_OHT2TP_CID_INFO PKT)
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

void TFoupOnlineCMDForm::Display_MapMove_Result(PKT_OHT2TP_MOVE_BY_MAP_RESULT PKT)
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

void TFoupOnlineCMDForm::Display_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT)
{
	AnsiString strResult;
	strResult.sprintf("[Map Make] %s", PKT.BODY.Log);
	mmCom->Lines->Add(strResult);
}

void TFoupOnlineCMDForm::Display_Com(PKT_OHT2OCS PKT)
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

void __fastcall TFoupOnlineCMDForm::btnForceOpenClick(TObject *Sender)
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


int TFoupOnlineCMDForm::TransLoad(void)
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

	SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;

	SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;		// Driving flag

	// ====================================================================================================
	// D사양서 p34 - "목적 스테이션이 없는 경우는 0을 설정한다. 이 경우, 경로 데이터도 모두 0으로 채운다"
	// shkim.  올리고 내리는 것이므로 노드번호, 목적지 스테이션 번로는 모두 0으로 채워도 될듯 (확인필요)
	// ==> 일단 검증을 해 보고 적용하도록 하자


	SEND_PACKET.BODY.PathData_ExtraNum = 0; 	//
	// ----------------------------------------------------------------------


	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	return iReturn;
}

//---------------------------------------------------------------------------



int TFoupOnlineCMDForm::TransUnLoad(void)
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

	SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;

	SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

	// ====================================================================================================
	// D사양서 p34 - "목적 스테이션이 없는 경우는 0을 설정한다. 이 경우, 경로 데이터도 모두 0으로 채운다"
	// shkim.  올리고 내리는 것이므로 노드번호, 목적지 스테이션 번로는 모두 0으로 채워도 될듯 (확인필요)
	// ==> 일단 검증을 해 보고 적용하도록 하자

	SEND_PACKET.BODY.PathData_ExtraNum = 0;
	// ----------------------------------------------------------------------

	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	return iReturn;
}
//---------------------------------------------------------------------------
// 이 함수는 필요하다면 TFoupOnlineCMDForm::Display_Com(PKT_OHT2OCS PKT)
// 을 참고하여 보강한다
void __fastcall TFoupOnlineCMDForm::tmrCidInfoReadTimer(TObject *Sender)
{
	tmrCidInfoRead->Enabled = false;	// timer off
	btnCidMonitoring->Enabled = true;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidMonitoringClick(TObject *Sender)
{

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == ON)
	{
		btnCidRightSelClick(NULL);
		Sleep(200);
	}

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == OFF)
	{
		btnCidLeftSelClick(NULL);
		Sleep(200);
	}

	iCidInfo_Direct = 0;	// left

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	Sleep(300);

	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnMapBaseMoveClick(TObject *Sender)
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
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
    {
        Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
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

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
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
//	SEND_PACKET.BODY.StartNodeID = StrToInt (edtMoveStart->Text);

	// 목적지 번호 입력
	SEND_PACKET.BODY.StopNodeID = StrToInt (edtMoveEnd->Text);

	// 데이터 전송
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");

}
//---------------------------------------------------------------------------



void __fastcall TFoupOnlineCMDForm::btnCidMonitoring_RightClick(TObject *Sender)
{
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == ON)
	{
		btnCidLeftSelClick(NULL);
		Sleep(200);
    }

	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == OFF)
	{
		btnCidRightSelClick(NULL);
		Sleep(200);
	}

	iCidInfo_Direct = 1;	// right

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_CMD);
	Sleep(300);
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_CID_MONITORING_READ);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void TFoupOnlineCMDForm::PowerOnOff(int iAxis, int iOnOff)
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

void __fastcall TFoupOnlineCMDForm::btnMoveAllHomeClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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

void __fastcall TFoupOnlineCMDForm::btnAutoChangeClick(TObject *Sender)
{
    if(CmdFoupOHTCheck.Is_Executable() == NO_ERR)
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


void __fastcall TFoupOnlineCMDForm::btnOCSMapBaseMoveClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	unsigned char cHigh, cMid, cLow;
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
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
    {
        Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
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

	if (cbCheckGoStation->Checked== false)
	{
			Warnning_Display(COLOR_RED, "ERROR : Station No Checked...");
			return;
	}


    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
        }
    }

	// 새로운 TP 이동명령 패킷을 이용한다.
	iPacketSize = sizeof(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111);
	PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);


	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_STATIONINFOREQ_TP2OHT; // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)

	// ---- Body Set
	// 도달점 노드 타입 입력
	// 목적지 STATION
     int  Tmepstation =  (unsigned int)StrToInt (edtMoveEnd->Text);
	 for(int Index =0; Index< 3; Index++)
	{

		SEND_PACKET.BODY.StopStationID[2-Index] = (Tmepstation >> (Index * 8))&0xFF;
    }

	/*
	Int2Uchar3((unsigned int)StrToInt (edtMoveEnd->Text), cHigh, cMid, cLow);

	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;
	*/

	SEND_PACKET.BODY.RequestDiv = 0x00;



	// 데이터 전송
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------








void __fastcall TFoupOnlineCMDForm::btnCidLeftSelClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL);
	}
	else
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL_OFF);
	}

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidRightSelClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL);
	}else
	{
        m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL_OFF);
	}
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidOccupReqClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ);
	}else
	{
	  m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ_OFF);
	}

    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidPassComptClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT);
	}
	else
	{
        m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT_OFF);
	}

    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidManualClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal == OFF)
	{
       m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL);
	}
	else
	{
       m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL_OFF);
	}

    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidOperationClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION);
	}
	else
	{
	   m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION_OFF);
	}

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidRev1Click(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1);
	}
	else
	{
        m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1_OFF);
	}
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TFoupOnlineCMDForm::btnCidRev2Click(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	if(gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2 == OFF)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2);
	}
	else
	{
        m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2_OFF);
	}
	delete m_CMDControl;
}
//---------------------------------------------------------------------------



