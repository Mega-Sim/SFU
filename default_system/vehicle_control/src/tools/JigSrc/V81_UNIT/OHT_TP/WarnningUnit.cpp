//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WarnningUnit.h"
#include "MainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWarnningForm *WarnningForm;
//---------------------------------------------------------------------------
__fastcall TWarnningForm::TWarnningForm(TComponent* Owner)
   : TForm(Owner)
{
   WarnningForm->Left = MainForm->Left+10;
   WarnningForm->Top  = MainForm->Top+300;
   WarnningForm->Width = MainForm->Width-120;

}
//---------------------------------------------------------------------------

void __fastcall TWarnningForm::FormShow(TObject *Sender)
{
   WarnningForm->Left = MainForm->Left+10;
   WarnningForm->Top  = MainForm->Top+200;
   WarnningForm->Width = MainForm->Width-120;

   ControlTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TWarnningForm::ControlTimerTimer(TObject *Sender)
{
   ControlTimer->Enabled = false;
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TWarnningForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   ControlTimer->Enabled = false;   
}
//---------------------------------------------------------------------------

