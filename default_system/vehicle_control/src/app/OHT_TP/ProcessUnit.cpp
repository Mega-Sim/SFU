//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ProcessUnit.h"
#include "FileUnit.h"
#include "LogFileUnit.h"
#include "OHTInfoUnit.h"
#include "Define.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma link "cgauges"
#pragma link "cgauges"
#pragma resource "*.dfm"
TProcessForm *ProcessForm;

//---------------------------------------------------------------------------
__fastcall TProcessForm::TProcessForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::FormShow(TObject *Sender)
{
   if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
   {
      ProcessForm->Left = LogFileForm->Left;
      ProcessForm->Top = LogFileForm->Top;
      ProcessForm->Width=LogFileForm->Width;
   }
   else
   {
      ProcessForm->Left = FileForm->Left;
      ProcessForm->Top = FileForm->Top;
      ProcessForm->Width=FileForm->Width;
   }
}
//---------------------------------------------------------------------------

