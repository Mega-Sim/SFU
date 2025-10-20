//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DisUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDisForm *DisForm;
//---------------------------------------------------------------------------
__fastcall TDisForm::TDisForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDisForm::FormClose(TObject *Sender, TCloseAction &Action)
{
   Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TDisForm::Save1Click(TObject *Sender)
{
   String strFileName;
   if(SaveDialog1->Execute())
   {
      strFileName=SaveDialog1->FileName;
      Memo1->Lines->SaveToFile(strFileName);
   }
}
//---------------------------------------------------------------------------
