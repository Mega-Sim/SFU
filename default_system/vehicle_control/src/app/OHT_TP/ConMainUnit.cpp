//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include "ConMainUnit.h"
#include "ConversionUnit.h"
#include "Define.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConversionForm *ConversionForm;
//---------------------------------------------------------------------------
__fastcall TConversionForm::TConversionForm(TComponent* Owner)
   : TForm(Owner)
{
    Edit2->Text = (String)COUNT_1ROTATE;
    Edit3->Text = (String)WD4_DIST_1ROTATE_MM;

    ConversionForm->Caption = "Unit Conversion";
    Label1->Caption = "The Enc value per motor 1 Rev = ";
    Label2->Caption = "The Dist per motor 1 Rev[mm] = ";
    Button1->Caption = "Conversion";
}

//---------------------------------------------------------------------------
void __fastcall TConversionForm::Button1Click(TObject *Sender)
{
   String strdd, strInUnit, strOutUnit;
   double dInput, dOutput;
   double dCountRotate;
   double dDistmm;

   dCountRotate=StrToFloat(Edit2->Text);
   dDistmm=StrToFloat(Edit3->Text);

   Memo1->Clear();
   Memo1->Lines->Add("==== result =====");
   dInput = StrToFloat(Edit1->Text);
   TCONVERSION *m_Conversion = new TCONVERSION;

   switch(RadioGroup1->ItemIndex+1)
   {
      case 1:
         strInUnit  = "cts/msec";
         strOutUnit = "cts/sec";
         dOutput = m_Conversion->CTSperMSEC_to_CTSperSEC(dInput);
         break;
      case 2:
         strInUnit  = "cts/sec";
         strOutUnit = "cts/msec";
         dOutput = m_Conversion->CTSperSEC_to_CTSperMSEC(dInput);
         break;
      case 3:
         strInUnit  = "cts";
         strOutUnit = "mm";
         dOutput = m_Conversion->CTS_to_MM(dInput,dCountRotate,dDistmm);
         break;
      case 4:
         strInUnit  = "mm";
         strOutUnit = "cts";
         dOutput = m_Conversion->MM_to_CTS(dInput,dCountRotate,dDistmm);
         break;
      case 5:
         strInUnit  = "cts/sec";
         strOutUnit = "rpm";
         dOutput = m_Conversion->CTSperSEC_to_RPM(dInput,dCountRotate);
         break;
      case 6:
         strInUnit  = "rpm";
         strOutUnit = "cts/sec";
         dOutput = m_Conversion->RPM_to_CTSperSEC(dInput,dCountRotate);
         break;
      case 7:
         strInUnit  = "cts/msec";
         strOutUnit = "rpm";
         dOutput = m_Conversion->CTSperMSEC_to_RPM(dInput,dCountRotate);
         break;
      case 8:
         strInUnit  = "rpm";
         strOutUnit = "cts/msec";
         dOutput = m_Conversion->RPM_to_CTSperMSEC(dInput,dCountRotate);
         break;
      case 9:
         strInUnit  = "m/sec";
         strOutUnit = "rpm";
         dOutput = m_Conversion->MperSEC_to_RPM(dInput, dDistmm);
         break;
      case 10:
         strInUnit  = "rpm";
         strOutUnit = "m/sec";
         dOutput = m_Conversion->RPM_to_MperSEC(dInput, dDistmm);
         break;
      case 11:
         strInUnit  = "cts/sec";
         strOutUnit = "m/sec";
         dOutput = m_Conversion->CTSperSEC_to_MperSEC(dInput,dCountRotate,dDistmm);
         break;
      case 12:
         strInUnit  = "m/sec";
         strOutUnit = "cts/sec";
         dOutput = m_Conversion->MperSEC_to_CTSperSEC(dInput,dCountRotate,dDistmm);
         break;
      case 13:
         strInUnit  = "cts/msec";
         strOutUnit = "m/sec";
         dOutput = m_Conversion->CTSperMSEC_to_MperSEC(dInput,dCountRotate,dDistmm);
         break;
      case 14:
         strInUnit  = "m/sec";
         strOutUnit = "cts/msec";
         dOutput = m_Conversion->MperSEC_to_CTSperMSEC(dInput,dCountRotate,dDistmm);
         break;
      case 15:
         strInUnit  = "cts/sec2";
         strOutUnit = "m/sec2";
         dOutput = m_Conversion->CTSperSEC2_to_MperSec2(dInput,dCountRotate,dDistmm);
         break;
      case 16:
         strInUnit  = "m/sec2";
         strOutUnit = "cts/sec2";
         dOutput = m_Conversion->MperSec2_to_CTSperSEC2(dInput,dCountRotate,dDistmm);
         break;
      case 17:
         strInUnit  = "cts/msec2";
         strOutUnit = "m/sec2";
         dOutput = m_Conversion->CTSperMSEC2_to_MperSec2(dInput,dCountRotate,dDistmm);
         break;
      case 18:
         strInUnit  = "m/sec2";
         strOutUnit = "cts/msec2";
         dOutput = m_Conversion->MperSec2_to_CTSperMSEC2(dInput,dCountRotate,dDistmm);
         break;
   }
   strdd.sprintf(L"Input  = %.02f[%s]", dInput, strInUnit);
   Memo1->Lines->Add(strdd);

   strdd.sprintf(L"Output = %.02f[%s]", dOutput, strOutUnit);
   Memo1->Lines->Add(strdd);

   Memo1->Lines->Add("================");

   delete m_Conversion;
}
//---------------------------------------------------------------------------
void __fastcall TConversionForm::RadioGroup2Click(TObject *Sender)
{
   switch(RadioGroup2->ItemIndex)
   {
      case 0 : //Reticle
         Edit3->Text = (String)ROHT_DIST_1ROTATE_MM;
         break;
      case 1 : //Foup ver4.5
         Edit3->Text = (String)WD2_DIST_1ROTATE_MM;
         break;
      case 2 : //Foup ver4.5/5.0
         Edit3->Text = (String)WD4_DIST_1ROTATE_MM;
         break;
      case 3 : //Foup ver6.0
         Edit3->Text = (String)DISTV6_1ROTATE_MM;
         break;
      case 4 : //Foup ver6.5 & TP OHT Ver1.0
         Edit3->Text = (String)DISTV65_1ROTATE_MM;
         break;
   }
}
//---------------------------------------------------------------------------
