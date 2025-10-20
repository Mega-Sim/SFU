// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "IOTest.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Define.h"
#include "Def_TPCommand.h"
#include "OHTInfoUnit.h"
#include "WarnningUnit.h"
#include "StatusUnit.h"
#include "ChkExecUnit.h"
#include "SetCurnodeNUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIOTestForm *IOTestForm;
TCHKOHT IOOHTCheck;

// ---------------------------------------------------------------------------
__fastcall TIOTestForm::TIOTestForm(TComponent* Owner) : TForm(Owner)
{
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::FormShow(TObject *Sender)
{
    IOTestForm->Top = MainForm->Top + MainForm->Height;
    IOTestForm->Left = MainForm->Left;
    IOTestForm->Width = MainForm->Width + StatusForm->Width;
    if(IOTestTimer->Enabled == false)
        IOTestTimer->Enabled = true;
}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if(IOTestTimer->Enabled == true)
        IOTestTimer->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::SoundOnBtnClick(TObject *Sender)
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

	SEND_PACKET.TAG.Value = TagID::CMD_TP_SOUND_ONOFF;
	SEND_PACKET.BODY.iVar1 = 0x01;	// sound on

	switch(SoundRadioGroup->ItemIndex)
	{
		case 0:
			SEND_PACKET.BODY.iVar2 = SOUND1;
		break;

		case 1:
			SEND_PACKET.BODY.iVar2 = SOUND2;
		break;

		case 2:
			SEND_PACKET.BODY.iVar2 = SOUND3;
		break;

		case 3:
			SEND_PACKET.BODY.iVar2 = SOUND4;
		break;
    }

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;


#if 0
    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        int nSoundIndex = 0;
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        String strTmpData;

        switch(SoundRadioGroup->ItemIndex)
        {
        case 0:
            nSoundIndex = SOUND1;
            TPCmd.TPCmd.Data1 = SOUND1;
            break;

        case 1:
            nSoundIndex = SOUND2;
            TPCmd.TPCmd.Data1 = SOUND2;
            break;

        case 2:
            nSoundIndex = SOUND3;
            TPCmd.TPCmd.Data1 = SOUND3;
            break;

        case 3:
            nSoundIndex = SOUND4;
            TPCmd.TPCmd.Data1 = SOUND4;
            break;
        }

        if(nSoundIndex != 0)
        {
            strTmpData.sprintf(L"%d", nSoundIndex);
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SOUND_ON, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_SOUND_ON, strTmpData);
            MainForm->DataDisplay(COLOR_BLACK, "Sound" + IntToStr(nSoundIndex) + " ON");
        }
        delete m_CMDControl;
    }
#endif

}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::SoundOffBtnClick(TObject *Sender)
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

	SEND_PACKET.TAG.Value = TagID::CMD_TP_SOUND_ONOFF;
	SEND_PACKET.BODY.iVar1 = 0x02;	// sound off

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

#if 0
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SOUND_OFF);
        MainForm->DataDisplay(COLOR_BLACK, "Sound OFF");
        delete m_CMDControl;
    }
#endif
}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::IOTestTimerTimer(TObject *Sender)
{
    String strHandPIO, strEQPIO, strServoAlarm;
    int nObsArea[5], nDetectArea[5];
    String strTmpData;

    BCRPanel->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID);

	shpObsLong->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long == 1) ? COLOR_LIME : COLOR_WHITE;
	shpObsMiddle->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle == 1) ? COLOR_LIME : COLOR_WHITE;
	shpObsShort->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short == 1) ? COLOR_LIME : COLOR_WHITE;
    shpObsTrouble->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail == 1) ? COLOR_RED : COLOR_WHITE;


	if(gm_OHTStatus->nIsUBGSenUse == 1)
	{
		if(GroupBox2->Visible == false)
			GroupBox2->Visible = true;
		if(GroupBox4->Visible == true)
			GroupBox4->Visible = false;

		shpDetectLong->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long == 1) ? COLOR_LIME : COLOR_WHITE;
		shpDetectMiddle->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle == 1) ? COLOR_LIME : COLOR_WHITE;
		shpDetectShort->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short == 1) ? COLOR_LIME : COLOR_WHITE;
		shpDetectTrouble->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail == 1) ? COLOR_RED : COLOR_WHITE;
	}
	else
	{
		if(GroupBox2->Visible == true)
			GroupBox2->Visible = false;
		if(GroupBox4->Visible == false)
			GroupBox4->Visible = true;

		// by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
		// shpDetect->Brush->Color    = (gm_OHTStatus->IOInput->nOHTDetect_Sesnor == 1) ? COLOR_LIME : COLOR_WHITE;
	}



	if(GroupBox6->Visible == true)
		GroupBox6->Visible = false;



    shpAlarm1_F->Brush->Color = (gm_OHTStatus->Motor_Driving_F->nError == 1) ? COLOR_RED : COLOR_WHITE;
    shpAlarm1_R->Brush->Color = (gm_OHTStatus->Motor_Driving_R->nError == 1) ? COLOR_RED : COLOR_WHITE;
    shpAlarm2->Brush->Color = (gm_OHTStatus->Motor_Hoist->nError == 1) ? COLOR_RED : COLOR_WHITE;
    shpAlarm3->Brush->Color = (gm_OHTStatus->Motor_Shift->nError == 1) ? COLOR_RED : COLOR_WHITE;
    shpAlarm4->Brush->Color = (gm_OHTStatus->Motor_Rotate->nError == 1) ? COLOR_RED : COLOR_WHITE;

	if(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset == 1) // power off
    {
		btnObsPwrOn->Enabled = true;
        btnObsPwrOff->Enabled = false;
    }
    else
    {
        btnObsPwrOn->Enabled = false;
        btnObsPwrOff->Enabled = true;
    }

    if(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset == 1) // power off
    {
        btnDetectPwrOn->Enabled = true;
        btnDetectPwrOff->Enabled = false;
    }
    else
    {
        btnDetectPwrOn->Enabled = false;
        btnDetectPwrOff->Enabled = true;
    }

    // *** Front Obs Enable / Disable ********************************************
    nObsArea[1] = (gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel == 0) ? 1 : 0;
	nObsArea[2] = (gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel == 0) ? 2 : 0;
	nObsArea[3] = (gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel == 0) ? 4 : 0;
	nObsArea[4] = (gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel == 0) ? 8 : 0;

    nObsArea[0] = nObsArea[1] + nObsArea[2] + nObsArea[3] + nObsArea[4];

    if((nObsArea[0] >= 1) && (nObsArea[0] <= 15))
    {
        strTmpData.sprintf(L"[%d]", nObsArea[0]);
        panObs->Caption = strTmpData;
    }
    else
    {
        panObs->Caption = "OFF";
        lblObs->Caption = "□";
    }

    if(panObs->Caption != "OFF")
    {
        if(lblObs->Caption == "□")
            lblObs->Caption = "◇";
        else if(lblObs->Caption == "◇")
            lblObs->Caption = "○";
        else if(lblObs->Caption == "○")
            lblObs->Caption = "□";
    }

    // *** Front Detect Enable / Disable *****************************************
    nDetectArea[1] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel == 0) ? 1 : 0;
    nDetectArea[2] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel == 0) ? 2 : 0;
    nDetectArea[3] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel == 0) ? 4 : 0;
    nDetectArea[4] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel == 0) ? 8 : 0;

    nDetectArea[0] = nDetectArea[1] + nDetectArea[2] + nDetectArea[3] + nDetectArea[4];

    if((nDetectArea[0] >= 1) && (nDetectArea[0] <= 15))
    {
        strTmpData.sprintf(L"[%d]", nDetectArea[0]);
        panDetect->Caption = strTmpData;
    }
    else
    {
        panDetect->Caption = "OFF";
        lblDetect->Caption = "□";
    }

    if(panDetect->Caption != "OFF")
    {
        if(lblDetect->Caption == "∪")
            lblDetect->Caption = "⊂";
        else if(lblDetect->Caption == "⊂")
            lblDetect->Caption = "∩";
        else if(lblDetect->Caption == "∩")
            lblDetect->Caption = "⊃";
        else if(lblDetect->Caption == "⊃")
            lblDetect->Caption = "∪";

    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::ReadBCRBtnClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_SETCURNODE); // 주행 BCR
        MainForm->DataDisplay(COLOR_BLACK, "BCR Read");

        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void TIOTestForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::Button8Click(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_SETCURNODE);
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::Button9Click(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        int nTmpCurNode;
        String strTmpCMD;

        nTmpCurNode = StrToIntDef(edtCurNode->Text, 0);

        if(nTmpCurNode == 0)
        {
            Warnning_Display(COLOR_RED, "ERROR : CMD Current Node is zero...");
        }
        else
        {
            SetCurForm->nCmdCurnode = nTmpCurNode;
            SetCurForm->ShowModal();
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnAlarmResetClick(TObject *Sender)
{
    // if(IOOHTCheck.Is_Executable()==NO_ERR)
    // {
    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

    switch(rgServoAlarm->ItemIndex)
    {
    case 0:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_DRV_F_RESET);
        break;
    case 1:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_DRV_R_RESET);
        break;
    case 2:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_HOIST_RESET);
        break;
    case 3:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_SHIFT_RESET);
        break;
    case 4:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_ROTATE_RESET);
        break;
    case 5:
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SERVO_RESET);
        break;
    }

    MainForm->DataDisplay(COLOR_BLUE, "Servo Reset");
    delete m_CMDControl;
    // }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnObsPwrOnClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_OBSTACLE_POWER_ON);
        MainForm->DataDisplay(COLOR_BLACK, "Obstacle Sensor Power ON");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnObsPwrOffClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_OBSTACLE_POWER_OFF);
        MainForm->DataDisplay(COLOR_BLACK, "Obstacle Sensor Power OFF");
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void __fastcall TIOTestForm::btnObsEnableClick(TObject *Sender)
{
    int nCmd = TPCMD_OHTDETECT_AREA1 + comboObs->ItemIndex;

    if((IOOHTCheck.Is_Executable() == NO_ERR) && (comboObs->ItemIndex != -1))
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(nCmd);
        MainForm->DataDisplay(COLOR_BLACK, "Obstacle Sensor Area :" + IntToStr(comboObs->ItemIndex + 1));
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnDetectPwrOnClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_POWER_ON);
        MainForm->DataDisplay(COLOR_BLACK, "OHT Detect Sensor Power ON");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnDetectPwrOffClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_POWER_OFF);
        MainForm->DataDisplay(COLOR_BLACK, "OHT Detect Sensor Power OFF");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnDetectEnableClick(TObject *Sender)
{
    int nCmd = TPCMD_OHTDETECT_AREA1 + comboDetect->ItemIndex;

    if((IOOHTCheck.Is_Executable() == NO_ERR) && (comboDetect->ItemIndex != -1))
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(nCmd);
        MainForm->DataDisplay(COLOR_BLACK, "OHT Detect Sensor Area :" + IntToStr(comboDetect->ItemIndex + 1));
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::Button1Click(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_LOOKDOWN_ENABLE);
        MainForm->DataDisplay(COLOR_BLACK, "LookDown Sensor Enable");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::Button2Click(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_LOOKDOWN_DISABLE);
        MainForm->DataDisplay(COLOR_BLACK, "LookDown Sensor Disable");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnEQPIOEnableClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_SELECT_ON);
        m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_SELECT_ON);
        MainForm->DataDisplay(COLOR_BLACK, "EQ PIO Enable");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnEQPIODisableClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_SELECT_OFF);
        m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_SELECT_OFF);
        MainForm->DataDisplay(COLOR_BLACK, "EQ PIO Disable");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnHPIOEnableClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_PIO_SELECT_ON);
        MainForm->DataDisplay(COLOR_BLACK, "Hand PIO Enable");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnHPIODisableClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_PIO_SELECT_OFF);
        MainForm->DataDisplay(COLOR_BLACK, "EQ PIO Disable");
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnServoONClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

        switch(rgServoAlarm->ItemIndex)
        {
        case 0:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_ON);
            break;
        case 1:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_ON);
            break;
        case 2:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_ON);
            break;
        case 3:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_ON);
            break;
        case 4:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_ON);
            break;
        case 5:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ALL_SERVO_ON);
            break;
        }

        MainForm->DataDisplay(COLOR_BLUE, "Servo ON");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TIOTestForm::btnServoOFFClick(TObject *Sender)
{
    if(IOOHTCheck.Is_Executable() == NO_ERR)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

        switch(rgServoAlarm->ItemIndex)
        {
        case 0:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_F_SERVO_OFF);
            break;
        case 1:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_DRIVING_R_SERVO_OFF);
            break;
        case 2:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SERVO_OFF);
            break;
        case 3:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SERVO_OFF);
            break;
        case 4:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_SERVO_OFF);
            break;
        case 5:
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ALL_SERVO_OFF);
            break;
        }

        MainForm->DataDisplay(COLOR_BLUE, "Servo OFF");
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------
