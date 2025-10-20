//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TimeSettingUnit.h"
#include "HIDGraphUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTimeSettingForm *TimeSettingForm;
//---------------------------------------------------------------------------
__fastcall TTimeSettingForm::TTimeSettingForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTimeSettingForm::BitBtn1Click(TObject *Sender)
{
   int ndd;

   ndd = StrToIntDef(Edit1->Text, 0);
   if(ndd!=0)
      HIDGraphForm->Graph_Start(ndd);
}
//---------------------------------------------------------------------------
