//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HIDVelSecUnit.h"
#include "ConversionUnit.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THIDVelSecForm *HIDVelSecForm;
//---------------------------------------------------------------------------
__fastcall THIDVelSecForm::THIDVelSecForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall THIDVelSecForm::Clear1Click(TObject *Sender)
{
   Series1->Clear();
   Series2->Clear();
   Series3->Clear();
   memHIDLog1->Clear();
   memHIDLog2->Clear();
   memHIDLog3->Clear();
}
//---------------------------------------------------------------------------
void __fastcall THIDVelSecForm::Capture1Click(TObject *Sender)
{
   String strBmpName;

   if(OpenDialog1->Execute())
   {
      strBmpName = OpenDialog1->FileName;
      if((strBmpName.Pos(".bmp")==0)&&(strBmpName.Pos(".bmp")==0))
         strBmpName += ".bmp";
      Chart1->SaveToBitmapFile(strBmpName);
   }

}
//---------------------------------------------------------------------------
void __fastcall THIDVelSecForm::Drawsec1Click(TObject *Sender)
{
//17:00:09.16 [MI]N000513 295,294,007,020,00,000,000,000 : Pos=[7094631.000][7138722.000] Speed=[0.000][0.000][m/sec][1 1]

   int i;
   int nPos;
   String strData, strSpeed_mpersec;
   double dTime_sec, dSpeed_mpersec;
   double dStTime;
   int nCount=0;
   double dMin, dSec;

   Series1->Clear();
   Series2->Clear();
   Series3->Clear();

   for(i=0; i<memHIDLog1->Lines->Count; i++)
   {
      strData = memHIDLog1->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;
      nCount++;

      if(nCount==1)
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dStTime = dMin*60.0+dSec;
         dTime_sec=0.0;
      }
      else
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dTime_sec = dMin*60.0+dSec - dStTime;
      }

      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);

      Series1->AddXY(dTime_sec,dSpeed_mpersec,"",COLOR_BLACK);
   }

   nCount=0;
   for(i=0; i<memHIDLog2->Lines->Count; i++)
   {
      strData = memHIDLog2->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;
      nCount++;

      if(nCount==1)
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dStTime = dMin*60.0+dSec;
         dTime_sec=0.0;
      }
      else
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dTime_sec = dMin*60.0+dSec - dStTime;
      }

      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);

      Series2->AddXY(dTime_sec,dSpeed_mpersec,"",COLOR_BLACK);
   }

   nCount=0;
   for(i=0; i<memHIDLog3->Lines->Count; i++)
   {
      strData = memHIDLog3->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;
      nCount++;

      if(nCount==1)
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dStTime = dMin*60.0+dSec;
         dTime_sec=0.0;
      }
      else
      {
         dMin = StrToFloat(strData.SubString(4,2));
         dSec = StrToFloat(strData.SubString(7,5));
         dTime_sec = dMin*60.0+dSec - dStTime;
      }

      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);

      Series3->AddXY(dTime_sec,dSpeed_mpersec,"",COLOR_BLACK);
   }   
}
//---------------------------------------------------------------------------
void __fastcall THIDVelSecForm::Drawmm1Click(TObject *Sender)
{
//17:00:09.16 [MI]N000513 295,294,007,020,00,000,000,000 : Pos=[7094631.000][7138722.000] Speed=[0.000][0.000][m/sec][1 1]

   int i;
   int nPos;

   String strData, strDist_mm, strDist_cts, strSpeed_mpersec;
   double dDist_mm, dDist_cts, dSpeed_mpersec;
   TCONVERSION *m_Conversion = new TCONVERSION;
   double d1RotateDist_mm=0.0;
   double dStartDist=0.0;
   int nCount=0;

   Series1->Clear();
   Series2->Clear();
   Series3->Clear();


   d1RotateDist_mm=DISTV6_1ROTATE_MM;


   nCount=0;
   for(i=0; i<memHIDLog1->Lines->Count; i++)
   {
      strData = memHIDLog1->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;

      nPos = strData.Pos("Pos=[");
      strData.Delete(1, nPos+4);
      nPos = strData.Pos("]");
      strDist_cts = strData.SubString(1, nPos-1);

      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);
      dDist_cts = StrToFloat(strDist_cts);
      nCount++;
      if(nCount==1)dStartDist=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm);
      dDist_mm=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm)-dStartDist;

      Series1->AddXY(dDist_mm,dSpeed_mpersec,"",COLOR_BLACK);
   }

   nCount=0;
   for(i=0; i<memHIDLog2->Lines->Count; i++)
   {
      strData = memHIDLog2->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;

      nPos = strData.Pos("Pos=[");
      strData.Delete(1, nPos+4);
      nPos = strData.Pos("]");
      strDist_cts = strData.SubString(1, nPos-1);

      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);
      dDist_cts = StrToFloat(strDist_cts);
      nCount++;
      if(nCount==1)dStartDist=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm);
      dDist_mm=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm)-dStartDist;

      Series2->AddXY(dDist_mm,dSpeed_mpersec,"",COLOR_BLACK);
   }

   nCount=0;
   for(i=0; i<memHIDLog3->Lines->Count; i++)
   {
      strData = memHIDLog3->Lines->Strings[i];
      if((strData.Pos("Pos")==0)||(strData.Pos("Speed")==0)) continue;

      nPos = strData.Pos("Pos=[");
      strData.Delete(1, nPos+4);
      nPos = strData.Pos("]");
      strDist_cts = strData.SubString(1, nPos-1);
      
      nPos = strData.Pos("Speed=[");
      strData.Delete(1, nPos+6);
      nPos = strData.Pos("]");
      strSpeed_mpersec = strData.SubString(1, nPos-1);

      dSpeed_mpersec = StrToFloat(strSpeed_mpersec);
      dDist_cts = StrToFloat(strDist_cts);
      nCount++;
      if(nCount==1)dStartDist=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm);
      dDist_mm=m_Conversion->CTS_to_MM(dDist_cts,COUNT_1ROTATE,d1RotateDist_mm)-dStartDist;

      Series3->AddXY(dDist_mm,dSpeed_mpersec,"",COLOR_BLACK);
   }
}
//---------------------------------------------------------------------------
