//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ChangeIOFileUnit.h"
#include "FileUnit.h"
#include "Define.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TChIOFileForm *ChIOFileForm;
//---------------------------------------------------------------------------
__fastcall TChIOFileForm::TChIOFileForm(TComponent* Owner)
   : TForm(Owner)
{
   Label1->Caption = "Can Multi Select";
}
//---------------------------------------------------------------------------
void __fastcall TChIOFileForm::btnOpenClick(TObject *Sender)
{
   String strFileName, strTmpFilter, strSaveFile;
   String strOHTVer;
   int i;

   strTmpFilter = "Log Data(*.log)|+;*.log";
   OpenDialog1->Filter = strTmpFilter;
   OpenDialog1->FileName="Select IO Log Data";

   if(OpenDialog1->Execute())
   {
      for(i=0; i< OpenDialog1->Files->Count; i++)
      {
         strFileName = OpenDialog1->Files->Strings[i]; //Multi Select...set the ofAllowMultiSelect flag in Options
         FileForm->ChangeIOFile("5.0 New[AMC]", strFileName);
      }
   }
}
//---------------------------------------------------------------------------
