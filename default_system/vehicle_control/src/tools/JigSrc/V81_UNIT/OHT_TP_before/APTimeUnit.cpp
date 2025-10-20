//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "APTimeUnit.h"
#include "APMonitoringUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAPTimeForm *APTimeForm;
//---------------------------------------------------------------------------
__fastcall TAPTimeForm::TAPTimeForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAPTimeForm::BitBtn1Click(TObject *Sender)
{
   int ndd;

   ndd = StrToIntDef(Edit1->Text, 0);
   if(ndd!=0)
      APMonitoringForm->Graph_Start1(ndd);
}
//---------------------------------------------------------------------------
