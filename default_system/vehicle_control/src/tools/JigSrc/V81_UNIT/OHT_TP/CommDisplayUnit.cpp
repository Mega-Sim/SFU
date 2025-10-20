//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CommDisplayUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCommDisplayForm *CommDisplayForm;
//---------------------------------------------------------------------------
__fastcall TCommDisplayForm::TCommDisplayForm(TComponent* Owner)
   : TForm(Owner)
{
   bDisplayFlag=true;
   bTrace=true;
}
//---------------------------------------------------------------------------
void __fastcall TCommDisplayForm::FormClose(TObject *Sender, TCloseAction &Action)
{
   bDisplayFlag=false;
}
//---------------------------------------------------------------------------
void TCommDisplayForm::Communicaion_Display(String strCommData)
{
   if(bDisplayFlag==true)
      DataAdd(strCommData);
}
//---------------------------------------------------------------------------
void __fastcall TCommDisplayForm::FormShow(TObject *Sender)
{
   bDisplayFlag=true;
}
//---------------------------------------------------------------------------
void TCommDisplayForm::DataAdd(String strTmpData)
{
   int i;
   mmoDisplay->Items->Add(FormatDateTime("hh:nn:ss: ",Now())+strTmpData);
   if(bTrace==true)
   {
      i=mmoDisplay->Items->Count;
      mmoDisplay->TopIndex = i-1;
   }
   if(mmoDisplay->Items->Count>1000) mmoDisplay->Items->Clear();

}
//---------------------------------------------------------------------------
void __fastcall TCommDisplayForm::mmoDisplayDblClick(TObject *Sender)
{
   bTrace = !bTrace;
}
//---------------------------------------------------------------------------

