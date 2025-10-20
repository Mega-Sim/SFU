//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SpeedHelpUnit.h"
#include "Define.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSpeedHelpForm *SpeedHelpForm;
//---------------------------------------------------------------------------
__fastcall TSpeedHelpForm::TSpeedHelpForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSpeedHelpForm::edtMPerSecKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
   float fMPerSec, fMPerMin, fCPerMSec, fCPerSec;
   String strMPerSec, strMPerMin, strCPerMSec, strCPerSec;


   if(Key == VK_RETURN)
   {
      if(Sender == edtMPerSec)
      {
         if(edtMPerSec->Text != "")
         {
            fMPerSec = StrToFloat(edtMPerSec->Text);
            fMPerMin = fMPerSec * 60.0;
            fCPerMSec = (fMPerSec*819200.0)/(1000.0*DIST_1ROTATE_CM);
            fCPerSec = fCPerMSec * 1000.0;
            strMPerMin.sprintf(L"%.02f", fMPerMin);
            strCPerMSec.sprintf(L"%.02f", fCPerMSec);
            strCPerSec.sprintf(L"%.02f", fCPerSec);

            edtMPerMin->Text = strMPerMin;
            edtCPerMSec->Text = strCPerMSec;
            edtCPerSec->Text = strCPerSec;
         }
      }
      else if(Sender == edtMPerMin)
      {
         if(edtMPerMin->Text != "")
         {
            fMPerMin = StrToFloat(edtMPerMin->Text);
            fMPerSec = fMPerMin / 60.0;
            fCPerMSec = (fMPerSec*819200.0)/(1000.0*DIST_1ROTATE_CM);
            fCPerSec = fCPerMSec * 1000.0;
            strMPerSec.sprintf(L"%.02f", fMPerSec);
            strCPerMSec.sprintf(L"%.02f", fCPerMSec);
            strCPerSec.sprintf(L"%.02f", fCPerSec);

            edtMPerSec->Text = strMPerSec;
            edtCPerMSec->Text = strCPerMSec;
            edtCPerSec->Text = strCPerSec;
         }

      }
      else if(Sender == edtCPerMSec)
      {
         if(edtCPerMSec->Text != "")
         {
            fCPerMSec = StrToFloat(edtCPerMSec->Text);
            fCPerSec = fCPerMSec * 1000.0;
            fMPerSec = (fCPerMSec*1000.0*DIST_1ROTATE_CM)/(819200.0);
            fMPerMin = fMPerSec * 60.0;

            strMPerSec.sprintf(L"%.02f", fMPerSec);
            strMPerMin.sprintf(L"%.02f", fMPerMin);
            strCPerSec.sprintf(L"%.02f", fCPerSec);

            edtMPerSec->Text = strMPerSec;
            edtMPerMin->Text = strMPerMin;
            edtCPerSec->Text = strCPerSec;
         }

      }
      else if(Sender == edtCPerSec)
      {
         if(edtCPerSec->Text != "")
         {
            fCPerSec = StrToFloat(edtCPerSec->Text);
            fCPerMSec = fCPerSec / 1000.0;
            fMPerSec = (fCPerMSec*1000.0*DIST_1ROTATE_CM)/(819200.0);
            fMPerMin = fMPerSec * 60.0;

            strMPerSec.sprintf(L"%.02f", fMPerSec);
            strMPerMin.sprintf(L"%.02f", fMPerMin);
            strCPerMSec.sprintf(L"%.02f", fCPerMSec);

            edtMPerSec->Text = strMPerSec;
            edtMPerMin->Text = strMPerMin;
            edtCPerMSec->Text = strCPerMSec;
         }

      }
   }
}
//---------------------------------------------------------------------------

