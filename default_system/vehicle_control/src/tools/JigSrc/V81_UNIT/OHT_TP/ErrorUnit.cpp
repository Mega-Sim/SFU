//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ErrorUnit.h"
#include "MainUnit.h"
#include "StatusUnit.h"
#include "OHTInfoUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm *ErrorForm;
//---------------------------------------------------------------------------
__fastcall TErrorForm::TErrorForm(TComponent* Owner)
   : TForm(Owner)
{
   g_nIsShow=0;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm::FormShow(TObject *Sender)
{
//   ErrorForm->Top = MainForm->Top + MainForm->Height;

    ErrorForm->Top = StatusForm->Top + StatusForm->Height;
    ErrorForm->Left = MainForm->Left;
    ErrorForm->Width = MainForm->Width + StatusForm->Width;

    ErrorForm->Caption = "Error Processing Standards";

    g_nIsShow=1;
    Get_ErrorHelp();
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   g_nIsShow=0;   
}
//---------------------------------------------------------------------------
int TErrorForm::Get_ErrorHelp(void)
{
   int nError=0;

   if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0)
   {
      memErrHelp->Clear();
   }
   else
   {
      memErrHelp->Clear();

      TStringList *tErrData = new TStringList;
      String strTPPath=ExtractFileDir(Application->ExeName);
      String strErrFile="";
      int i;
      String strFindErr="";
      String strLineData="";
      strFindErr.sprintf(L"[E%04d]",gm_OHTStatus->m_OHTStatus.Data.ErrorCode);

      strErrFile.sprintf(L"%s\\%s",strTPPath,"HelpError.txt");

      tErrData->LoadFromFile(strErrFile);

      bool bFind=false;
      for(i=0; i<tErrData->Count; i++)
      {
         strLineData = tErrData->Strings[i];
         if(bFind==false)
         {
            if(strLineData.Pos(strFindErr)!=0) bFind=true;
         }
         else
         {
            if(strLineData.Pos("//[E")!=0) break;
            else
               memErrHelp->Lines->Add(strLineData);
         }
      }
      delete tErrData;
   }

   return nError;
}
//---------------------------------------------------------------------------
