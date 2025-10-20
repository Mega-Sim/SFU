//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HIDVelUnit.h"
#include "ConversionUnit.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THIDVelForm *HIDVelForm;
//---------------------------------------------------------------------------
__fastcall THIDVelForm::THIDVelForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall THIDVelForm::Draw1Click(TObject *Sender)
{
//17:00:09.16 [MI]N000513 295,294,007,020,00,000,000,000 : Pos=[7094631.000][7138722.000] Speed=[0.000][0.000][m/sec][1 1]

   int i;
   int nPos;
   String strData, strDist_mm, strDist_cts, strSpeed_mpersec;
   double dDist_mm, dDist_cts, dSpeed_mpersec;
   TCONVERSION *m_Conversion = new TCONVERSION;
   double d1RotateDist_mm=0.0;

   Series1->Clear();

   d1RotateDist_mm=DISTV6_1ROTATE_MM;



   for(i=0; i<memHIDLog->Lines->Count; i++)
   {
      strData = memHIDLog->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;
      nPos = strData.Pos("Pos=[");
      strData.Delete(1, nPos+4);
      nPos = strData.Pos("]");
      strDist_cts = strData.SubString(1, nPos-1);
      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dDist_cts = StrToFloat(strDist_cts);
      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);
      dDist_mm=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm);

      Series1->AddXY(dDist_mm,dSpeed_mpersec,"",COLOR_BLACK);
   }
   delete m_Conversion;
}
//---------------------------------------------------------------------------
void __fastcall THIDVelForm::Clear1Click(TObject *Sender)
{
   Series1->Clear();
   memHIDLog->Clear();
}
//---------------------------------------------------------------------------
