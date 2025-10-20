//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "M1VelTimeUnit.h"
#include "VelGraphUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TM1VelForm *M1VelForm;
//---------------------------------------------------------------------------
__fastcall TM1VelForm::TM1VelForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TM1VelForm::BitBtn1Click(TObject *Sender)
{
   int ndd;

   ndd = StrToIntDef(Edit1->Text, 0);
   if(ndd!=0)
      VelGraphForm->Graph_Start1(ndd);

}
//---------------------------------------------------------------------------
