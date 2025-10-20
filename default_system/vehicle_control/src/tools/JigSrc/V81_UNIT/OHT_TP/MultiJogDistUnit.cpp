//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MultiJogDistUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "CmdWinUnit.h"
#include "ChkExecUnit.h"
#include "CMDControlUnit.h"
#include "MainUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMultiJogDistForm *MultiJogDistForm;
//---------------------------------------------------------------------------
__fastcall TMultiJogDistForm::TMultiJogDistForm(TComponent* Owner)
   : TForm(Owner)
{
   gm_nStep=0;
}
//---------------------------------------------------------------------------
void __fastcall TMultiJogDistForm::Timer1Timer(TObject *Sender)
{
   Label1->Caption = String().sprintf(L"Cmd Number = %d", gm_nStep);
}
//---------------------------------------------------------------------------
void __fastcall TMultiJogDistForm::Button9Click(TObject *Sender)
{
   gm_nStep=0;
}
//---------------------------------------------------------------------------
int TMultiJogDistForm::MoveDist(int nStep)
{
   int nError=0;
   String strTmpData;
   int dAcceleration_mpersec2=0.0,dDeceleration_mpersec2=0.0, dVel_mpersec=0.0, dDist_mm=0.0;

   //puting 2012.12.03 공통cmd 구조체 초기화
   uTP_CMD SndTPCmd;     // TP Command
   memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

   switch(nStep)
   {
	  case 0 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec1_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec1_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel1->Text)*1000;
//         dDist_mm = StrToInt(edtDist1->Text);
//         gm_nStep=1;
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec1_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec1_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel1->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist1->Text);
         gm_nStep=1;
         break;
      case 1 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec2_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec2_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel2->Text)*1000;
//         dDist_mm = StrToInt(edtDist2->Text);
//         gm_nStep=2;
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec2_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec2_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel2->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist2->Text);
         gm_nStep=2;
         break;
      case 2 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec3_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec3_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel3->Text)*1000;
//         dDist_mm = StrToInt(edtDist3->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec3_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec3_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel3->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist3->Text);
         gm_nStep=3;
         break;
      case 3 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec4_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec4_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel4->Text)*1000;
//         dDist_mm = StrToInt(edtDist4->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec4_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec4_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel4->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist4->Text);
         gm_nStep=4;
         break;
      case 4 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec5_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec5_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel5->Text)*1000;
//         dDist_mm = StrToInt(edtDist5->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec5_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec5_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel5->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist5->Text);
         gm_nStep=5;
         break;
      case 5 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec6_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec6_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel6->Text)*1000;
//         dDist_mm = StrToInt(edtDist6->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec6_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec6_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel6->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist6->Text);
         gm_nStep=6;
         break;
      case 6 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec7_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec7_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel7->Text)*1000;
//         dDist_mm = StrToInt(edtDist7->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec7_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec7_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel7->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist7->Text);
         gm_nStep=7;
         break;
      case 7 :
//         dAcceleration_mpersec2 = StrToFloat(edtmpersec8_ACC->Text)*1000;
//         dDeceleration_mpersec2 = StrToFloat(edtmpersec8_DEC->Text)*1000;
//         dVel_mpersec = StrToFloat(edtVel8->Text)*1000;
//         dDist_mm = StrToInt(edtDist8->Text);
		 SndTPCmd.CmdTP_Jog_DIST.Accel = StrToFloat(edtmpersec8_ACC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Decel = StrToFloat(edtmpersec8_DEC->Text)*1000;
		 SndTPCmd.CmdTP_Jog_DIST.Velocity = StrToFloat(edtVel8->Text)*1000;
         SndTPCmd.CmdTP_Jog_DIST.Distance = StrToInt(edtDist8->Text);
         gm_nStep=8;
         break;
   }

   if((dAcceleration_mpersec2!=0.0)&&(dDeceleration_mpersec2!=0.0)&&(dVel_mpersec!=0.0)&&(dDist_mm!=0.0))
   {
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   //   strTmpData.sprintf(L"%d/%d/%d/%d",dAcceleration_mpersec2,dDeceleration_mpersec2,dVel_mpersec, dDist_mm);

      MainForm->DataDisplay(COLOR_BLACK,"Driving Forward");
	  m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_DRIVING_DIST, &SndTPCmd);
      delete m_CMDControl;
   }
   return nError;
}
//---------------------------------------------------------------------------
void __fastcall TMultiJogDistForm::Button10Click(TObject *Sender)
{
   MoveDist(gm_nStep);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button8Click(TObject *Sender)
{
   MoveDist(0);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button7Click(TObject *Sender)
{
   MoveDist(1);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button6Click(TObject *Sender)
{
   MoveDist(2);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button5Click(TObject *Sender)
{
   MoveDist(3);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button4Click(TObject *Sender)
{
   MoveDist(4);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button3Click(TObject *Sender)
{
   MoveDist(5);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button2Click(TObject *Sender)
{
   MoveDist(6);
}
//---------------------------------------------------------------------------

void __fastcall TMultiJogDistForm::Button1Click(TObject *Sender)
{
   MoveDist(7);
}
//---------------------------------------------------------------------------

