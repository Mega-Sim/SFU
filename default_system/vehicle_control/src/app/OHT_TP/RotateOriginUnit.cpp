// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RotateOriginUnit.h"
#include "OHTInfoUnit.h"
#include "ChkExecUnit.h"
#include "Define.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "MainUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRotateOrgForm *RotateOrgForm;
// ---------------------------------------------------------------------------
TCHKOHT CmdohtCheck;

// ---------------------------------------------------------------------------
__fastcall TRotateOrgForm::TRotateOrgForm(TComponent* Owner) : TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TRotateOrgForm::FormShow(TObject *Sender)
{
    Label1->Caption = "The Mechanical Origin Offset";
    Label2->Caption = "==> Only Integer Values";

    Timer1->Enabled = true;
}
// ---------------------------------------------------------------------------

void __fastcall TRotateOrgForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    Timer1->Enabled = false;
}
// ---------------------------------------------------------------------------

void __fastcall TRotateOrgForm::Timer1Timer(TObject *Sender)
{
    String strData;

    strData.sprintf(L"%.02f", gm_OHTStatus->Motor_Rotate->dPosition_cts);
    lblMotor4_pos->Caption = strData;

    shp4Run->Brush->Color = (gm_OHTStatus->Motor_Rotate->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
    panAx4_ServoON->Color = (gm_OHTStatus->Motor_Rotate->bIs_ServoON == true) ? COLOR_LIME : COLOR_FUCHSIA;
}
// ---------------------------------------------------------------------------

void __fastcall TRotateOrgForm::btnCloseClick(TObject *Sender)
{
    Close();
}
// ---------------------------------------------------------------------------

void __fastcall TRotateOrgForm::btnOKClick(TObject *Sender)
{
    String strData;
    int nOffset = 0;

    // puting 2012.12.03 공통cmd 구조체 초기화
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    strData = Edit1->Text;
    nOffset = StrToIntDef(strData, 4000000);

    if(nOffset == 4000000)
        Edit1->Text = "Invalid Data";
    else if((nOffset > 3276700) || (nOffset < -3276700))
        Edit1->Text = "Invalid Data";
    else
    {
        if(CmdohtCheck.Is_Executable() == NO_ERR)
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

            TPCmd.TPCmd.Data1 = (int) nOffset / 100;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_ORIGIN_SETTING, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "CMD Shift Origin Set");
            delete m_CMDControl;
            Edit1->Text = "";
            Close();
        }
    }
}
// ---------------------------------------------------------------------------
