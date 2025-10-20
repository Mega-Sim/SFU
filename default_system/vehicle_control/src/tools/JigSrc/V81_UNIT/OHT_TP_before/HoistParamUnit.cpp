//---------------------------------------------------------------------------

#include <vcl.h>
#include <inifiles.hpp>
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#include<iostream.h>
#include<fstream.h>

#pragma hdrstop

#include "HoistParamUnit.h"
#include "TeachingUnit.h"
#include "ConnectUnit.h"
#include "ChkExecUnit.h"
#include "Def_TPCommand.h"
#include "StatusUnit.h"
#include "CMDControlUnit.h"
#include "MainUnit.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "SocketUnit.h"
#include "HoistDistanceConverter.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THoistParamForm *HoistParamForm;
TCHKOHT OHTCheck;
//CHoistConverter HoistConverter;
//---------------------------------------------------------------------------
__fastcall THoistParamForm::THoistParamForm(TComponent* Owner)
   : TForm(Owner)
{
   bParamOK = false;
   InitHoistParam();
}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------//---------------------------------------------------------------------------//---------------------------------------------------------------------------//---------------------------------------------------------------------------//---------------------------------------------------------------------------
void __fastcall THoistParamForm::FormShow(TObject *Sender)
{
   HoistParamForm->Left=(Screen->Width/2)-(HoistParamForm->Width/2);
   HoistParamForm->Top=(Screen->Height/2)-(HoistParamForm->Height/2);

}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------
int THoistParamForm::HoistParamSave(int nShiftSave)
{

   double d3Coeff;
   double d4Coeff;
   int i;
   fstream  m_file;
   String sFileName;
   String sPara, sLine;

   //d3Coeff = 0.00329602;


   d4Coeff = 0.00023483;

   //OHTNameFind();

   sFileName = ExtractFilePath(Application->ExeName) + "HoistParam.ini";

   if(nShiftSave == 0)
   {
      sPara.sprintf(L"%10s,%10s,%10s,%10s,%10s,%10s,%10s,%10s,%10s,%.8f,%.8f,%.8f",
                     "0",                                                                 // 주행축
                     (String) HoistParam.dHoist1, (String) HoistParam.dHoist2,
                     (String) HoistParam.dHoist3, (String) HoistParam.dHoist4,
                     (String) HoistParam.dHoist5, (String) HoistParam.dHoist6,
                     (String) HoistParam.dHoist7, (String) HoistParam.dHoist8,    // Hoist
                     d3Coeff, d4Coeff, HoistParam.dMPSlide);                                                   // Shift, Rotate
   }
   else
   {
      sPara.sprintf(L"%10s,%10s,%10s,%10s,%10s,%10s,%10s,%10s,%10s,%.8f,%.8f,%.8f",
                     "0",                                                                 // 주행축
                     (String) HoistParam.dHoist1, (String) HoistParam.dHoist2,
                     (String) HoistParam.dHoist3, (String) HoistParam.dHoist4,
                     (String) HoistParam.dHoist5, (String) HoistParam.dHoist6,
                     (String) HoistParam.dHoist7, (String) HoistParam.dHoist8,    // Hoist
                     d3Coeff, d4Coeff, gm_OHTStatus->Motor_Shift->dPosition_cts);                                                   // Shift, Rotate
   }                                               // Shift, Rotate

   TIniFile *ini;
   ini = new TIniFile(sFileName);
  // ini->WriteString("TEACHING PARA", "OHT#"+SocketForm->g_strOHTNum, sPara);
   delete ini;

   return true;

}
//---------------------------------------------------------------------------
void THoistParamForm::HoistParamRead(void)
{
   FILE *fp;
   String sFileName;
   char chBuffer[256];
   String sTmpBuffer;
   String sTmpValue;
   int Pos = 0;

   sFileName = ExtractFilePath(Application->ExeName) + "HoistParam.ini";

   if (sFileName.Length() <= 0)
   {
      MainForm->DataDisplay(COLOR_RED, "ERROR : Hoist Param File Name Error...");
      return;
   }

   TIniFile *ini;
   ini = new TIniFile(sFileName);
   //sTmpBuffer = ini->ReadString("TEACHING PARA", "OHT#"+SocketForm->g_strOHTNum, "N/A");
   delete ini;

   Pos=sTmpBuffer.Pos(",");
   while(Pos>0)
   {
      for(int i=1;i<13;i++)
      {
         if( i == 12 )
            sTmpValue = sTmpBuffer;
         else
            sTmpValue = sTmpBuffer.SubString(1,Pos-1);
         sTmpValue.Trim();
         sTmpBuffer.Delete(1, Pos);
         Pos=sTmpBuffer.Pos(",");
         switch(i)
         {
            case 1 :
               HoistParam.dDriving = sTmpValue.ToDouble();
               break;
            case 2 :
               HoistParam.dHoist1 = sTmpValue.ToDouble();
               break;
            case 3 :
               HoistParam.dHoist2 = sTmpValue.ToDouble();
               break;
            case 4 :
               HoistParam.dHoist3 = sTmpValue.ToDouble();
               break;
            case 5 :
               HoistParam.dHoist4 = sTmpValue.ToDouble();
               break;
            case 6 :
               HoistParam.dHoist5 = sTmpValue.ToDouble();
               break;
            case 7 :
               HoistParam.dHoist6 = sTmpValue.ToDouble();
               break;
            case 8 :
               HoistParam.dHoist7 = sTmpValue.ToDouble();
               break;
            case 9 :
               HoistParam.dHoist8 = sTmpValue.ToDouble();
               break;
            case 10 :
               HoistParam.dSlide = sTmpValue.ToDouble();
               break;
            case 11 :
               HoistParam.dRotate = sTmpValue.ToDouble();
               break;
            case 12 :
               HoistParam.dMPSlide = sTmpValue.ToDouble();
               break;
         }
      }
   }

   if( sTmpBuffer != "N/A" )
   {
      HoistConverter->calcCoefficient( HoistParam.dHoist1, HoistParam.dHoist2, HoistParam.dHoist3, HoistParam.dHoist4,
                                         HoistParam.dHoist5, HoistParam.dHoist6, HoistParam.dHoist7, HoistParam.dHoist8 );
      bParamOK = true;
   }
   else
      bParamOK = false;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void THoistParamForm::HoistMoveByPulse(double dblHoistPos)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		ShowMessage(L"메뉴얼 상태에서만 사용 가능합니다!");
		return;
	}
	else
	{
		// MainForm->timerStatusRequest->Enabled = false;

		double dblMoveSpeed = 80;

		// 여기서 기능 실행
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize;
		int iReturn, iTemp;
		AnsiString strMachineID = "TP001";

		// 이적재 명령 구조체
		iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
		PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;

		// 구조체 초기화
		memset(&SEND_PACKET, 0, iPacketSize);

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

		SEND_PACKET.TAG.Value = TagID::CMD_TP_HOIST_TEACH_MOVE_TO_PULSE;
		SEND_PACKET.BODY.PositionValue[0].dPulse = dblHoistPos;

		SEND_PACKET.BODY.PositionValue[1].dPulse = dblMoveSpeed;	// 새로 추가함. 속도 선택

		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

		delete m_CMDControl;
	}
}

void __fastcall THoistParamForm::btnMove_0Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_0->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_1Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_1->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_2Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_2->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_3Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_3->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_4Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_4->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_5Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_5->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_6Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_6->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnParamLoadClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";
	
	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
	PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;
	
	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	
	SEND_PACKET.TAG.Value = TagID::CMD_TP_HOIST_TEACH_LOAD_VALUE;	
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	
	delete m_CMDControl;

	InitHoistParam();

}

//---------------------------------------------------------------------------
//Hoist 파라미터값 초기화
void THoistParamForm::InitConfirm()
{
	edtRealMM_master->ReadOnly = false;
	btnConfirm_master->Enabled = true;
	btnCancel_master->Enabled = false;

	edtRealMM_zero->ReadOnly = false;
	btnMove_zero->Enabled = true;
	btnConfirm_zero->Enabled = true;
	btnCancel_zero->Enabled = false;

	edtRealMM_0->ReadOnly = false;
	btnMove_0->Enabled = true;
	btnConfirm_0->Enabled = true;
	btnCancel_0->Enabled = false;

	edtRealMM_1->ReadOnly = false;
	btnMove_1->Enabled = true;
	btnConfirm_1->Enabled = true;
	btnCancel_1->Enabled = false;

	edtRealMM_2->ReadOnly = false;
	btnMove_2->Enabled = true;
	btnConfirm_2->Enabled = true;
	btnCancel_2->Enabled = false;

	edtRealMM_3->ReadOnly = false;
	btnMove_3->Enabled = true;
	btnConfirm_3->Enabled = true;
	btnCancel_3->Enabled = false;

	edtRealMM_4->ReadOnly = false;
	btnMove_4->Enabled = true;
	btnConfirm_4->Enabled = true;
	btnCancel_4->Enabled = false;

	edtRealMM_5->ReadOnly = false;
	btnMove_5->Enabled = true;
	btnConfirm_5->Enabled = true;
	btnCancel_5->Enabled = false;

	edtRealMM_6->ReadOnly = false;
	btnMove_6->Enabled = true;
	btnConfirm_6->Enabled = true;
	btnCancel_6->Enabled = false;

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  == OHT_TYPE_EUV) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85))
	{
		edtRealMM_7->Visible = true;
		btnMove_7->Visible = true;
		btnConfirm_7->Visible = true;
		btnCancel_7->Visible = true;

		edtRealMM_8->Visible = true;
		btnMove_8->Visible = true;
		btnConfirm_8->Visible = true;
		btnCancel_8->Visible = true;

		edtRealMM_9->Visible = true;
		btnMove_9->Visible = true;
		btnConfirm_9->Visible = true;
		btnCancel_9->Visible = true;

		edtRealMM_7->ReadOnly = false;
		btnMove_7->Enabled = true;
		btnConfirm_7->Enabled = true;
		btnCancel_7->Enabled = false;

		edtRealMM_8->ReadOnly = false;
		btnMove_8->Enabled = true;
		btnConfirm_8->Enabled = true;
		btnCancel_8->Enabled = false;

		edtRealMM_9->ReadOnly = false;
		btnMove_9->Enabled = true;
		btnConfirm_9->Enabled = true;
		btnCancel_9->Enabled = false;
	}else
	{
		edtRealMM_7->Visible = false;
		btnMove_7->Visible = false;
		btnConfirm_7->Visible = false;
		btnCancel_7->Visible = false;

		edtRealMM_8->Visible = false;
		btnMove_8->Visible = false;
		btnConfirm_8->Visible = false;
		btnCancel_8->Visible = false;

		edtRealMM_9->Visible = false;
		btnMove_9->Visible = false;
		btnConfirm_9->Visible = false;
		btnCancel_9->Visible = false;
    }


}
void THoistParamForm::InitHoistParam()
{
	edtPosMM_master->Text = "0";
	edtRealMM_master->Text = "0";


	edtRealMM_zero->Text = "0";

	edtPulse_0->Text = "0";
	edtPosMM_0->Text = "0";
	edtRealMM_0->Text = "0";

	edtPulse_1->Text = "0";
	edtPosMM_1->Text = "0";
	edtRealMM_1->Text = "0";

	edtPulse_2->Text = "0";
	edtPosMM_2->Text = "0";
	edtRealMM_2->Text = "0";

	edtPulse_3->Text = "0";
	edtPosMM_3->Text = "0";
	edtRealMM_3->Text = "0";

	edtPulse_4->Text = "0";
	edtPosMM_4->Text = "0";
	edtRealMM_4->Text = "0";

	edtPulse_5->Text = "0";
	edtPosMM_5->Text = "0";
	edtRealMM_5->Text = "0";

	edtPulse_6->Text = "0";
	edtPosMM_6->Text = "0";
	edtRealMM_6->Text = "0";

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  == OHT_TYPE_EUV) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85))
	{
		edtPulse_7->Visible = true;
		edtPosMM_7->Visible =true;
		edtRealMM_7->Visible = true;

		edtPulse_8->Visible = true;
		edtPosMM_8->Visible =true;
		edtRealMM_8->Visible = true;

		edtPulse_9->Visible = true;
		edtPosMM_9->Visible =true;
		edtRealMM_9->Visible = true;

		edtPulse_7->Text = "0";
		edtPosMM_7->Text = "0";
		edtRealMM_7->Text = "0";

		edtPulse_8->Text = "0";
		edtPosMM_8->Text = "0";
		edtRealMM_8->Text = "0";

		edtPulse_9->Text = "0";
		edtPosMM_9->Text = "0";
		edtRealMM_9->Text = "0";
	}else
	{
		edtPulse_7->Visible = false;
		edtPosMM_7->Visible =false;
		edtRealMM_7->Visible = false;

		edtPulse_8->Visible = false;
		edtPosMM_8->Visible =false;
		edtRealMM_8->Visible = false;

		edtPulse_9->Visible = false;
		edtPosMM_9->Visible =false;
		edtRealMM_9->Visible = false;
	}

	InitConfirm();
}

void THoistParamForm::Display_Result(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT)
{
	edtPulse_0->Text = FloatToStr(PKT.BODY.PositionValue[0].dPulse);
	edtPosMM_0->Text = FloatToStr(PKT.BODY.PositionValue[0].dUserUnit);

	edtPulse_1->Text = FloatToStr(PKT.BODY.PositionValue[1].dPulse);
	edtPosMM_1->Text = FloatToStr(PKT.BODY.PositionValue[1].dUserUnit);

	edtPulse_2->Text = FloatToStr(PKT.BODY.PositionValue[2].dPulse);
	edtPosMM_2->Text = FloatToStr(PKT.BODY.PositionValue[2].dUserUnit);

	edtPulse_3->Text = FloatToStr(PKT.BODY.PositionValue[3].dPulse);
	edtPosMM_3->Text = FloatToStr(PKT.BODY.PositionValue[3].dUserUnit);

	edtPulse_4->Text = FloatToStr(PKT.BODY.PositionValue[4].dPulse);
	edtPosMM_4->Text = FloatToStr(PKT.BODY.PositionValue[4].dUserUnit);

	edtPulse_5->Text = FloatToStr(PKT.BODY.PositionValue[5].dPulse);
	edtPosMM_5->Text = FloatToStr(PKT.BODY.PositionValue[5].dUserUnit);

	edtPulse_6->Text = FloatToStr(PKT.BODY.PositionValue[6].dPulse);
	edtPosMM_6->Text = FloatToStr(PKT.BODY.PositionValue[6].dUserUnit);

	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  == OHT_TYPE_EUV) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85))
	{
		edtPulse_7->Text = FloatToStr(PKT.BODY.PositionValue[7].dPulse);
		edtPosMM_7->Text = FloatToStr(PKT.BODY.PositionValue[7].dUserUnit);

		edtPulse_8->Text = FloatToStr(PKT.BODY.PositionValue[8].dPulse);
		edtPosMM_8->Text = FloatToStr(PKT.BODY.PositionValue[8].dUserUnit);

		edtPulse_9->Text = FloatToStr(PKT.BODY.PositionValue[9].dPulse);
		edtPosMM_9->Text = FloatToStr(PKT.BODY.PositionValue[9].dUserUnit);

	}

}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnParamSaveClick(TObject *Sender)
{

	
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";
	
	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);
	PKT_TP2OHT_HOIST_TEACH_COMMAND	SEND_PACKET;
	
	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
	
	SEND_PACKET.TAG.Value = TagID::CMD_TP_HOIST_TEACH_SAVE_VALUE;

    //pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Hoist -= (int)m_pExecuteInfo->TransDifference.dHoist;

	SEND_PACKET.BODY.dHomeOffsetOfRailToJigOnUserUnit =  StrToFloat(edtPosMM_master->Text);

	// value save
	SEND_PACKET.BODY.PositionValue[0].dPulse 	= StrToFloat(edtPulse_0->Text);
	SEND_PACKET.BODY.PositionValue[0].dUserUnit = StrToFloat(edtPosMM_0->Text);

	SEND_PACKET.BODY.PositionValue[1].dPulse 	= StrToFloat(edtPulse_1->Text);
	SEND_PACKET.BODY.PositionValue[1].dUserUnit = StrToFloat(edtPosMM_1->Text);

	SEND_PACKET.BODY.PositionValue[2].dPulse 	= StrToFloat(edtPulse_2->Text);
	SEND_PACKET.BODY.PositionValue[2].dUserUnit = StrToFloat(edtPosMM_2->Text);

	SEND_PACKET.BODY.PositionValue[3].dPulse 	= StrToFloat(edtPulse_3->Text);
	SEND_PACKET.BODY.PositionValue[3].dUserUnit = StrToFloat(edtPosMM_3->Text);

	SEND_PACKET.BODY.PositionValue[4].dPulse 	= StrToFloat(edtPulse_4->Text);
	SEND_PACKET.BODY.PositionValue[4].dUserUnit = StrToFloat(edtPosMM_4->Text);

	SEND_PACKET.BODY.PositionValue[5].dPulse 	= StrToFloat(edtPulse_5->Text);
	SEND_PACKET.BODY.PositionValue[5].dUserUnit = StrToFloat(edtPosMM_5->Text);

	SEND_PACKET.BODY.PositionValue[6].dPulse 	= StrToFloat(edtPulse_6->Text);
	SEND_PACKET.BODY.PositionValue[6].dUserUnit = StrToFloat(edtPosMM_6->Text);


	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  == OHT_TYPE_EUV) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85))
	{
		SEND_PACKET.BODY.PositionValue[7].dPulse 	= StrToFloat(edtPulse_7->Text);
		SEND_PACKET.BODY.PositionValue[7].dUserUnit = StrToFloat(edtPosMM_7->Text);

		SEND_PACKET.BODY.PositionValue[8].dPulse 	= StrToFloat(edtPulse_8->Text);
		SEND_PACKET.BODY.PositionValue[8].dUserUnit = StrToFloat(edtPosMM_8->Text);

		SEND_PACKET.BODY.PositionValue[9].dPulse 	= StrToFloat(edtPulse_9->Text);
		SEND_PACKET.BODY.PositionValue[9].dUserUnit = StrToFloat(edtPosMM_9->Text);

	}

	// -----------------------------------------
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	
	delete m_CMDControl;

}
//---------------------------------------------------------------------------




void __fastcall THoistParamForm::btnMove_7Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_7->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_8Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_8->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnMove_9Click(TObject *Sender)
{
	double dblTarget;

	dblTarget = edtPulse_9->Text.ToDouble();
	HoistMoveByPulse(dblTarget);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::edtRealMM_masterKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
	 //pTmeCycleData->EXE_Trans_Info.TransCmd.Tposition_Hoist -= (int)m_pExecuteInfo->TransDifference.dHoist;
	//master 3500, slave 3600
	//Teachng 1000, teaching 1100

}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_masterClick(TObject *Sender)
{
	edtRealMM_master->ReadOnly = true;
	btnConfirm_master->Enabled = false;
	btnCancel_master->Enabled = true;


	int Tempgap = 0;
	Tempgap = edtRealMM_zero->Text.ToInt() - edtRealMM_master->Text.ToInt();
	edtPosMM_master->Text = IntToStr(Tempgap);
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_masterClick(TObject *Sender)
{
	edtRealMM_master->ReadOnly = false;
	btnConfirm_master->Enabled = true;
	btnCancel_master->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_zeroClick(TObject *Sender)
{
	edtRealMM_zero->ReadOnly = true;
	btnMove_zero->Enabled = false;
	btnConfirm_zero->Enabled = false;
	btnCancel_zero->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_zeroClick(TObject *Sender)
{
	edtRealMM_zero->ReadOnly = false;
	btnMove_zero->Enabled = true;
	btnConfirm_zero->Enabled = true;
	btnCancel_zero->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_0Click(TObject *Sender)
{
	edtRealMM_0->ReadOnly = true;
	btnMove_0->Enabled = false;
	btnConfirm_0->Enabled = false;
	btnCancel_0->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_0Click(TObject *Sender)
{
	edtRealMM_0->ReadOnly = false;
	btnMove_0->Enabled = true;
	btnConfirm_0->Enabled = true;
	btnCancel_0->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_1Click(TObject *Sender)
{
	edtRealMM_1->ReadOnly = true;
	btnMove_1->Enabled = false;
	btnConfirm_1->Enabled = false;
	btnCancel_1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_1Click(TObject *Sender)
{
	edtRealMM_1->ReadOnly = false;
	btnMove_1->Enabled = true;
	btnConfirm_1->Enabled = true;
	btnCancel_1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_2Click(TObject *Sender)
{
	edtRealMM_2->ReadOnly = true;
	btnMove_2->Enabled = false;
	btnConfirm_2->Enabled = false;
	btnCancel_2->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_2Click(TObject *Sender)
{
	edtRealMM_2->ReadOnly = false;
	btnMove_2->Enabled = true;
	btnConfirm_2->Enabled = true;
	btnCancel_2->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_3Click(TObject *Sender)
{
	edtRealMM_3->ReadOnly = true;
	btnMove_3->Enabled = false;
	btnConfirm_3->Enabled = false;
	btnCancel_3->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_3Click(TObject *Sender)
{
	edtRealMM_3->ReadOnly = false;
	btnMove_3->Enabled = true;
	btnConfirm_3->Enabled = true;
	btnCancel_3->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_4Click(TObject *Sender)
{
	edtRealMM_4->ReadOnly = true;
	btnMove_4->Enabled = false;
	btnConfirm_4->Enabled = false;
	btnCancel_4->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_4Click(TObject *Sender)
{
	edtRealMM_4->ReadOnly = false;
	btnMove_4->Enabled = true;
	btnConfirm_4->Enabled = true;
	btnCancel_4->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_5Click(TObject *Sender)
{
	edtRealMM_5->ReadOnly = true;
	btnMove_5->Enabled = false;
	btnConfirm_5->Enabled = false;
	btnCancel_5->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_5Click(TObject *Sender)
{
	edtRealMM_5->ReadOnly = false;
	btnMove_5->Enabled = true;
	btnConfirm_5->Enabled = true;
	btnCancel_5->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_6Click(TObject *Sender)
{
	edtRealMM_6->ReadOnly = true;
	btnMove_6->Enabled = false;
	btnConfirm_6->Enabled = false;
	btnCancel_6->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_6Click(TObject *Sender)
{
	edtRealMM_6->ReadOnly = false;
	btnMove_6->Enabled = true;
	btnConfirm_6->Enabled = true;
	btnCancel_6->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_7Click(TObject *Sender)
{
	edtRealMM_7->ReadOnly = true;
	btnMove_7->Enabled = false;
	btnConfirm_7->Enabled = false;
	btnCancel_7->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_7Click(TObject *Sender)
{
	edtRealMM_7->ReadOnly = false;
	btnMove_7->Enabled = true;
	btnConfirm_7->Enabled = true;
	btnCancel_7->Enabled = false;
}
//---------------------------------------------------------------------------


void __fastcall THoistParamForm::btnConfirm_8Click(TObject *Sender)
{
	edtRealMM_8->ReadOnly = true;
	btnMove_8->Enabled = false;
	btnConfirm_8->Enabled = false;
	btnCancel_8->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_8Click(TObject *Sender)
{
	edtRealMM_8->ReadOnly = false;
	btnMove_8->Enabled = true;
	btnConfirm_8->Enabled = true;
	btnCancel_8->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnConfirm_9Click(TObject *Sender)
{
	edtRealMM_9->ReadOnly = true;
	btnMove_9->Enabled = false;
	btnConfirm_9->Enabled = false;
	btnCancel_9->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall THoistParamForm::btnCancel_9Click(TObject *Sender)
{
	edtRealMM_9->ReadOnly = false;
	btnMove_9->Enabled = true;
	btnConfirm_9->Enabled = true;
	btnCancel_9->Enabled = false;
}
//---------------------------------------------------------------------------

