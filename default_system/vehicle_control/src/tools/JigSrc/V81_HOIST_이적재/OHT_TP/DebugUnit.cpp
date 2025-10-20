//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include "DebugUnit.h"
#include "Define.h"
#include "DisUnit.h"
#include "ConnectUnit.h"
#include "OHTInfoUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDebugForm *DebugForm;
//---------------------------------------------------------------------------
__fastcall TDebugForm::TDebugForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDebugForm::FormShow(TObject *Sender)
{
   DebugEdit->Left=Panel1->Left;
//   DebugEdit->Top=Panel1->Top;
   DebugEdit->Width=Panel1->Width;
//   DebugEdit->Height=Panel1->Height;
}
//---------------------------------------------------------------------------



void __fastcall TDebugForm::DebugEditKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
   if((Key==VK_RETURN)&&(DebugEdit->Text!=""))
   {
      DebugListB->Items->Add(DebugEdit->Text);
      DebugEdit->Clear();
   }
}
//---------------------------------------------------------------------------
void __fastcall TDebugForm::DebugListBDblClick(TObject *Sender)
{
   int nListNum;
   String strListData, strDisData;

   nListNum=DebugListB->ItemIndex;
   DebugListB->Checked[nListNum]=true;
   strListData=DebugListB->Items->Strings[nListNum];
   strDisData.sprintf(L"[%d]:%s", nListNum, strListData);
   DebugForm->Caption=strDisData;

   DataDisplay(strListData);
}
//---------------------------------------------------------------------------
void __fastcall TDebugForm::Clear1Click(TObject *Sender)
{
   DebugListB->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TDebugForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   DebugEdit->Clear();
   DebugListB->Clear();
}
//---------------------------------------------------------------------------
void TDebugForm::DataDisplay(String strTmpData)
{

   TDisForm *DisForm = new TDisForm(NULL);
   DisForm->Show();

   String strdata;
   DisForm->Caption=strTmpData;

   if(strTmpData.Length()==0)
   {
      DisForm->Memo1->Lines->Add("Debug Data Error..");
      return;
   }

   if(strTmpData.Pos("Sts")==0)
   {
      DisForm->Memo1->Lines->Add("Debug Data Error..");
      return;
   }

   int i, nPoss;
// 21:00:00 Sts:$sAU00800340000I101101001100101000000100U0101000000011011100O00000000000000000000P3200000000000C111111111
   nPoss=strTmpData.Pos("$");

   strdata=strTmpData.SubString(1,nPoss-1);
   DisForm->Memo1->Lines->Add("["+strdata+"]");

   i=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Mode                   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Status                 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,3);
   DisForm->Memo1->Lines->Add("CurNode                : " + strdata);

   i+=3;
   strdata=strTmpData.SubString(nPoss+i,3);
   DisForm->Memo1->Lines->Add("TargetNode             : " + strdata);

   i+=5;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("CurID                  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("NextID                 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,3);
   DisForm->Memo1->Lines->Add("ErrorCode              : " + strdata);

//*** PMAC Input
   DisForm->Memo1->Lines->Add("==[PMAC Input]==");

   i+=4;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand InPosition        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Alarm             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Gripper Right          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Gripper Left           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Close Sensor      : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Open Sensor       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved1         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved2         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Stop Sensor      : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Stop Sensor       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle 1ST           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle 2ND           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Direction Front Left   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Direction Front Right  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Direction Rear Left    : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Direction Rear Right   : " + strdata);

   if((gm_OHTStatus->strOHTVer!=OHTVER_V10LP)&&(gm_OHTStatus->strOHTVer!=OHTVER_V20_MZ))
   {
      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("LookDown Sensor        : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("LookDown Error         : " + strdata);
   }
   else
   {
      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("LookDown Sensor        : " + strdata);

      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("LookDown Error         : " + strdata);
   }

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hoist Align Sensor     : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Oscillation Detect Sen : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Watch Dog Error        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Sen _ Long   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Sen _ Middle  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Sen _ Short   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Sen _ Fail    : " + strdata);


//*** User IO Input
   DisForm->Memo1->Lines->Add("==[UserIO Input]==");

   i+=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Shutter Close Sensor   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Shutter Open Sensor    : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Shutter Close Sensor    : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Shutter Open Sensor     : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("E-Stop                 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Bumper Press           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PMAC WatchDog Error    : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("EQ_PIO GO Signal       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand PIO GO Signal     : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Alarm1           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Alarm2           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Alarm3           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input1          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input2          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input3          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input4          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input5          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input6          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input7          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input8          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input9          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input10          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input11          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Input12          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Remote Strobe           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("WatchDogControl Status  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("WatchDog SigOut Enable  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Driving Running        : " + strdata);


   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hoist Running          : " + strdata);


   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Shift Running          : " + strdata);


   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rotate Running         : " + strdata);

// PMAC Output
   DisForm->Memo1->Lines->Add("==[PMAC Output]==");

   i+=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Close             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Open              : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Homming           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reset             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved1         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved2         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved3         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Reserved4         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle Area Select1  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle Area Select2  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle Area Select3  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle Right Enable  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Obstacle Left Enable   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Main Reset             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Steering Left          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Steering Right         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Steering Power Reset   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Shutter Power Reset    : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Power Reset       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Area Select1 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Area Select2 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Area Select3 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Area Select4 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("OHTDetect Reset        : " + strdata);

//*** User IO Output
   DisForm->Memo1->Lines->Add("==[User IO Output]==");

   i+=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Lamp_Red         : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Lamp_Yellow      : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Front Lamp_Green       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Lamp_Red          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Lamp_Yellow       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Rear Lamp_Green        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("MP3 Select1[Error]     : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("MP3 Select2[Go]        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("MP3 Select3[Trans]     : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("MP3 Select4[Reserved]  : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("WatchDog Enable        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("WatchDog Signal        : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Safety Clear           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PMAC Reset             : " + strdata);

   if((gm_OHTStatus->strOHTVer!=OHTVER_V10LP)&&(gm_OHTStatus->strOHTVer!=OHTVER_V20_MZ))
   {
      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("LookDown Select1       : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("LookDown Select2       : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("LookDown Select3       : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("Driving Brake Release  : " + strdata);
   }
   else
   {
      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("LookDown Select1       : " + strdata);

      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("LookDown Select2       : " + strdata);

      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("LookDown Select3       : " + strdata);

      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("Driving Brake Release  : " + strdata);
   }

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Power Reset      : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("SMPS Power Reset       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Select1          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Select2          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Select3          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Select4          : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Servo Alarm Reset      : " + strdata);


//*** PIO
   DisForm->Memo1->Lines->Add("==[PIO]==");

   i+=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("ProcessID              : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("TransFlag              : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_L_REQ              : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_U_REQ              : " + strdata);


   if(gm_OHTStatus->nIsRFPIOType==PIOTYPE_E84)
   {

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("PIO_READY              : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("PIO_HO_AVBL            : " + strdata);

      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("PIO_ES                 : " + strdata);
   }
   else
   {
      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("PIO_ABORT              : " + strdata);


      i++;
      strdata=strTmpData.SubString(nPoss+i,1);
      DisForm->Memo1->Lines->Add("PIO_READY              : " + strdata);      

      i++;
      strdata="X";
      DisForm->Memo1->Lines->Add("PIO_RESERVED           : " + strdata);
   }

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_VALID              : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_CS                 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIOLR_REQ             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_BUSY               : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("PIO_COMPT              : " + strdata);

// En/Disable
   DisForm->Memo1->Lines->Add("==[En/Disable]==");

   i+=2;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("BCR Read               : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("STB Sensor             : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("HandSensor Check       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Hand Control           : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Sound Alarm Flag       : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Lamp                   : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("EQ Out                 : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Direction Control      : " + strdata);

   i++;
   strdata=strTmpData.SubString(nPoss+i,1);
   DisForm->Memo1->Lines->Add("Shutter Control        : " + strdata);

}
void __fastcall TDebugForm::DebugListBClick(TObject *Sender)
{
   nClearLineNum=DebugListB->ItemIndex;
}
//---------------------------------------------------------------------------
