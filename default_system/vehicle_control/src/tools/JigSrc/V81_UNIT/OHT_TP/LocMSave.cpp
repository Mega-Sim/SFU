//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "LocMSave.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "MainUnit.h"
#include "Define.h"
#include "Def_TP.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMoveForm *MoveForm;
//---------------------------------------------------------------------------
__fastcall TMoveForm::TMoveForm(TComponent* Owner)
   : TForm(Owner)
{
   nIsWhatMove=MOVE_INIT;
   nIsAxis=0;
}
//---------------------------------------------------------------------------
void __fastcall TMoveForm::FormShow(TObject *Sender)
{
   Edit1->SetFocus();   
}
//---------------------------------------------------------------------------
void __fastcall TMoveForm::BitBtn1Click(TObject *Sender)
{
   TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
   int nMoveData;
   int nError=0;
   String strTmpData;

   nMoveData = StrToIntDef(Edit1->Text,0);
   if((nMoveData==0)||abs(nMoveData)>50000) nError=1;
   if(nError==0)
   {
      strTmpData.sprintf(L"%07d",nMoveData);
      if(nIsWhatMove==MOVE_STB)
      {
         switch(nIsAxis)
         {
            case 2 :
            //   m_CMDControl->MakeCommand2OHT(LOC2_STB_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[STB] #2 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 3 :
           //    m_CMDControl->MakeCommand2OHT(LOC3_STB_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[STB] #3 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 4 :
            //   m_CMDControl->MakeCommand2OHT(LOC4_STB_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[STB] #4 LOC = LOC + Move["+strTmpData+"]");
               break;
            default :
               MainForm->DataDisplay(COLOR_RED,"LOC Save Error........");
               break;
         }
      }
      else if(nIsWhatMove==MOVE_EQ)
      {
         switch(nIsAxis)
         {
            case 2 :
            //   m_CMDControl->MakeCommand2OHT(LOC2_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #2 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 3 :
             //  m_CMDControl->MakeCommand2OHT(LOC3_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #3 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 4 :
             //  m_CMDControl->MakeCommand2OHT(LOC4_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #4 LOC = LOC + Move["+strTmpData+"]");
               break;
            default :
               MainForm->DataDisplay(COLOR_RED,"LOC Save Error........");
               break;
         }
      }
      else if(nIsWhatMove==MOVE_EQ_X)
      {
         switch(nIsAxis)
         {
            case 2 :
            //   m_CMDControl->MakeCommand2OHT(LOC2_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #2 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 3 :
             //  m_CMDControl->MakeCommand2OHT(LOC3_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #3 LOC = LOC + Move["+strTmpData+"]");
               break;
            case 4 :
            //   m_CMDControl->MakeCommand2OHT(LOC4_SAVE,strTmpData);
               MainForm->DataDisplay(COLOR_BLACK,"[EQ] #4 LOC = LOC + Move["+strTmpData+"]");
               break;
            default :
               MainForm->DataDisplay(COLOR_RED,"LOC Save Error........");
               break;
         }
      }
      else
      {
         MainForm->DataDisplay(COLOR_RED,"LOC Save Error........");
      }
   }
   else
   {
      MainForm->DataDisplay(COLOR_RED,"LOC Save Error........");
   }
   delete m_CMDControl;

}
//---------------------------------------------------------------------------
int TMoveForm::SetWhatMove(int nTmpMove, int nTmpAxis)
{
   int nError=0;

   Edit1->Text="0";
   nIsWhatMove = nTmpMove;
   switch(nIsWhatMove)
   {
      case MOVE_INIT :
         nError=1;
         break;
      case MOVE_STB :
         lblWho->Caption = "[STB]";
         break;
      case MOVE_EQ :
         lblWho->Caption = "[EQ]";
         break;
   }

   if(nError==0)
   {
      nIsAxis = nTmpAxis;
      switch(nIsAxis)
      {
         case 2 :
            lblAxis->Caption="#2 Move Loc Save";
            break;
         case 3 :
            lblAxis->Caption ="#3 Move Loc Save";
            break;
         case 4 :
            lblAxis->Caption ="#4 Move Loc Save";
            break;
         default :
            nError=1;
            break;
      }
   }
   return nError;
}
