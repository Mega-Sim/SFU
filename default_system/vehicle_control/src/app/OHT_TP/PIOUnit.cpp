//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PIOUnit.h"
#include "OHTInfoUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPIOForm *PIOForm;
//---------------------------------------------------------------------------
__fastcall TPIOForm::TPIOForm(TComponent* Owner)
   : TForm(Owner)
{
    timerUpdateDisplay->Enabled = false;
    timerUpdateDisplay->Interval = 100;
}
//---------------------------------------------------------------------------
void TPIOForm::PIODisplay(void)
{
   String strInData, strOutData, strHandPIO, strEQPIO;

   if(PIOForm->Visible==false) return;


   // Output

		shapLREQ->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ==1) ? COLOR_BLUE : COLOR_WHITE;
		shapUREQ->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ==1) ? COLOR_BLUE : COLOR_WHITE;
		shapREADY->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready==1) ? COLOR_BLUE : COLOR_WHITE;
		shapHOAVBL->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL==1) ? COLOR_BLUE : COLOR_WHITE;
		shapES->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES==1) ? COLOR_BLUE : COLOR_WHITE;

		// 추가 puting 2012.11.27
		shpGoLeft->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go==1) ? COLOR_BLUE : COLOR_WHITE;


	   shapVALID->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid==1) ? COLOR_RED : COLOR_WHITE;
	   shapCS0->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0==1) ? COLOR_RED : COLOR_WHITE;
	   shapTRREQ->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req==1) ? COLOR_RED : COLOR_WHITE;
	   shapBUSY->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy==1) ? COLOR_RED : COLOR_WHITE;
	   shapCOMPT->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete==1) ? COLOR_RED : COLOR_WHITE;

	   shpEQPIO_Left_Sel->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select==1) ? COLOR_BLUE : COLOR_WHITE;
//	   shpEQPIO_Right_Sel->Brush->Color = (gm_OHTStatus->IOOutput->uOutput.Output_EUV.EQPIO_Right_Select==1) ? COLOR_BLUE : COLOR_WHITE;

	   if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select==1)
		{
			btnPIOEnable->Caption="PIO Enable";
		}
		else
		{
			btnPIOEnable->Caption="PIO Disable";

		}


    // Go Signal
    // by zzang9un 2012.11.26 : Left, Right 처리가 필요
    //shapGo->Brush->Color = (gm_OHTStatus->IOInput->nEQPIO_GO_Signal==1) ? COLOR_RED : COLOR_WHITE;

   strHandPIO.sprintf(L"%d%d%d%d%d%d",
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition,
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm,
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip,
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip,
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Close,
					   gm_OHTStatus->IOInput->uInputCommon.HAND_Open
                     );
   HandPIOInPanel->Caption = strHandPIO;

   if(gm_OHTStatus->nIsRFPIOType==PIOTYPE_E84)
   {
      strEQPIO.sprintf(L"%d%d%d%d%d",
					   gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ,
					   gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ,
					   gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready,
					   gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL,
                       gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES
                     );
   }
   EqPIOInPanel->Caption   = strEQPIO;



   if(CheckBox1->Checked==false)
   {
      //Memo
      if(Memo1->Lines->Count>300) Memo1->Clear();
      if(Memo2->Lines->Count>300) Memo2->Clear();
      strInData.sprintf(L" %x%x[%06X] G[%d]:L[%d]U[%d]R[%d]H[%d]E[%d]",
					  gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode,
					  gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status,
                      gm_OHTStatus->m_OHTStatus.Data.CurNodeID,
					  //gm_OHTStatus->IOInput->uInputCommon.EQPIO_Right_Go,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES
                    );
      strOutData.sprintf(L" V[%d]C[%d]T[%d]B[%d]C[%d]",
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete
					 );
	  Memo1->Lines->Add(strInData);
	  Memo2->Lines->Add(strOutData);
   }
}
//---------------------------------------------------------------------------
void __fastcall TPIOForm::PIOOutput(TObject *Sender)
{
   if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL) return;


		if(Sender == Label11)
	   {
		  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

		  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid == OFF)       //puting 2012.11.29
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_VALID_ON);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_VALID_ON);
		  }
		  else
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_VALID_OFF);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_VALID_OFF);
		  }
		  delete m_CMDControl;
	   }
	   else if(Sender == Label12)
	   {
		  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

		  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0 == OFF)
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_0_ON);      //puting 2012.12.04
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_0_ON);
		  }
		  else
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_0_OFF);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_0_OFF);

		  }
		  delete m_CMDControl;
	   }
	   else if(Sender == Label15)
	   {
		  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

		  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req == OFF)
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_TR_REQ_ON);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_TR_REQ_ON);
		  }
		  else
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_TR_REQ_OFF);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_TR_REQ_OFF);
		  }
		  delete m_CMDControl;

	   }
	   else if(Sender == Label16)
	   {
		  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

		  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy == OFF)
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_BUSY_ON);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_BUSY_ON);
		  }
		  else
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_BUSY_OFF);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_BUSY_OFF);
		  }
		  delete m_CMDControl;

	   }
	   else if(Sender == Label17)
	   {
		  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

		  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete == OFF)
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_COMPT_ON);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_COMPT_ON);
		  }
		  else
		  {
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_COMPT_OFF);
			 m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_COMPT_OFF);
		  }
		  delete m_CMDControl;
	   }

}
//---------------------------------------------------------------------------

void __fastcall TPIOForm::OutReset1Click(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_ALL_OFF);   //2012.11.29 puting
   m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_ALL_OFF);   //2012.11.29 puting
   delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TPIOForm::InputClear1Click(TObject *Sender)
{
   Memo1->Clear();
   Memo2->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TPIOForm::btnPIOEnableClick(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

	if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select==1)
	 {
		m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_SELECT_ON);     //2012.11.29 puting
		m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_SELECT_ON);   //2012.11.29 puting
	 }else
	 {
		m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_SELECT_OFF);     //2012.11.29 puting
		m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_SELECT_OFF);   //2012.11.29 puting
	 }

   delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TPIOForm::Button2Click(TObject *Sender)
{
	uTP_CMD TPCmd; // TP Command
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

	TPCmd.TPCmd.Data1 = StrToInt(edtID->Text);
	TPCmd.TPCmd.Data2 = StrToInt(edtCH->Text);

	TPCmd.TPCmd.Data3 = cbbPIOType->ItemIndex;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_SET_RFPIO_CHANNEL, &TPCmd);

	delete m_CMDControl;
}
//---------------------------------------------------------------------------



void __fastcall TPIOForm::FormShow(TObject *Sender)
{
    timerUpdateDisplay->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TPIOForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    timerUpdateDisplay->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TPIOForm::timerUpdateDisplayTimer(TObject *Sender)
{
    PIODisplay();
}
//---------------------------------------------------------------------------



