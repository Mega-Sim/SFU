//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "E23PIOUnit.h"
#include "OHTInfoUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TE23PIOForm *E23PIOForm;
//---------------------------------------------------------------------------
__fastcall TE23PIOForm::TE23PIOForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TE23PIOForm::PIODisplay(void)
{
   String strInData, strOutData;

   if(E23PIOForm->Visible==false) return;

   // Input
   shapLREQ->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ==1) ? COLOR_BLUE : COLOR_WHITE;
   shapUREQ->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ==1) ? COLOR_BLUE : COLOR_WHITE;
   shapREADY->Brush->Color = (gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready==1) ? COLOR_BLUE : COLOR_WHITE;

   // by zzang9un 2012.11.26 : 7.0에서는 Input이 없으므로 주석 처리
   //shapABORT->Brush->Color = (gm_OHTStatus->IOInput->nPIO_ABORT==1) ? COLOR_BLUE : COLOR_WHITE;

   // Output

   shapVALID->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid==1) ? COLOR_RED : COLOR_WHITE;
   shapCS0->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0==1) ? COLOR_RED : COLOR_WHITE;
   shapCS1->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1==1) ? COLOR_RED : COLOR_WHITE;
  // shapCS2->Brush->Color = (gm_OHTStatus->IOOutput->nPIO_CS_2==1) ? COLOR_RED : COLOR_WHITE;
  // shapCS3->Brush->Color = (gm_OHTStatus->IOOutput->nPIO_CS_3==1) ? COLOR_RED : COLOR_WHITE;
   shapTRREQ->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req==1) ? COLOR_RED : COLOR_WHITE;
   shapBUSY->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy==1) ? COLOR_RED : COLOR_WHITE;
   shapCOMPT->Brush->Color = (gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete==1) ? COLOR_RED : COLOR_WHITE;


   // Go Signal
   // by zzang9un 2012.11.26 : 7.0에서는 lft, right 처리가 필요함
   //shapGo->Brush->Color = (gm_OHTStatus->IOInput->nEQPIO_GO_Signal==1) ? COLOR_RED : COLOR_WHITE;

  if((gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select==1)) btnPIOEnable->Enabled=true;
   else btnPIOEnable->Enabled=false;

   if(CheckBox1->Checked==false)
   {
      //Memo
      if(Memo1->Lines->Count>300) Memo1->Clear();
      if(Memo2->Lines->Count>300) Memo2->Clear();
      strInData.sprintf(L" %d%s[N%06d] G[%d]:L[%d]U[%d]A[%d]R[%d]",
					  gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode,
					  gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status,
                      gm_OHTStatus->m_OHTStatus.Data.CurNodeID,
                      0, //gm_OHTStatus->IOInput->nEQPIO_GO_Signal,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ,
					  gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ,
                      0, //gm_OHTStatus->IOInput->nPIO_ABORT,
                      gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready
                    );
      strOutData.sprintf(L" V[%d]C[%d]1[%d]T[%d]B[%d]C[%d]",
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1,
                    //  gm_OHTStatus->IOOutput->nPIO_CS_2,
                    //  gm_OHTStatus->IOOutput->nPIO_CS_3,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy,
					  gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete
                     );
      Memo1->Lines->Add(strInData);
      Memo2->Lines->Add(strOutData);
   }
}
//---------------------------------------------------------------------------
void __fastcall TE23PIOForm::PIOOutput(TObject *Sender)
{
   if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL) return;

    //puting 2012.12.04
   if(Sender == Label11)
   {
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

	  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid == OFF)
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
         m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_0_ON);
         m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_0_ON);
      }
      else
      {
         m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_0_OFF);
         m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_0_OFF);
      }
      delete m_CMDControl;
   }
   else if(Sender == Label6)
   {
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;

	  if(gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1 == OFF)
      {
         m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_1_ON);
         m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_1_ON);
      }
      else
      {
         m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_CS_1_OFF);
         m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_CS_1_OFF);
      }
      delete m_CMDControl;
   }
//   else if(Sender == Label8)
//   {
//      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
////
////      if(gm_OHTStatus->IOOutput->nPIO_CS_2 == OFF)
////         m_CMDControl->MakeCommand2OHT(OUT_PIO_CS_2_ON);
////      else
////         m_CMDControl->MakeCommand2OHT(OUT_PIO_CS_2_OFF);
//
//      delete m_CMDControl;
//   }
//   else if(Sender == Label9)
//   {
//      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
//
////      if(gm_OHTStatus->IOOutput->nPIO_CS_3 == OFF)
////         m_CMDControl->MakeCommand2OHT(OUT_PIO_CS_3_ON);
////      else
////         m_CMDControl->MakeCommand2OHT(OUT_PIO_CS_3_OFF);
//
//      delete m_CMDControl;
//   }
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

void __fastcall TE23PIOForm::OutReset1Click(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   //puting 2012.12.04
   m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_ALL_OFF);
   m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_ALL_OFF);
   delete m_CMDControl;
}
//---------------------------------------------------------------------------
void __fastcall TE23PIOForm::InputClear1Click(TObject *Sender)
{
   Memo1->Clear();
   Memo2->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TE23PIOForm::btnPIOEnableClick(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   m_CMDControl->MakeTPCmd2OHT(TPCMD_LEFT_PIO_SELECT_ON);
   m_CMDControl->MakeTPCmd2OHT(TPCMD_RIGHT_PIO_SELECT_ON);
   delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TE23PIOForm::Button2Click(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   m_CMDControl->MakeTPCmd2OHT(TPCMD_SET_RFPIO_CHANNEL);
   delete m_CMDControl;
}
//---------------------------------------------------------------------------

