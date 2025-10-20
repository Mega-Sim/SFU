//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Trace.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWork *frmWork;
//---------------------------------------------------------------------------
__fastcall TfrmWork::TfrmWork(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmWork::btnClearClick(TObject *Sender)
{
	Memo1->Lines->Clear();
}
//---------------------------------------------------------------------------
void TfrmWork::DisplayLog(String strLog)
{
	Memo1->Lines->Add(strLog);
}
