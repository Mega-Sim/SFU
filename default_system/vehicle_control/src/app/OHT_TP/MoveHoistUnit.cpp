//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MoveHoistUnit.h"
#include "CMDControlUnit.h"
#include "MainUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMHoistForm *MHoistForm;
//---------------------------------------------------------------------------
__fastcall TMHoistForm::TMHoistForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------

/**
@brief   Hoist를 특정 거리, 특정 가속 시간, 속도로 이동시키는 함수
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TMHoistForm::btnHoistMoveDSTClick(TObject *Sender)
{
    int nTmpDist = 0, nAccelTime = 0, nSpeed = 0;

    uTP_CMD SndTPCmd;     // TP Command
    memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

    nTmpDist = StrToIntDef(lblDist->Caption, 2147483647);
    nAccelTime = StrToIntDef(edtAccel->Text, 0);
    nSpeed = StrToIntDef(edtSpeed->Text, 0);

    // 거리, 속도, 가속 시간 체크
    if((nTmpDist != 2147483647) && (nAccelTime != 0) && (nSpeed != 0))
    {
        // by zzang9un 2013.02.19 : TP Jog(Driving) Command를 사용
        SndTPCmd.CmdTP_Jog_TR.AccelTime = nAccelTime;
        SndTPCmd.CmdTP_Jog_TR.DecelTime = nAccelTime;
        SndTPCmd.CmdTP_Jog_TR.Velocity = nSpeed;
        SndTPCmd.CmdTP_Jog_TR.Distance = nTmpDist;
    
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DIST, &SndTPCmd);
        MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DIST =" + String(nTmpDist));
        delete m_CMDControl;
    }
    
    Close(); 
}
//---------------------------------------------------------------------------
void __fastcall TMHoistForm::btnCancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
