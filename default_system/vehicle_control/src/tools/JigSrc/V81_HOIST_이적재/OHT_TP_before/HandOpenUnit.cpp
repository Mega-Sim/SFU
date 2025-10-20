//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HandOpenUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THandOpenForm *HandOpenForm;
//---------------------------------------------------------------------------
__fastcall THandOpenForm::THandOpenForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall THandOpenForm::FormShow(TObject *Sender)
{
   CheckBox1->Checked=false;
   CheckBox2->Checked=false;
   CheckBox3->Checked=false;
   CheckBox4->Checked=false;
   btnOK->Enabled=false;
   #if(LANGUAGE_MODE==MODE_KOREAN)
      HandOpenForm->Caption = "Hand Open 확인";
	  GroupBox1->Caption = " 체크 항목 ";
      CheckBox1->Caption = "반송체 존재, 이상 여부 육안 확인";
      CheckBox2->Caption = "반송체 존재시 티칭위치까지 이동여부";
      CheckBox3->Caption = "반송체 존재시 Hand Gripper 센서 감지 여부(2EA=ON)";
      CheckBox4->Caption = "반송체 존재시 Shutter Open 여부";

   #else
      HandOpenForm->Caption = "Checklist for Open Hand";
      GroupBox1->Caption = " CheckList ";
      CheckBox1->Caption = "FOSB Present, Please do a visual check a Problem.";
      CheckBox2->Caption = "If FOSB Present, Check that Hand position is a teaching position.";
	  CheckBox3->Caption = "If FOSB Present, Check that the Hand Gripper Sensor is On.(2EA=ON).";
      CheckBox4->Caption = "If FOSB Present, Check that the Shutter is Open State.";
   #endif
}
//---------------------------------------------------------------------------
void __fastcall THandOpenForm::CheckBox1Click(TObject *Sender)
{
   Check_OKBtn();
}
//---------------------------------------------------------------------------
int THandOpenForm::Check_OKBtn(void)
{
   int nError=0;

   if((CheckBox1->Checked==true) &&
      (CheckBox2->Checked==true) &&
      (CheckBox3->Checked==true) &&
      (CheckBox4->Checked==true))
   {
      btnOK->Enabled=true;
   }
   else
   {
	  btnOK->Enabled=false;
   }

   return nError;
}
void __fastcall THandOpenForm::btnCancelClick(TObject *Sender)
{
   Close();   
}
//---------------------------------------------------------------------------
void __fastcall THandOpenForm::btnOKClick(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

   bool bHandRGrip = false;
   bool bHandLGrip = false;


   bHandRGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
   bHandLGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;



   if((bHandRGrip == OFF) && (bHandLGrip == OFF))
   {
      if(gm_OHTStatus->nProgram_Mode==MODE_OPERATOR)//근무자
      {
         if(Check_Is_HandOpen()==NO_ERR)
         {
	         m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_OPEN_LIMIT);
            MainForm->DataDisplay(COLOR_BLACK,"Hand Open");
         }
      }
   }
   else if( MessageDlg("FOUP is detected. Really open?", mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0) == IDOK)
   {
      if(gm_OHTStatus->nProgram_Mode==MODE_OPERATOR)//근무자
      {
         if(Check_Is_HandOpen()==NO_ERR)
         {
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HAND_OPEN_LIMIT);
            MainForm->DataDisplay(COLOR_BLACK,"Hand Open");
         }
      }
   }
   delete m_CMDControl;

   Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int THandOpenForm::Check_Is_HandOpen(void)
{
   int nError=0;
   bool bHandClose = false;
   bool bHandOpen = false;
   bool bHandAlarm = false;


   bHandClose = gm_OHTStatus->IOInput->uInputCommon.HAND_Close;
   bHandOpen = gm_OHTStatus->IOInput->uInputCommon.HAND_Open;
   bHandAlarm = gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm;


   if(bHandAlarm==ON)
   {
		nError=1;
		if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
		{
			FoupOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] Hand Alarm");
		}
		else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
		{
			EUVOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] Hand Alarm");
		}

   }
   else if((bHandClose==OFF)&&(bHandOpen==OFF))
   {
	  nError=1;
		if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
		{
			FoupOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] Hand Close/Open Limit Sensor==OFF");
		}
		else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
		{
			EUVOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] Hand Close/Open Limit Sensor==OFF");
		}


   }
   else if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID==0)
   {
	  nError=1;
		if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
		{
			FoupOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] OHT CurrentNode==0");
		}
		else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
		{
			EUVOnlineCMDForm->Warnning_Display(COLOR_RED, "[ERROR] OHT CurrentNode==0");
		}

   }
   return nError;
   
}
