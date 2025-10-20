//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HIDGraphUnit.h"
#include "OHTInfoUnit.h"
#include "TimeSettingUnit.h"
#include "SocketUnit.h"
#include "Define.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THIDGraphForm *HIDGraphForm;
//---------------------------------------------------------------------------
__fastcall THIDGraphForm::THIDGraphForm(TComponent* Owner)
   : TForm(Owner)
{
   nTmpCount=0;
   fTmpCount=0.0;
}
//---------------------------------------------------------------------------
void __fastcall THIDGraphForm::Timer1Timer(TObject *Sender)
{
   String strHIDValue, strNode;
   int n1Axis, n2Axis;
   nTmpCount++;

   fTmpCount=fTmpCount+0.2;

   if(fTmpCount>=(float)nxTime)
   {
//      Chart1->BottomAxis->Automatic = true;
//	  Series1->Delete(0);
//	  Series2->Delete(0);
//	  Series3->Delete(0);
//	  Series4->Delete(0);
//
//	  Chart2->BottomAxis->Automatic = true;
//	  FastLineSeries1->Delete(0);
//	  FastLineSeries2->Delete(0);
   }

   strNode.sprintf(L"N%06d",gm_OHTStatus->m_OHTStatus.Data.CurNodeID);

//   Series1->AddXY(fTmpCount,gm_OHTStatus->HIDData->nErec,strNode,COLOR_BLACK);
//   Series2->AddXY(fTmpCount,gm_OHTStatus->HIDData->nEdc,strNode,COLOR_BLACK);
//   Series3->AddXY(fTmpCount,gm_OHTStatus->HIDData->nIdc,strNode,COLOR_BLACK);


   n1Axis = (int)gm_OHTStatus->Motor_Driving_F->dVelocity_rpm;
   n2Axis = (int)gm_OHTStatus->Motor_Hoist->dVelocity_rpm;
//   FastLineSeries1->AddXY(fTmpCount,n1Axis,strNode,COLOR_BLACK);
//   FastLineSeries2->AddXY(fTmpCount,n2Axis,strNode,COLOR_BLACK);


   if(gm_OHTStatus->HIDData->nEdc>=nEdc_MaxValue)
      nEdc_MaxValue=gm_OHTStatus->HIDData->nEdc;

   if(gm_OHTStatus->HIDData->nEdc<=nEdc_MinValue)
      nEdc_MinValue=gm_OHTStatus->HIDData->nEdc;

   if(gm_OHTStatus->HIDData->nErec>=nErec_MaxValue)
      nErec_MaxValue=gm_OHTStatus->HIDData->nErec;

   if(gm_OHTStatus->HIDData->nErec<=nErec_MinValue)
      nErec_MinValue=gm_OHTStatus->HIDData->nErec;

   StatusBar1->Panels->Items[0]->Text="ONLINE";
   StatusBar1->Panels->Items[1]->Text="N"+IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID);
   StatusBar1->Panels->Items[2]->Text="Edc ="+IntToStr(gm_OHTStatus->HIDData->nEdc);
   StatusBar1->Panels->Items[3]->Text="Erc ="+IntToStr(gm_OHTStatus->HIDData->nErec);
/*
   StatusBar1->Panels->Items[4]->Text.sprintf(L"%d%d%d%d %d%d%d%d",
                                 gm_OHTStatus->HIDData->nErecUnderVoltage,
                                 gm_OHTStatus->HIDData->nErecOverVoltage,
                                 gm_OHTStatus->HIDData->nIdcOverCurrent,
                                 gm_OHTStatus->HIDData->nEdcUnderVoltage,
                                 gm_OHTStatus->HIDData->nEdcOverVoltage,
                                 gm_OHTStatus->HIDData->nOverTemperature,
                                 gm_OHTStatus->HIDData->nFuseOpen
                                );
*/
   StatusBar1->Panels->Items[4]->Text="Idc ="+IntToStr(gm_OHTStatus->HIDData->nIdc);
   StatusBar1->Panels->Items[5]->Text = FloatToStr(fTmpCount);

   StatusBar1->Panels->Items[6]->Text = "#1:"+FloatToStr(gm_OHTStatus->Motor_Driving_F->dVelocity_rpm);
   StatusBar1->Panels->Items[7]->Text = "#2:"+FloatToStr(gm_OHTStatus->Motor_Hoist->dVelocity_rpm);

   //strHIDValue.sprintf(L"Edc_Max=%03d Edc_Min=%03d Erec_Max=%03d Erec_Min=%03d", nEdc_MaxValue,nEdc_MinValue,nErec_MaxValue,nErec_MinValue);
   //HIDGraphForm->Caption = strHIDValue;

}
//---------------------------------------------------------------------------
void THIDGraphForm::Graph_Start(int nTime_min)
{
//   Series1->Clear();
//   Series2->Clear();
//   Series3->Clear();
//   Series4->Clear();
//
//   FastLineSeries1->Clear();
//   FastLineSeries2->Clear();


//   Chart1->BottomAxis->Automatic = true;
//   Chart2->BottomAxis->Automatic = true;
   nxTime = nTime_min*60;
//   Chart1->BottomAxis->Automatic = false;
//   Chart1->BottomAxis->Minimum = 0.0;
//   Chart1->BottomAxis->Maximum = (float)nxTime;

//   Chart2->BottomAxis->Automatic = false;
//   Chart2->BottomAxis->Minimum = 0.0;
//   Chart2->BottomAxis->Maximum = (float)nxTime;


   nTmpCount=0;
   fTmpCount=0.0;
   StatusBar1->Panels->Items[0]->Text="";
   StatusBar1->Panels->Items[1]->Text="";
   StatusBar1->Panels->Items[2]->Text="";
   StatusBar1->Panels->Items[3]->Text="";
   nEdc_MaxValue=0;
   nEdc_MinValue=500;
   nErec_MaxValue=0;
   nErec_MinValue=500;

   Timer1->Enabled = true;
   popStart->Enabled=false;
   popEnd->Enabled=true;

}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::popEndClick(TObject *Sender)
{
   Timer1->Enabled = false;
   StatusBar1->Panels->Items[0]->Text="OFFLINE";
   popStart->Enabled=true;
   popEnd->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   Timer1->Enabled = false;
   StatusBar1->Panels->Items[0]->Text="OFFLINE";
   popStart->Enabled=true;
   popEnd->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::FileOpen1Click(TObject *Sender)
{
   String strTmpFileName;
 //  int nError=0;

   OpenDialog1->Title = "Open Saved HID Data";
   Timer1->Enabled = false;
   popStart->Enabled=true;
   popEnd->Enabled=false;
   if (OpenDialog1->Execute() == false) return;
   strTmpFileName = OpenDialog1->FileName;

   if (strTmpFileName.Length() <= 0) return;
//   Chart1->BottomAxis->Automatic = true;
//   Chart2->BottomAxis->Automatic = true;
//   Series1->Clear();
//   Series2->Clear();
//   Series3->Clear();
//   Series4->Clear();
   LogParse(strTmpFileName);
/*
   if(!nError)
      HIDGraphForm->Caption = "HID Data... Good!!";
   else
      HIDGraphForm->Caption = "HID Data... Bad!!";
*/
}
//---------------------------------------------------------------------------
int THIDGraphForm::LogParse(String strTmpFileName)
{
   int i;
   String strLineData;
   TStringList *tMapData = new TStringList;
   int nEdc, nErec, dd;
   String strCurNode;
   int j;
   tMapData->LoadFromFile(strTmpFileName);

   for(i=0; i<tMapData->Count; i++)
   {
      strLineData = tMapData->Strings[i];
      if((strLineData.Length()==0) || (strLineData =="")) continue;
      j=10;
      strCurNode = strLineData.SubString(j,4);
      j+=6;
      nErec = StrToIntDef(strLineData.SubString(j,3),0);
      j+=3;
      nEdc = StrToIntDef(strLineData.SubString(j,3),0);

//      Series1->AddXY(i,nErec,strCurNode,COLOR_RED);
//      Series2->AddXY(i,nEdc,strCurNode,COLOR_BLUE);
   }
   delete tMapData;
   return 0;
}
//---------------------------------------------------------------------------
void __fastcall THIDGraphForm::SaveImage1Click(TObject *Sender)
{
   String strBmpName, strBmpName1;

   if(SPicture->Execute())
   {
      strBmpName = SPicture->FileName;
      strBmpName1 = strBmpName+"_1";
      if((strBmpName.Pos(".bmp")==0)&&(strBmpName.Pos(".bmp")==0))
      {
         strBmpName += ".bmp";
         strBmpName1 += ".bmp";
      }
//      Chart1->SaveToBitmapFile(strBmpName);
//      Chart2->SaveToBitmapFile(strBmpName1);
   }
   
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::Chart1MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{

//   float fErec;
   double tmpX,tmpY;
//   String strAxis;

//   fErec = Series1->YScreenToValue(Y);
//   strAxis.sprintf(L"E=%.0f", fErec);
//   HIDGraphForm->Caption = strAxis;

//   Shape1->Top = 0;
//   Shape2->Left = 0;

/****
   Memo1->Clear();
   Memo1->Lines->Add(FloatToStr(fErec));
   Memo1->Lines->Add(IntToStr(Y));
   Memo1->Lines->Add(IntToStr(HIDGraphForm->Height));
   Memo1->Lines->Add(IntToStr(Chart1->Height3D));
   Memo1->Lines->Add(IntToStr(Shape1->Top));
***/

//   Series1->GetCursorValues(tmpX,tmpY);
//   HIDGraphForm->Caption = "E="+Series1->GetVertAxis->LabelValue(tmpY);

}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::popStartClick(TObject *Sender)
{
   TimeSettingForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::Chart1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
//   Shape1->Top = Y;
//   Shape2->Left = X;
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::Chart1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
//   Shape1->Top = 0;
//   Shape2->Left = 0;
}
//---------------------------------------------------------------------------

void __fastcall THIDGraphForm::FormShow(TObject *Sender)
{
   HIDGraphForm->Caption = "OHT#"+SocketForm->m_strOHTNum;
}
//---------------------------------------------------------------------------

