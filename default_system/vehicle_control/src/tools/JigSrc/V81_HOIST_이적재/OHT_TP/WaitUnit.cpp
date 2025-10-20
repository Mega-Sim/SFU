//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WaitUnit.h"
#include "MainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWaitForm *WaitForm;
//---------------------------------------------------------------------------
__fastcall TWaitForm::TWaitForm(TComponent* Owner)
   : TForm(Owner)
{
   WaitForm->Left = MainForm->Left+10;
   WaitForm->Top  = MainForm->Top+300;
   WaitForm->Width = MainForm->Width-120;
}
//---------------------------------------------------------------------------
void __fastcall TWaitForm::FormShow(TObject *Sender)
{
   WaitForm->Left = MainForm->Left+10;
   WaitForm->Top  = MainForm->Top+200;
   WaitForm->Width = MainForm->Width-120;
}
//---------------------------------------------------------------------------
