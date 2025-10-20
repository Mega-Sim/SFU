//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#pragma hdrstop

#include "AmcDebugMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAmcDebugForm *AmcDebugForm;
//---------------------------------------------------------------------------
__fastcall TAmcDebugForm::TAmcDebugForm(TComponent* Owner)
   : TForm(Owner)
{
   PageControl1->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TAmcDebugForm::AnalysisClick(TObject *Sender)
{
   // 분석.
   Memo2->Clear();
   PageControl1->ActivePageIndex = 1;
   if(LogTypeRadio->ItemIndex == 0)
      AnalysisData();
   else
      AnalysisData2();
}
//---------------------------------------------------------------------------
void __fastcall TAmcDebugForm::Clear1Click(TObject *Sender)
{
   PageControl1->ActivePageIndex = 0;
   Memo1->Clear();
   Memo2->Clear();
}
//---------------------------------------------------------------------------
void TAmcDebugForm::AnalysisData(void)
{  // Error Log의 debug Data.
   String strReadData = "";
   String strDebugData1 = "", strDebugData2 = "", strDebugData3 = "", strDebugData4 = "";
   String strWriteData = "";
   int nReadData1 = 0, nReadData2 = 0, nReadData3 = 0;
   float fReadData = 0.0, fReadData2 = 0.0;
//   int nDataCount = 0;


   strReadData = Memo1->Lines->Strings[0];
   if(strReadData == "" || strReadData == NULL) return;

   strDebugData1 = strReadData.SubString(3,1);
   strDebugData2 = strReadData.SubString(4,1);
   strWriteData.sprintf(L"OHT Version = %s.%s",strDebugData1, strDebugData2);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(5,1);
   strWriteData.sprintf(L"Status = %s", strDebugData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(6,3), 0);
   strWriteData.sprintf(L"nErrorCode = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(94,1);
   strWriteData.sprintf(L"chIPCStatus = %s", strDebugData1);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(95,1);
   strWriteData.sprintf(L"chDebugStatus = %s", strDebugData1);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(96,1);
   strWriteData.sprintf(L"chDebugProcessID = %s", strDebugData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(97,3), 0);
   strWriteData.sprintf(L"nDebugError = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(9,4), 0);
   strWriteData.sprintf(L"nNodeCount_Sum = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(13,1);
   if(strDebugData1 == "-")
   {
      fReadData = StrToFloat(strReadData.SubString(14,8));
      strWriteData.sprintf(L"nNode_Dist_cts = -%f", fReadData);
   }
   else
   {
      fReadData = StrToFloat(strReadData.SubString(13,9));
      strWriteData.sprintf(L"nNode_Dist_cts = %f", fReadData);
   }
   Memo2->Lines->Add(strWriteData);


   strDebugData1 = strReadData.SubString(22,1);
   if(strDebugData1 == "-")
   {
      fReadData = StrToFloat(strReadData.SubString(23,8));
      strWriteData.sprintf(L"dFSen_Dist_cts = -%.02f", fReadData);
   }
   else
   {
      fReadData = StrToFloat(strReadData.SubString(22,9));
      strWriteData.sprintf(L"dFSen_Dist_cts = %.02f", fReadData);
   }
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(31,1);
   if(strDebugData1 == "-")
   {
      fReadData = StrToFloat(strReadData.SubString(32,8));
      strWriteData.sprintf(L"dRSen_Dist_cts = -%.02f", fReadData);
   }
   else
   {
      fReadData = StrToFloat(strReadData.SubString(31,9));
      strWriteData.sprintf(L"dRSen_Dist_cts = %.02f", fReadData);
   }
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = StrToIntDef(strReadData.SubString(40,1), 0);
   strWriteData.sprintf(L"nAxis = %s", strDebugData1);
   Memo2->Lines->Add(strWriteData);

   fReadData = StrToFloat(strReadData.SubString(41,6));
   fReadData2 = StrToFloat(strReadData.SubString(48,2));
   strWriteData.sprintf(L"dAcceleration = %.02f",fReadData + fReadData2/100);
   Memo2->Lines->Add(strWriteData);

   fReadData = StrToFloat(strReadData.SubString(50,6));
   fReadData2 = StrToFloat(strReadData.SubString(57,2));
   strWriteData.sprintf(L"dDeceleration = %.02f",fReadData + fReadData2/100);
   Memo2->Lines->Add(strWriteData);

   fReadData = StrToFloat(strReadData.SubString(59,6));
   fReadData2 = StrToFloat(strReadData.SubString(66,2));
   strWriteData.sprintf(L"dSpeed = %.02f",fReadData + fReadData2/100);
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(68,1);
   fReadData = StrToFloat(strReadData.SubString(69,9));
   fReadData2 = StrToFloat(strReadData.SubString(79,2));
    if(strDebugData1 == "-")
   {
      strWriteData.sprintf(L"dTargetDistance = -%.02f", fReadData + fReadData2/100);
   }
   else
   {
      strWriteData.sprintf(L"dTargetDistance = +%.02f", fReadData + fReadData2/100);
   }
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(81,1);
   fReadData = StrToFloat(strReadData.SubString(82,9));
   fReadData2 = StrToFloat(strReadData.SubString(92,2));
   if(strDebugData1 == "-")
   {
      strWriteData.sprintf(L"dCurrentPosition = -%.02f", fReadData + fReadData2/100);
   }
   else
   {
      strWriteData.sprintf(L"dCurrentPosition = +%.02f", fReadData + fReadData2/100);
   }
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(100,1);
   fReadData = StrToFloat(strReadData.SubString(101,9));
   fReadData2 = StrToFloat(strReadData.SubString(111,2));
   if(strDebugData1 == "-")
   {
      strWriteData.sprintf(L"dFinalStopOBSDistance = -%.02f", fReadData + fReadData2/100);
   }
   else
   {
      strWriteData.sprintf(L"dFinalStopOBSDistance = +%.02f", fReadData + fReadData2/100);
   }
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"bFinalStopOBSFlag = %s", strReadData.SubString(113,1));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"bBumperPressed = %s", strReadData.SubString(114,1));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"bVHLPauseCmd = %s", strReadData.SubString(115,1));
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(116,1), 0);
   if(nReadData1 == 1)   // 1 : TP, 2 : OCS
      strWriteData.sprintf(L"nVHLPausedBy = %d(TP)", nReadData1);
   else if(nReadData1 == 2)
      strWriteData.sprintf(L"nVHLPausedBy = %d(OCS)", nReadData1);
   else
      strWriteData.sprintf(L"nVHLPausedBy = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(117,1), 0);
   if(nReadData1 == 1)   // 1 : TP, 2 : OCS
      strWriteData.sprintf(L"nVHLPaused = %d(TP)", nReadData1);
   else if(nReadData1 == 2)
      strWriteData.sprintf(L"nVHLPaused = %d(OCS)", nReadData1);
   else
      strWriteData.sprintf(L"nVHLPaused = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(118,1), 0);
   strWriteData.sprintf(L"nStartGoFlag = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(119,1), 0);
   strWriteData.sprintf(L"nStopGoFlag = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(120,1), 0);
   strWriteData.sprintf(L"nGoCommandCheck = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"bMove_ds_No_Move_RetryFlag = %s", strReadData.SubString(121,1));
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(122,4), 0);
   strWriteData.sprintf(L"nTempMoveCount = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(126,4), 0);
   strWriteData.sprintf(L"nTempStopCount = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(130,1), 0);
   strWriteData.sprintf(L"nPBS_ActivationLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(131,1), 0);
   strWriteData.sprintf(L"nOBS_ActivationLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(132,1), 0);
   strWriteData.sprintf(L"nPBS_Detect = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(133,1), 0);
   strWriteData.sprintf(L"nOBS_Detect = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(134,1), 0);
   strWriteData.sprintf(L"nPBS_SetValue = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(135,1), 0);
   strWriteData.sprintf(L"nOBS_SetValue = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(136,1), 0);
   strWriteData.sprintf(L"nCalculatedDetect = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(137,1), 0);
   nReadData2 = (nReadData1>>1)&0x1;
   nReadData3 = nReadData1&0x1;
   strWriteData.sprintf(L"nTagDetect = %d[FLag %d, RLag %d]", nReadData1, nReadData2, nReadData3);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(138,1), 0);
   strWriteData.sprintf(L"gm_StatusUpdateFlag = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   return;
}
//---------------------------------------------------------------------------
void __fastcall TAmcDebugForm::LogClear1Click(TObject *Sender)
{
   Memo1->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TAmcDebugForm::ResultClear1Click(TObject *Sender)
{
   Memo2->Clear();
}
//---------------------------------------------------------------------------
void TAmcDebugForm::AnalysisData2(void)
{  // AmcData 로그 분석용
   String strReadData = "";
   String strDebugData1 = "", strDebugData2 = "", strDebugData3 = "", strDebugData4 = "";
   String strWriteData = "";
   int nReadData1 = 0, nReadData2 = 0, nReadData3 = 0;
   float fReadData = 0.0, fReadData2 = 0.0;
   int nDataCount = 0;
   int nOhtVersion = 0;


   strReadData = Memo1->Lines->Strings[0];
   if(strReadData == "" || strReadData == NULL) return;

   // 초기 값이 시간여부를 확인..
   // 시간이 아니면.. 타입이 다르다 라고 하고.. 메세지 표시..
   strDebugData1 = strReadData.SubString(1,11);
   nReadData1 = strDebugData1.Pos(":");
   if(!nReadData1)
   {
      ShowMessage("Data Format Error!!");
      return;
   }

   if(strReadData.SubString(13,1) != "S")
   {
      ShowMessage("This Data is not Status Data for Log Analysis.");
      return;
   }

   strWriteData.sprintf(L"Time = %s", strReadData.SubString(1,11));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"chIPCStatus = %s", strReadData.SubString(15,1));
   Memo2->Lines->Add(strWriteData);

   strDebugData1 = strReadData.SubString(17,1);
   strDebugData2 = strReadData.SubString(18,1);
   nOhtVersion =  StrToIntDef(strReadData.SubString(17,2),0);
   strWriteData.sprintf(L"OHT Version = %s.%s",strDebugData1, strDebugData2);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(20,2), 0);
   strWriteData.sprintf(L"nPBS_ActivationLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(23,2), 0);
   strWriteData.sprintf(L"nPBS_ActivationLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"Cmd Response = %s", strReadData.SubString(29,1));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"chStatus = %s", strReadData.SubString(30,1));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"chDebugStatus = %s", strReadData.SubString(31,1));
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"bFinalStopOBSFlag = %s", strReadData.SubString(32,1));
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(33,1), 0);
   strWriteData.sprintf(L"nVHLPaused = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"chDebugProcessID = %s", strReadData.SubString(34,1));
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(35,3), 0);
   strWriteData.sprintf(L"nDebugError = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(38,4), 0);
   strWriteData.sprintf(L"nNodeCount_Sum = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(42,4), 0);
   strWriteData.sprintf(L"nErrorCode = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   if(nOhtVersion == 60)
   {
      strDebugData1 = strReadData.SubString(47,1);
      if(strDebugData1 == "-")
      {
         nReadData1 = StrToIntDef(strReadData.SubString(48,8),0);
         strWriteData.sprintf(L"nNode_Dist_cts = -%d", nReadData1);
      }
      else
      {
         nReadData1 = StrToIntDef(strReadData.SubString(47,9),0);
         strWriteData.sprintf(L"nNode_Dist_cts = %d", nReadData1);
      }
      Memo2->Lines->Add(strWriteData);
      nDataCount = 56;
   }
   else if(nOhtVersion == 65)
   {
      strDebugData1 = strReadData.SubString(47,1);
      if(strDebugData1 == "-")
      {
         nReadData1 = StrToIntDef(strReadData.SubString(48,8),0);
         strWriteData.sprintf(L"dFSen_Dist_cts = -%d", nReadData1);
      }
      else
      {
         nReadData1 = StrToIntDef(strReadData.SubString(47,9),0);
         strWriteData.sprintf(L"dFSen_Dist_cts = %d", nReadData1);
      }
      Memo2->Lines->Add(strWriteData);

      strDebugData1 = strReadData.SubString(56,1);
      if(strDebugData1 == "-")
      {
         nReadData1 = StrToIntDef(strReadData.SubString(57,8),0);
         strWriteData.sprintf(L"dRSen_Dist_cts = -%d", nReadData1);
      }
      else
      {
         nReadData1 = StrToIntDef(strReadData.SubString(56,9),0);
         strWriteData.sprintf(L"dRSen_Dist_cts = %d", nReadData1);
      }
      Memo2->Lines->Add(strWriteData);

      nDataCount = 65;
   }
   else
   {
      strWriteData.sprintf(L"OHT Version 오류.. %d", nOhtVersion);
      ShowMessage(strWriteData);
      return;
   }

   strDebugData1 = strReadData.SubString(nDataCount+1,1);
   fReadData = StrToFloat(strReadData.SubString(nDataCount+2,9));
   fReadData2 = StrToFloat(strReadData.SubString(nDataCount+12,2));
   if(strDebugData1 == "-")
   {
      strWriteData.sprintf(L"dDebugTargetDistance = -%.02f", fReadData + fReadData2/100);
   }
   else
   {
      strWriteData.sprintf(L"dDebugTargetDistance = +%.02f", fReadData + fReadData2/100);
   }
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+15,1), 0);
   strWriteData.sprintf(L"OBS Detect = Long %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+16,1), 0);
   strWriteData.sprintf(L"OBS Detect = Midd %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+17,1), 0);
   strWriteData.sprintf(L"OBS Detect = Shor %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+18,1), 0);
   if(nReadData1 == 1) nReadData1 = 0;
   else if(nReadData1 == 0) nReadData1 = 1;
   strWriteData.sprintf(L"OBS Detect = Erro %d", nReadData1);  
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+20,1), 0);
   strWriteData.sprintf(L"PBS Detect = Long %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+21,1), 0);
   strWriteData.sprintf(L"PBS Detect = Midd %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+22,1), 0);
   strWriteData.sprintf(L"PBS Detect = Shor %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+23,1), 0);
   strWriteData.sprintf(L"PBS Detect = Erro %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   strWriteData.sprintf(L"gm_Detect_SpeedLevel = %s", strReadData.SubString(nDataCount+25,1));
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+27,2), 0);
   strWriteData.sprintf(L"gm_PBSDetectLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+30,2), 0);
   strWriteData.sprintf(L"gm_nPBSRcvActivationData = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+33,2), 0);
   strWriteData.sprintf(L"gm_OBSDetectLevel = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+36,2), 0);
   strWriteData.sprintf(L"gm_nOBSRcvActivationData = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+38,1), 0);
   strWriteData.sprintf(L"bBumperPressed = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+40,4), 0);
   strWriteData.sprintf(L"gmLempStopCount = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+44,4), 0);
   strWriteData.sprintf(L"gmLempMoveCount = %d", nReadData1);
   Memo2->Lines->Add(strWriteData);

   nReadData1 = StrToIntDef(strReadData.SubString(nDataCount+48,1), 0);
   nReadData2 = (nReadData1>>1)&0x1;
   nReadData3 = nReadData1&0x1;
   strWriteData.sprintf(L"nTag Sensor = %d[FLag %d, RLag %d]", nReadData1, nReadData2, nReadData3);
   Memo2->Lines->Add(strWriteData);
}
