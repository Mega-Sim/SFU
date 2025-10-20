// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "M2AUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TPCommand.h"
#include "CMDControlUnit.h"
#include "MainUnit.h"
#include "Define.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TM2AForm *M2AForm;

// ---------------------------------------------------------------------------
__fastcall TM2AForm::TM2AForm(TComponent *Owner)
    :
    TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void TM2AForm::IO_Display_Init()
{
    int i, j, k;

    for(i = 0; i <= 18; i++)
    {
        CheckONOFFGrid->Cells[0][i] = IntToStr(i + 1);
    }

    // *** Check ON/OFF ***
    k = 1;
    j                             = 0;
    CheckONOFFGrid->Cells[k][j++] = "Use BCR";
    CheckONOFFGrid->Cells[k][j++] = "Use PBS Sensor";
    CheckONOFFGrid->Cells[k][j++] = "Use SoundAlarm";
    CheckONOFFGrid->Cells[k][j++] = "Use LampIndi";
    CheckONOFFGrid->Cells[k][j++] = "Use Shutter Control";
    CheckONOFFGrid->Cells[k][j++] = "Use EQPIO";

    CheckONOFFGrid->Cells[k][j++] = "Use STBSensor";

    CheckONOFFGrid->Cells[k][j++] = "Use HandLimitSensor";
    CheckONOFFGrid->Cells[k][j++] = "Use HandControl";
    CheckONOFFGrid->Cells[k][j++] = "Use SteeringControl";

    CheckONOFFGrid->Cells[k][j++] = "Use LookDownSensor";

    CheckONOFFGrid->Cells[k][j++] = "Use RemoteControl";
    CheckONOFFGrid->Cells[k][j++] = "Use GripperSensor";

    CheckONOFFGrid->Cells[k][j++] = "Use HoistAlignSensor";
    CheckONOFFGrid->Cells[k][j++] = "Use Oscillation Sen";
}

// ---------------------------------------------------------------------------
void TM2AForm::IO_Display()
{
    int j, k;
    k = 2;
    j = 0;

    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_BCR == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_PBSSensor == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_SoundAlarm == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_LampIndi == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_ShutterControl == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_EQPIO == true) ? "ON" : "OFF";

	CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_STBSensor == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandLimitSensorCheck == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandControl == true) ? "ON" : "OFF";
	CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_SteeringControl == true) ? "ON" : "OFF";

    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_LookDownSensor == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandPushSensorCheck == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_GripperSen == true) ? "ON" : "OFF";

    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HoistAlignSen == true) ? "ON" : "OFF";
    CheckONOFFGrid->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_OscillationSen == true) ? "ON" : "OFF";
}
// ---------------------------------------------------------------------------

void __fastcall TM2AForm::FormShow(TObject *Sender)
{
    IO_Display_Init();
    IO_Display();
}
// ---------------------------------------------------------------------------

void __fastcall TM2AForm::CheckONOFFGridDrawCell(TObject *Sender, int ACol,
    int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        CheckONOFFGrid->Canvas->Brush->Color = clBtnFace;
        CheckONOFFGrid->Canvas->Font->Color  = clWindowText;
        CheckONOFFGrid->Canvas->FillRect(Rect);
        Frame3D(CheckONOFFGrid->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 2)
    {
        if(CheckONOFFGrid->Cells[ACol][ARow] == "OFF")
        {
            CheckONOFFGrid->Canvas->Brush->Color = (TColor)0x00C080FF;
            CheckONOFFGrid->Canvas->FillRect(Rect);
        }
        else
        {
            CheckONOFFGrid->Canvas->Brush->Color = COLOR_WHITE;
            CheckONOFFGrid->Canvas->FillRect(Rect);
        }
    }
    else
    {
        CheckONOFFGrid->Canvas->Brush->Color = COLOR_WHITE;
        CheckONOFFGrid->Canvas->FillRect(Rect);

    }
    strText = CheckONOFFGrid->Cells[ACol][ARow];
    CheckONOFFGrid->Canvas->Font->Color = COLOR_BLACK;
    CheckONOFFGrid->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);

}
// ---------------------------------------------------------------------------

void __fastcall TM2AForm::btnAutoOKClick(TObject *Sender)
{
    String strTmpMessage;
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	/* shkim. 2016-03-14 skip modify
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) && (gm_OHTStatus->IOInput->uInput.InputV70.RailSupport_Home == OFF ))
	{
		// by zzang9un 2012.12.20 : Rail Support가 Home이 아닌 경우는 Auto 안되도록
		MainForm->Warnning_Display(COLOR_RED, "ERROR : Rail Support Not Home...");
	}
	else */ if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT)
	{
		// by zzang9un 2013.08.18 : Init 상태가 아닌 경우 Auto 안되도록
        MainForm->Warnning_Display(COLOR_RED, "ERROR : Status is Not Init...");
	}
    else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Data == 0)
    {
        // by zzang9un 2013.08.26 : Mark가 되지 않은 경우
        MainForm->Warnning_Display(COLOR_RED, "ERROR : OHT is Not Mark...");
    }
    else
    {
        if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL)
			// m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_AUTOMODE);
			m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_AUTOMODE);
        else
            MainForm->Warnning_Display(COLOR_RED, "OHT Current Mode = Auto");
    }

    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

void __fastcall TM2AForm::BitBtn2Click(TObject *Sender)
{
    Close();
}
// ---------------------------------------------------------------------------
