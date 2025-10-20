//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "M24VelTimeUnit.h"
#include "VelGraphUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TM24VelForm *M24VelForm;
//---------------------------------------------------------------------------
__fastcall TM24VelForm::TM24VelForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TM24VelForm::BitBtn1Click(TObject *Sender)
{
   int ndd;

   ndd = StrToIntDef(Edit1->Text, 0);
   if(ndd!=0)
      VelGraphForm->Graph_Start24(ndd);

}
//---------------------------------------------------------------------------
