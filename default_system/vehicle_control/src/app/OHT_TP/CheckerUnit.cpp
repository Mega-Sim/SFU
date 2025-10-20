//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckerUnit.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "OHTInfoUnit.h"
#include "ChkExecUnit.h"
#include "SensorSettingUnit.h"
#include "CidCheckerUnit.h"
//#include "LogFileUnit.h"

TCHKOHT TechOHTCheck;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define Log_Steering		0
#define Log_ShiftHome		1
#define Log_HoistHome		2

#define Log_Step_Init			100
#define Log_Step_Check			101
#define Log_Step_Done			102

#define CheckTime_30s		30*1000
#define	CheckTime_1m		1*60*1000
#define CheckTime_5m		5*60*1000

UnicodeString strTitle[5] =
{
	// 1
	L"Steering Front Right",
	L"Steering Rear Right",
	L"Steering Front Left",
	L"Steering Rear Left",
	L"검사 종료",
};

UnicodeString strTitleShift[3] =
{
	// 1
	L"Shift Right",
	L"Shift Left",
	L"검사 종료",
};

UnicodeString strTitleHoist[2] =
{
	// 1
	L"Hoist Down",
	L"검사 종료",
};

UnicodeString strTitleEStop[4] =
{
	// 1
	L"E-STOP Press ON",
	L"E-STOP Error OFF",
	L"Bumper Press ON",
	L"Bumper Error OFF",

};
UnicodeString strTitleOscilation[4] =
{
	// 1
	L"Hoist 300mm↓ 이적재",
	L"Oscil 1차 감지 후 정지",
	L"약 60초 감지 후 Error",
	L"Oscilation Error OFF",

};
UnicodeString strTitleLookDown[4] =
{
	// 1
	L"LookDown Area 설정",
	L"LookDown 감지",
	L"약 10초 감지 후 Error",
	L"LookDown Error OFF",

};
UnicodeString strTitleSTBDetect[1] =
{
	// 1
	L"STB 이적재 후 Error",
//	L"STB Error 확인",

};
UnicodeString strTitleEQ[2] =
{
	// 1
	L"EQ PORT 이적재 명령",
	L"EQ PORT Error 확인",

};
UnicodeString strTitleFoupDetect[3] =
{
	// 1
	L"FoupDetect 감지",
	L"Unload 명령 인가",
	L"FoupDetect Error OFF",

};

UnicodeString strTitleFoupCover[2] =
{
	// 1
	L"FoupCover 감지",
	L"FoupDetect Error OFF",

};

TUnitChecker *UnitChecker;
//---------------------------------------------------------------------------
__fastcall TUnitChecker::TUnitChecker(TComponent* Owner)
	: TForm(Owner)
{
	for(int i = 0; i < 5; i++)
	{
		strngrd1->Cells[0][i + 1] = IntToStr(i + 1);
		strngrd1->Cells[1][i + 1] = strTitle[i];
	}

	strngrd1->Cells[0][0] = L"순서";
	strngrd1->Cells[1][0] = L"검사내용";
	strngrd1->Cells[2][0] = L"검사결과";
	strngrd1->DefaultRowHeight = 16;
	strngrd1->ColWidths[0] = 26;

	for(int i = 0; i < 3; i++)
	{
		strngrdShiftHome->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdShiftHome->Cells[1][i + 1] = strTitleShift[i];
	}

	strngrdShiftHome->Cells[0][0] = L"순서";
	strngrdShiftHome->Cells[1][0] = L"검사내용";
	strngrdShiftHome->Cells[2][0] = L"검사결과";
	strngrdShiftHome->DefaultRowHeight = 16;
	strngrdShiftHome->ColWidths[0] = 26;

	for(int i = 0; i < 2; i++)
	{
		strngrdHoist->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdHoist->Cells[1][i + 1] = strTitleHoist[i];
	}

	strngrdHoist->Cells[0][0] = L"순서";
	strngrdHoist->Cells[1][0] = L"검사내용";
	strngrdHoist->Cells[2][0] = L"검사결과";
	strngrdHoist->DefaultRowHeight = 16;
	strngrdHoist->ColWidths[0] = 26;

	for(int i = 0; i < 4; i++)
	{
		strngrdEStop1->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdEStop1->Cells[1][i + 1] = strTitleEStop[i];
	}
	strngrdEStop1->Cells[0][0] = L"순서";
	strngrdEStop1->Cells[1][0] = L"검사내용";
	strngrdEStop1->Cells[2][0] = L"검사결과";
	strngrdEStop1->DefaultRowHeight = 16;
	strngrdEStop1->ColWidths[0] = 26;
	strngrdEStop1->ColWidths[1] = 120;
	strngrdEStop1->ColWidths[2] = 80;

	for(int i = 0; i < 4; i++)
	{
		strngrdOscilation->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdOscilation->Cells[1][i + 1] = strTitleOscilation[i];
	}
	strngrdOscilation->Cells[0][0] = L"순서";
	strngrdOscilation->Cells[1][0] = L"검사내용";
	strngrdOscilation->Cells[2][0] = L"검사결과";
	strngrdOscilation->DefaultRowHeight = 16;
	strngrdOscilation->ColWidths[0] = 26;
	strngrdOscilation->ColWidths[1] = 120;
	strngrdOscilation->ColWidths[2] = 80;

	for(int i = 0; i < 2; i++)
	{
		strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
	}
	strngrdLookDown->Cells[0][0] = L"순서";
	strngrdLookDown->Cells[1][0] = L"검사내용";
	strngrdLookDown->Cells[2][0] = L"검사결과";
	strngrdLookDown->DefaultRowHeight = 16;
	strngrdLookDown->ColWidths[0] = 26;
	strngrdLookDown->ColWidths[1] = 120;
	strngrdLookDown->ColWidths[2] = 80;


	for(int i = 0; i < 1; i++)
	{
		strngrdSTBDetect->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdSTBDetect->Cells[1][i + 1] = strTitleSTBDetect[i];
	}
	strngrdSTBDetect->Cells[0][0] = L"순서";
	strngrdSTBDetect->Cells[1][0] = L"검사내용";
	strngrdSTBDetect->Cells[2][0] = L"검사결과";
	strngrdSTBDetect->DefaultRowHeight = 16;
	strngrdSTBDetect->ColWidths[0] = 26;
	strngrdSTBDetect->ColWidths[1] = 120;
	strngrdSTBDetect->ColWidths[2] = 80;

	for(int i = 0; i < 2; i++)
	{
		strngrdEQ->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdEQ->Cells[1][i + 1] = strTitleEQ[i];
	}
	strngrdEQ->Cells[0][0] = L"순서";
	strngrdEQ->Cells[1][0] = L"검사내용";
	strngrdEQ->Cells[2][0] = L"검사결과";
	strngrdEQ->DefaultRowHeight = 16;
	strngrdEQ->ColWidths[0] = 26;
	strngrdEQ->ColWidths[1] = 120;
	strngrdEQ->ColWidths[2] = 80;

	for(int i = 0; i < 3; i++)
	{
		strngrdFoupDetect->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdFoupDetect->Cells[1][i + 1] = strTitleFoupDetect[i];
	}
	strngrdFoupDetect->Cells[0][0] = L"순서";
	strngrdFoupDetect->Cells[1][0] = L"검사내용";
	strngrdFoupDetect->Cells[2][0] = L"검사결과";
	strngrdFoupDetect->DefaultRowHeight = 16;
	strngrdFoupDetect->ColWidths[0] = 26;
	strngrdFoupDetect->ColWidths[1] = 120;
	strngrdFoupDetect->ColWidths[2] = 80;


	for(int i = 0; i < 3; i++)
	{
		strngrdFoupCover->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdFoupCover->Cells[1][i + 1] = strTitleFoupCover[i];
	}
	strngrdFoupCover->Cells[0][0] = L"순서";
	strngrdFoupCover->Cells[1][0] = L"검사내용";
	strngrdFoupCover->Cells[2][0] = L"검사결과";
	strngrdFoupCover->DefaultRowHeight = 16;
	strngrdFoupCover->ColWidths[0] = 26;
	strngrdFoupCover->ColWidths[1] = 120;
	strngrdFoupCover->ColWidths[2] = 80;



}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrIOUITimer(TObject *Sender)
{
	FStrLeftLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) ? COLOR_LIME : COLOR_WHITE;
	RStrLeftLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left 	== ON) ? COLOR_LIME : COLOR_WHITE;
	FStrRightLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right 	== ON) ? COLOR_LIME : COLOR_WHITE;
	RStrRightLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right 	== ON) ? COLOR_LIME : COLOR_WHITE;

	Shp_ShiftHome->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== ON) ? COLOR_LIME : COLOR_WHITE;

	ShpHoist->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpEStop->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.EMO_Press 	== OFF) ? COLOR_LIME : COLOR_WHITE;
	shpBumperPress->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpOscilation->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.OSCILLATION 	== ON) ? COLOR_LIME : COLOR_WHITE;

	shpLookDown->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpSTBLDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpSTBRDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpFoupDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpFoupCover->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect	== ON) ? COLOR_LIME : COLOR_WHITE;


	struct date today;
	struct time currenttime;
	String Time;

	getdate(&today);
	gettime(&currenttime);
	Time.sprintf(L"현재시간 : %04d-%02d-%02d : %02d:%02d:%02d", today.da_year, today.da_mon, today.da_day,
					currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec);

	lblCurrentTime->Caption = Time;


	if(chkRailTest->Checked == true)
	{
		for(int i = 0; i < 4; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
			strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
		}
		lblLookDownTime->Visible = true;
	}
	else
	{
		for(int i = 0; i < 2; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
			strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
		}
		for(int i = 2; i < 4; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = "";
			strngrdLookDown->Cells[1][i + 1] = "";
		}
		lblLookDownTime->Visible = false;
	}
	lblVHLName->Caption = "VHL_NAME : "+SocketForm->m_strOHTName;
}

void TUnitChecker::MaxDistShiftHoist(int iIndex, int MaxDist, bool IsPass,bool IsHoist)
{
	if(!IsHoist)
	{
		if(IsPass)
		{
		   if(MaxDist == 1)
				strngrdShiftHome->Cells[2][iIndex] = L"Pass";
		   else
				strngrdShiftHome->Cells[2][iIndex] = L"Fail";
		}
		else
			strngrdShiftHome->Cells[2][iIndex] = IntToStr(MaxDist);
	}
	else
	{
        if(IsPass)
		{
		   if(MaxDist == 1)
				strngrdHoist->Cells[2][iIndex] = L"Pass";
		   else
				strngrdHoist->Cells[2][iIndex] = L"Fail";
		}
		else
			strngrdHoist->Cells[2][iIndex] = IntToStr(MaxDist);
    }

}

//---------------------------------------------------------------------------
void TUnitChecker::Pass(int iIndex, bool bPass)
{
	if(bPass)
		strngrd1->Cells[2][iIndex] = L"Pass";
	else
		strngrd1->Cells[2][iIndex] = L"Fail";
}
//---------------------------------------------------------------------------
void TUnitChecker::PassOrNot(TStringGrid* a, int iIndex, bool bPass)
{
	if(bPass)
	{
		struct time currenttime;
		String Time;

		gettime(&currenttime);
		Time.sprintf(L"%02d:%02d:%02d.%03d",currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec, currenttime.ti_hund);
//		lblCurrentTime->Caption = Time;
		a->Cells[2][iIndex] = Time;//L"Pass";
	}
	else
		a->Cells[2][iIndex] = L"Fail";
}
//---------------------------------------------------------------------------
void TUnitChecker::PassOrNot(TStringGrid* a, int iIndex, bool bPass, double dSec)
{
	if(bPass != true)
		return;

	a->Cells[2][iIndex] = String(L"%.2f",dSec);
}
//---------------------------------------------------------------------------
void TUnitChecker::Log(UnicodeString strLog, int Type)
{
	switch(Type)
	{
		case Log_Steering:
		{
		mmoMemo->Lines->Add(strLog);
		}
		break;
		case Log_ShiftHome:
		{
		mmoShiftHome->Lines->Add(strLog);
		}
		break;
		case Log_HoistHome:
		{
        mmoHoist->Lines->Add(strLog);
}
		break;
		case Log_Step_Init:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_WHITE;
		}
		break;
		case Log_Step_Check:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_YELLOW;
		}
		break;
		case Log_Step_Done:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_LIME;
		}
		break;
		default:
		{
			;
		}
		break;

	}
}

void TUnitChecker::LastTime(DWORD dwTmpTime, DWORD dwRefTime, bool bUse)
{
	String strTmpTime;
	if(bUse == true)
	{
		DWORD dwCheckTime = dwRefTime - dwTmpTime;
		strTmpTime.sprintf(L"%02d분 %02d초", (int)(dwCheckTime/1000/60), (int)((dwCheckTime/1000)%60));
		lblLimitTime->Caption = strTmpTime;
	}
	else
	{
		lblLimitTime->Caption = L"00분 00초";
	}
}
//void TUnitChecker::OACISLocalLog(String strLog)
//{
//	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
//	m_LogFile->WriteLogFile("OACIS",SocketForm->m_strOHTIPAddress+" : "+strLog);
//	delete m_LogFile;
//}

void TUnitChecker::OACISDataSendTP2OHT(short type, short typeNum,int Data1,int Data2,int Data3,int Data4)
{
  	int iPacketSize = sizeof(PKT_OHT2TP_OACIS_INFO);
	int iReturn;

	PKT_OHT2TP_OACIS_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_OACIS_INFO_SAVE;

	SEND_PACKET.BODY.Type = type;
	SEND_PACKET.BODY.TypeNum = typeNum;
	SEND_PACKET.BODY.nData1 = Data1;
	SEND_PACKET.BODY.nData2 = Data2;
	SEND_PACKET.BODY.nData3 = Data3;
	SEND_PACKET.BODY.nData4 = Data4;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}



void TUnitChecker::StatusMachineShift(int iCmd)
{
	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 int iRet;
	 static int nCountCheck = 0;

     AnsiString strWanted_Result;

	 // 커맨드 처리
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Shift::StartShiftMove;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulationShift->Enabled = false;
			iStatus = Sim_Step_Shift::None;
			return;
		}
		break;
	 }

	 tmrSimulationShift->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_Shift::None: 	// (0) Not work
		 {
			;
		 }
		 break;


		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step_Shift::StartShiftMove:
		 {
			 Log("Shift Check Home");
			 iStatus++;
		 }
		 break;

		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step_Shift::StartShiftRight:
		 {
			 ShiftINCMove(30,0);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftRight_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == OFF)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 7.0)
					{
						Log("Shift Right OK",1);
						MaxDistShiftHoist(1,int(gm_OHTStatus->Motor_Shift->dPosition_mm*10));
						iStatus++;
					}else
					{
						MaxDistShiftHoist(1,2,true);
						Log("Shift Right Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}


				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 10.0)
					{
						MaxDistShiftHoist(1,2,true);
						Log("Shift Right Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}else
					{
						ShiftINCMove(3,0);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(1,2,true);
					Log("Shift Right Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;




		  case Sim_Step_Shift::StartShiftHome1:
		 {
			 TechOHTCheck.Axis_Shift_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftHome1_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 1.0)
					{
						Log("Shift Home1 OK",1);
						iStatus++;
					}else
					{
						Log("Shift Home1 Fail",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}
				}
				else
				{
					Log("Shift Home1 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Shift Home1 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		  // === (1) VHL 외부베터리를 연결
		 case Sim_Step_Shift::StartShiftLeft:
		 {
			 ShiftINCMove(30,1);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftLeft_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == OFF)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 7.0)
					{
						Log("Shift Left OK",1);
						MaxDistShiftHoist(2,int(gm_OHTStatus->Motor_Shift->dPosition_mm*10));
						iStatus++;
					}else
					{
						MaxDistShiftHoist(2,2,true);
						Log("Shift Left Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}


				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 10.0)
					{
						MaxDistShiftHoist(2,2,true);
						Log("Shift Left Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}else
					{
						ShiftINCMove(3,1);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(2,2,true);
					Log("Shift Left Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		   case Sim_Step_Shift::StartShiftHome2:
		 {
			 TechOHTCheck.Axis_Shift_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftHome2_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 1.0)
					{
						Log("Shift Home2 OK",1);
						iStatus++;
					}else
					{
						Log("Shift Home2 Fail",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}
				}
				else
				{
					Log("Shift Home2 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Shift Home2 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		 case Sim_Step_Shift::ShiftCheckComplete:
		 {
			Log("(5) Shift Check Complete",1);


			int nOK =0;
			if(fabs((fabs(StrToInt(strngrdShiftHome->Cells[2][1])) -fabs(StrToInt(strngrdShiftHome->Cells[2][2])))) <= 10)
			{
				nOK = 1;
			}
			else
			{
				nOK = 2;
            }

			MaxDistShiftHoist(3,nOK,true);

			OACISDataSendTP2OHT(CATEGORY_SHIFTHOME,0,nOK,StrToInt(strngrdShiftHome->Cells[2][2]),StrToInt(strngrdShiftHome->Cells[2][1]));
			tmrSimulationShift->Enabled = false;
			iStatus = Sim_Step_Shift::None;
		 }
		 break;

	 }

	 if(iStatus != Sim_Step_Shift::None)
		tmrSimulationShift->Enabled = true;

}


void TUnitChecker::StatusMachine(int iCmd)
{
	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 bool bFailFlag = false;

     AnsiString strWanted_Result;

	 // 커맨드 처리
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_ST::StartSteeringMove;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
			delete m_CMDControl;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step_ST::None;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
			delete m_CMDControl;
			return;
		}
		break;
	 }

	 tmrSimulation->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_ST::None: 	// (0) Not work
		 {
			;
		 }
		 break;

		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step_ST::StartSteeringMove:
		 {
			 Log("StartSteeringMove Default Steering");
			 iStatus++;
		 }
		 break;


		 case Sim_Step_ST::StartSteeringFRight:
		 {

			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) &&
				(gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON))
			{
					TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
					m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_FRONT_RIGHT);
					delete m_CMDControl;
					Log("STEER_F_Right Cmd");
					iStatus++;
			}
			else
			{
				Log("Default Steering Fail!");
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;

			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringFRight_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Front가 Right로 이동을 확인하였습니까?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_F_Right Pass");
					Pass(1, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_F_Right Fail");
					Pass(1, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }

			}
			else
			{
				Log("STEER_F_Right Fail");
				Pass(1, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;


		 case Sim_Step_ST::StartSteeringRRight:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_REAR_RIGHT);
			delete m_CMDControl;
			Log("STEER_R_Right Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringRRight_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Rear가 Right로 이동을 확인하였습니까?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_R_Right Pass");
					Pass(2, true);
					iStatus++;
				}
				else
				{
					Log("STEER_R_Right Fail");
					Pass(2, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }
			}
			else
			{
				Log("STEER_R_Right Fail");
				Pass(2, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringFLeft:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_FRONT_LEFT);
			delete m_CMDControl;
			Log("STEER_F_Left Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringFLeft_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Front가 Left로 이동을 확인하였습니까?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_F_Left Pass");
					Pass(3, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_F_Left Fail");
					Pass(3, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }
			}
			else
			{
				Log("STEER_F_Left Fail");
				Pass(3, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringRLeft:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_REAR_LEFT);
			delete m_CMDControl;
			Log("STEER_R_Left Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringRLeft_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON)&&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Rear가 Left로 이동을 확인하였습니까?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_R_Left Pass");
					Pass(4, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_R_Left Fail");
					Pass(4, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }

			}
			else
			{
				Log("STEER_R_Left Fail");
				Pass(4, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;
		 // (28) Cid Check Complete
		 case Sim_Step_ST::SteeringCheckComplete:
		 {
			Log("(5) Steering Check Complete");
			Pass(5, true);
			OACISDataSendTP2OHT(CATEGORY_STEER,1,1);
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step_ST::None;
		 }
		 break;
	 }

	 if(iStatus != Sim_Step_ST::None)
		tmrSimulation->Enabled = true;
}


void __fastcall TUnitChecker::btnCloseClick(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_STOP);
	StatusMachineShift(Sim_Cmd_ST::TEST_STOP);
	StatusMachineHoist(Sim_Cmd_ST::TEST_STOP);
	StatusMachineEStop(Sim_Cmd_ST::TEST_STOP);
	this->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnStartClick(TObject *Sender)
{
		// 검사결과 클리어
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
	}
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Steering Checker를 진행하시겠습니까?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 5; i++)
			{
				strngrd1->Cells[2][i + 1] = "";
			}
			mmoMemo->Lines->Clear();
			StatusMachine(Sim_Cmd_ST::TEST_START);
			tmrSimulation->Enabled = true;
		}
		else
			return;
	}


}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnStopClick(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrSimulationTimer(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnSMPSCheckClick(TObject *Sender)
{
		if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
	}
	else
	{
    	OACISDataSendTP2OHT(CATEGORY_SMPSVOLTAGE,2,0);
	}

}
//---------------------------------------------------------------------------
void TUnitChecker::ShiftINCMove(int MoveDist,bool bLeft)
{
    uTP_CMD TPCmd; // TP Command
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR))
    {
        // by zzang9un 2013.09.09 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command 전송
            // 가/감속시간, 속도는 Main에 고정되어 있으므로 거리값만 전송
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			if(bLeft)
			{
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_LEFT_INC, &TPCmd);
            	Log("JOG_SHIFT_LEFT_INC:",1);
			}
			else
			{
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_RIGHT_INC, &TPCmd);
				Log("JOG_SHIFT_RIGHT_INC:",1);
            }
            delete m_CMDControl;
		}
	}
}

void TUnitChecker::HoistINCMove(int MoveDist)
{


    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
    {
        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command 전송
            // 가/감속시간, 속도는 Main에 고정되어 있으므로 거리값만 전송
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DOWN_INC, &TPCmd);
			Log("JOG_HOIST_DOWN_INC:",2);
            delete m_CMDControl;
        }
    }
}


void __fastcall TUnitChecker::btnShiftHomeStopClick(TObject *Sender)
{
	StatusMachineShift(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrSimulationShiftTimer(TObject *Sender)
{
	StatusMachineShift(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnShiftHomeStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.",1);
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.",1);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Shift Home Sensor is Not ON. Please Retry After Shift Home");
		Log("Shift Home Sensor is Not ON. Please Retry After Shift Home",1);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Hoist Home Sensor is Not ON. Please Retry After Hoist Home");
		Log("Hoist Home Sensor is Not ON. Please Retry After Hoist Home",1);
		return;
	}
	else if((gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Close == ON))
	{
		MainForm->Warnning_Display(clRed, "Hand Foup Exist. Please Retry After Remove Foup");
		Log("Hand Foup Exist. Please Retry After Remove Foup",1);
		return;
	}
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Shift Home Checker를 진행하시겠습니까?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 3; i++)
			{
				strngrdShiftHome->Cells[2][i + 1] = "";
			}
			mmoShiftHome->Lines->Clear();
			StatusMachineShift(Sim_Cmd_ST::TEST_START);
			tmrSimulationShift->Enabled = true;
		}
		else
			return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnHoistStopClick(TObject *Sender)
{
	StatusMachineHoist(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSimulationHoistTimer(TObject *Sender)
{
	StatusMachineHoist(Sim_Cmd_ST::TEST_GOING);
}

void TUnitChecker::StatusMachineHoist(int iCmd)
{
  	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 int iRet;
	 static int nCountCheck = 0;

     AnsiString strWanted_Result;

	 // 커맨드 처리
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Hoist::StartHoistMove;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulationHoist->Enabled = false;
			iStatus = Sim_Step_Hoist::None;
			return;
		}
		break;
	 }

	 tmrSimulationHoist->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_Hoist::None: 	// (0) Not work
		 {
			;
		 }
		 break;


		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step_Hoist::StartHoistMove:
		 {
			 Log("Shift Check Home");
			 iStatus++;
		 }
		 break;

		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step_Hoist::StartHoistDown:
		 {
			 HoistINCMove(40);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Hoist::StartHoistDown_Confirm:
		 {
			if(gm_OHTStatus->Motor_Hoist->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == OFF)
				{
					
					Log("Hoist Down & Home Sensor Off OK",2);
					MaxDistShiftHoist(1,int(gm_OHTStatus->Motor_Hoist->dPosition_mm*10),false,true);
					iStatus++;

				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 15.0)
					{
						MaxDistShiftHoist(1,2,true,true);
						Log("Hoist Down Fail!",2);
						iStatus = Sim_Step_Hoist::None;
						tmrSimulationHoist->Enabled = false;
					}else
					{
						HoistINCMove(3);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(1,2,true,true);
					Log("Hoist Down Fail!",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
				}
            }

		 }
		 break;




		  case Sim_Step_Hoist::StartHoistHome:
		 {
			 TechOHTCheck.Axis_Hoist_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Hoist::StartHoistHome_Confirm:
		 {
			if(gm_OHTStatus->Motor_Hoist->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) < 1.0)
					{
						Log("Hoist Home OK",2);
						iStatus++;
					}else
					{
						Log("Hoist Home Fail",2);
						iStatus = Sim_Step_Hoist::None;
						tmrSimulationHoist->Enabled = false;
					}
				}
				else
				{
					Log("Hoist Home Fail",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Hoist Home Fail",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
                }
            }

		 }
		 break;

		
		 case Sim_Step_Hoist::HoistCheckComplete:
		 {
			Log("HoistCheck Complete",2);


			int nOK =0;
			if(fabs(StrToInt(strngrdHoist->Cells[2][1]) - StrToInt(edtHoistStandard->Text)) < 10)
			{
				nOK = 1;
			}
			else
			{
				nOK = 2;
            }

			MaxDistShiftHoist(2,nOK,true,true);

			OACISDataSendTP2OHT(CATEGORY_HOISTHOME,0,nOK,StrToInt(strngrdHoist->Cells[2][1]));
			tmrSimulationHoist->Enabled = false;
			iStatus = Sim_Step_Hoist::None;
		 }
		 break;

	 }

	 if(iStatus != Sim_Step_Hoist::None)
		tmrSimulationHoist->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnHoistStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.",2);
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.",2);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Shift Home Sensor is Not ON. Please Retry After Shift Home");
		Log("Shift Home Sensor is Not ON. Please Retry After Shift Home",2);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Hoist Home Sensor is Not ON. Please Retry After Hoist Home");
		Log("Hoist Home Sensor is Not ON. Please Retry After Hoist Home",2);
		return;
	}
	else if((gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Close == ON))
	{
		MainForm->Warnning_Display(clRed, "Hand Foup Exist. Please Retry After Remove Foup");
		Log("Hand Foup Exist. Please Retry After Remove Foup",2);
		return;
	}
	else if(((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  != OHT_TYPE_FOUP) &&(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  != OHT_TYPE_FOUP_S)) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver != OHT_VERSION_81))
	{
		MainForm->Warnning_Display(clRed, "OHT is Not FoupType/V8.1.");
		Log("OHT is Not FoupType/V8.1",2);
		return;
    }
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Hoist Home Checker를 진행하시겠습니까?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 2; i++)
			{
				strngrdHoist->Cells[2][i + 1] = "";
			}
			mmoHoist->Lines->Clear();
			StatusMachineHoist(Sim_Cmd_ST::TEST_START);
			tmrSimulationHoist->Enabled = true;
		}
		else
			return;
	}
}
//---------------------------------------------------------------------------
/*
■ UNITCHECKER 4) E-STOP & Bumper Press
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEStop(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_SensorCheck::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEStopDone->Caption = "진행중";
			shpEStopDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblEStopDone->Caption = "중지";
			shpEStopDone->Brush->Color	=  COLOR_SILVER;
			tmrEStop->Enabled = false;
			iStatus = Sim_Step_SensorCheck::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrEStop->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_SensorCheck::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_SensorCheck::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS 확인
		case Sim_Step_SensorCheck::CheckPressESTOP:
		{
			Log("E-STOP 을 눌러주세요.",Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.EMO_Press == false)// || chkTest->Checked == true)
			{
				PassOrNot(strngrdEStop1, 1, true);
				Log("E-STOP PRESS ON 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 1, false);
					String strMsg = "감지 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "실패";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS 해제 확인
		case Sim_Step_SensorCheck::CheckResetESTOP:
		{
			Log("E-STOP 을 해제하고 Error 를 Reset해주세요.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.EMO_Press == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEStop1, 2, true);
				Log("E-STOP PRESS OFF 및 Error Rest 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 2, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "실패";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) BUMPER PERESS 확인
		case Sim_Step_SensorCheck::CheckPressBumper:
		{
			Log("Bumper를 눌러주세요.",Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == true)// || chkTest->Checked == true)
			{
				PassOrNot(strngrdEStop1, 3, true);
				Log("BUMPER PRESS ON 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 3, false);
					String strMsg = "1분 이상 경과되었습니다. 검사를 종료합니다.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "실패";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (5) BUMPER PRESS 해제 확인
		case Sim_Step_SensorCheck::CheckResetBumper:
		{
			Log("Error 를 Reset해주세요.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEStop1, 4, true);
				Log("Error Reset 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 4, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "실패";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_SensorCheck::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_ESTOPBUMPER,1,1);

			tmrEStop->Enabled = false;
			iStatus = Sim_Step_SensorCheck::None;

			lblEStopDone->Caption = "완료";
			shpEStopDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_SensorCheck::None)
		tmrEStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEStopStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if((gm_OHTStatus->IOInput->uInputCommon.EMO_Press == false) ||
			(gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == true))
	{
		MainForm->Warnning_Display(clRed, "E-STOP 또는 BUMPERPRESS가 감지됩니다. 해제후 재시도하세요");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" E-STOP, BUMPER PRESS 체크를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineEStop(Sim_Cmd_ST::TEST_START);
		tmrEStop->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::tmrEStopTimer(TObject *Sender)
{
	StatusMachineEStop(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEStopEndCick(TObject *Sender)
{
	StatusMachineEStop(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
/*
■ UNITCHECKER 5)STB Detect
*/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
■ UNITCHECKER 6)Oscilation Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineOscilation(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static DWORD dwDetectSensor;
	static double dwDetectTime;
	static int	nErrorCount = 0;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_Oscilation::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblOscilationDone->Caption = "진행중";
			shpOscilationDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblOscilationDone->Caption = "중지";
			shpOscilationDone->Brush->Color	=  COLOR_SILVER;
			tmrOscilation->Enabled = false;
			iStatus = Sim_Step_Oscilation::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrOscilation->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Oscilation::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_Oscilation::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) Hoist 하강 확인
		case Sim_Step_Oscilation::CheckHoistDown:
		{
			Log("Oscil On이 되도록 이적재 명령을 인가합니다.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == true) && (gm_OHTStatus->Motor_Hoist->dPosition_mm > 300))
			{
				PassOrNot(strngrdOscilation, 1, true);
				Log("Hoist 300mm 이상 하강 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdOscilation, 1, false);
					String strMsg = "Hoist 위치 인가 후 5분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "실패";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation 감지 확인
		case Sim_Step_Oscilation::CheckOscilation:
		{
			if(nErrorCount > 5)
			{
				PassOrNot(strngrdOscilation, 3, false);
				MainForm->Warnning_Display(clRed, "Oscilation Error 가 발생하지 않았습니다. 확인 후 재진행해주세요.");
				//OACIS로 실패 내용 전달
				//변경변경 Fail 처리
				iStatus = Sim_Step_Oscilation::None;
				tmrOscilation->Enabled = false;
				lblOscilationDone->Caption = "실패";
				shpOscilationDone->Brush->Color	=  COLOR_RED;
				Log("",Log_Step_Init);
				LastTime(0, 0, false);
				tmpTesting = 0;
				break;
			}

			Log("Hand 동작 중 Oscilation Sensor를 감지시켜주세요(최소 60s이상)",Log_Step_Check);

			//21.03.27 JHJ False 인지 True 인지 확인 필요
			if((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == false) && (gm_OHTStatus->Motor_Hoist->dVelocity_mpersec < 0.1))
			{
				dwDetectSensor = timeGetTime();
				PassOrNot(strngrdOscilation, 2, true);
				Log("Oscilation 감지 및 정지 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdOscilation, 2, false);
					String strMsg = "감지 없이 5분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "실패";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) Oscilation Error 확인
		case Sim_Step_Oscilation::CheckError:
		{
			dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
			String strLog;
			strLog.sprintf(L"감지 상태로 유지해주세요 : %d초",(int)(dwTmpTime/1000));
			Log(strLog,Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == false)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 944) //ERR_OSCILLATION_ERROR
				{
					String tmp;
					dwDetectTime = PreventOverflowGetTimeMs(dwDetectSensor)*0.001;
					tmp.sprintf(L"감지~Err 발생 시간 : %.2f초",dwDetectTime);
					lblOscilationTime->Caption = tmp;//String(L"감지~Err 발생 시간 : %.2f초",PreventOverflowGetTimeMs(dwDetectTime));
					PassOrNot(strngrdOscilation, 3, true);
					Log("Oscilation Error 확인 완료",Log_Step_Done);

					LastTime(0, 0, false);
					dwSensorCheck = timeGetTime();
					iStatus++;
				}
				else
				{
					LastTime(dwTmpTime, CheckTime_1m, true);
					if(dwTmpTime > CheckTime_1m)
					{
						PassOrNot(strngrdOscilation, 3, false);
						String strMsg = "1분 이상 Error 가 발생하지 않았습니다. 검사를 종료합니다.";
						if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
						{
							//OACIS로 실패 내용 전달
							//변경변경 Fail 처리
							iStatus = Sim_Step_Oscilation::None;
							tmrOscilation->Enabled = false;
							lblOscilationDone->Caption = "실패";
							shpOscilationDone->Brush->Color	=  COLOR_RED;
							Log("",Log_Step_Init);
							LastTime(0, 0, false);
							tmpTesting = 0;
						}

					}
				}
			}
			else
			{
				nErrorCount++;
				iStatus = Sim_Step_Oscilation::CheckOscilation;
			}
		}
		break;

// === (5) Error 해제 확인
		case Sim_Step_Oscilation::CheckResetError:
		{
			Log("Sensor 가림막을 치우고 Error 를 Reset해주세요.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdOscilation, 4, true);
				Log("Error Reset 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdOscilation, 4, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "실패";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_Oscilation::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_OSCILATION,1,1);

			tmrOscilation->Enabled = false;
			iStatus = Sim_Step_Oscilation::None;

			lblOscilationDone->Caption = "완료";
			shpOscilationDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Oscilation::None)
		tmrOscilation->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnOscilationStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" Oscilation Check를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineOscilation(Sim_Cmd_ST::TEST_START);
		tmrOscilation->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::btnOsclationEndClick(TObject *Sender)
{
	StatusMachineOscilation(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrOscilationTimer(TObject *Sender)
{
	StatusMachineOscilation(Sim_Cmd_ST::TEST_GOING);
}


//---------------------------------------------------------------------------
/*
■ UNITCHECKER 7)LookDown Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineLookDown(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static DWORD dwDetectSensor;
	static double dwDetectTime;
	static int	nErrorCount = 0;
	static bool	bSendTPCmd = false;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			bSendTPCmd = false;
			iStatus = Sim_Step_LookDown::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblLookDownDone->Caption = "진행중";
			shpLookDownDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblLookDownDone->Caption = "중지";
			shpLookDownDone->Brush->Color	=  COLOR_SILVER;
			tmrLookDown->Enabled = false;
			iStatus = Sim_Step_LookDown::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrLookDown->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_LookDown::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_LookDown::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2)LookDown Area Setting
		case Sim_Step_LookDown::CheckAreaSelect:
		{
			Log("LookDown Area Setting 진행 중",Log_Step_Check);

			if(bSendTPCmd == false)
			{
            	bSendTPCmd = true;
				TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
				m_CMDControl->MakeTPCmd2OHT(TPCMD_LOOKDOWN_AREA1);      //■이거동작안한다
				delete m_CMDControl;
			}
			int nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel << 1)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel << 2)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel << 3);
			nValue = ~nValue & 0xF;
			if(((nValue == 0x01) && (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error  == false))
			)// || (chkTest->Checked == true))
			{
				PassOrNot(strngrdLookDown, 1, true);
				Log("LookDown Area Setting 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 1, false);
					String strMsg = "LookDown Area Setting 명령 인가 후 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "실패";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation 감지 확인
		case Sim_Step_LookDown::CheckLookDown:
		{
			if(nErrorCount > 5)
			{
				PassOrNot(strngrdLookDown, 3, false);
				MainForm->Warnning_Display(clRed, "LookDown Error 가 발생하지 않았습니다. 확인 후 재진행해주세요.");
				//OACIS로 실패 내용 전달
				//변경변경 Fail 처리
				iStatus = Sim_Step_LookDown::None;
				tmrLookDown->Enabled = false;
				lblLookDownDone->Caption = "실패";
				shpLookDownDone->Brush->Color	=  COLOR_RED;
				Log("",Log_Step_Init);
				LastTime(0, 0, false);
				tmpTesting = 0;
				break;
			}

			if(chkRailTest->Checked == true)
				Log("이적재를 시작한 뒤 LookDown Sensor를 감지시켜주세요(최소 10s이상)",Log_Step_Check);
			else
				Log("LookDown Sensor 를 감지시켜주세요.",Log_Step_Check);

			//21.03.27 JHJ False 인지 True 인지 확인 필요
			if(((gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == true) && (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error == false))
			)// || (chkTest->Checked == false))
			{
				dwDetectSensor = timeGetTime();
				PassOrNot(strngrdLookDown, 2, true);
				Log("LookDown 감지 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;

				if(chkRailTest->Checked == true)
					iStatus++;
				else
				{
					iStatus = Sim_Step_LookDown::SensorCheckComplete;
				}
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 2, false);
					String strMsg = "감지 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "실패";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) Oscilation Error 확인
		case Sim_Step_LookDown::CheckError:
		{
			dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
			String strLog;
			strLog.sprintf(L"감지 상태로 유지해주세요 : %d초",(int)(dwTmpTime/1000));
			Log(strLog,Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == true )//|| chkTest->Checked == true)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 941)// || chkTest->Checked == true) //ERR_LOOKDOWN_DETECT
				{
					String tmp;
					dwDetectTime = PreventOverflowGetTimeMs(dwDetectSensor)*0.001;
					tmp.sprintf(L"감지~Err 발생 시간 : %.2f초",dwDetectTime);
					lblLookDownTime->Caption = tmp;

					PassOrNot(strngrdLookDown, 3, true);
					Log("LookDown Error 확인 완료",Log_Step_Done);

					LastTime(0, 0, false);
					dwSensorCheck = timeGetTime();
					iStatus++;
				}
				else
				{
					LastTime(dwTmpTime, CheckTime_1m, true);
					if(dwTmpTime > CheckTime_1m)
					{
						PassOrNot(strngrdLookDown, 3, false);
						String strMsg = "1분 이상 Error 가 발생하지 않았습니다. 검사를 종료합니다.";
						if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
						{
							//OACIS로 실패 내용 전달
							//변경변경 Fail 처리
							iStatus = Sim_Step_LookDown::None;
							tmrLookDown->Enabled = false;
							lblLookDownDone->Caption = "실패";
							shpLookDownDone->Brush->Color	=  COLOR_RED;
							Log("",Log_Step_Init);
							LastTime(0, 0, false);
							tmpTesting = 0;
						}

					}
				}
			}
			else
			{
				nErrorCount++;
				iStatus = Sim_Step_LookDown::CheckLookDown;
			}
		}
		break;

// === (5) Error 해제 확인
		case Sim_Step_LookDown::CheckResetError:
		{
			Log("Sensor 가림막을 치우고 Error 를 Reset해주세요.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdLookDown, 4, true);
				Log("Error Reset 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 4, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "실패";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_LookDown::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_LOOKDOWN,1,1);

			tmrLookDown->Enabled = false;
			iStatus = Sim_Step_LookDown::None;

			lblLookDownDone->Caption = "완료";
			shpLookDownDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_LookDown::None)
		tmrLookDown->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnLookDownStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error  == true)
	{
		MainForm->Warnning_Display(clRed, "LookDown Sensor 점검 후 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}
	if(gm_OHTStatus->CheckOnOff->bUse_LookDownSensor == false)
	{
		MainForm->Warnning_Display(clRed, "LookDown 미사용 상태입니다. Use LookDown을 True로 변경해주세요");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" LookDown Check를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineLookDown(Sim_Cmd_ST::TEST_START);
		tmrLookDown->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnLookDownEndClick(TObject *Sender)
{
	StatusMachineLookDown(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrLookDownTimer(TObject *Sender)
{
	StatusMachineLookDown(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
/*
■ UNITCHECKER 8)STB Detect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineSTBNo(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_STBTrans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblSTBNoDone->Caption = "진행중";
			shpSTBNoDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 1)
			{
				tmpTesting = 0;

				lblSTBNoDone->Caption = "중지";
				shpSTBNoDone->Brush->Color	=  COLOR_SILVER;
				tmrSTBNo->Enabled = false;
				iStatus = Sim_Step_STBTrans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrSTBNo->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_STBTrans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_STBTrans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;
// === (3) E-STOP PRESS 해제 확인
		case Sim_Step_STBTrans::CheckError:
		{
			Log("Unload 명령을 인가해주세요.",Log_Step_Check);

			if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == false && gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 851)
			 || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == false && gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 851))//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdSTBDetect, 1, true);
				Log("Error 발생 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdSTBDetect, 1, false);
					String strMsg = "감지 및 Error 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_STBTrans::None;
						tmrSTBNo->Enabled = false;
						lblSTBNoDone->Caption = "실패";
						shpSTBNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_STBTrans::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_STBDETECT,1,1);

			tmrSTBNo->Enabled = false;
			iStatus = Sim_Step_STBTrans::None;

			lblSTBNoDone->Caption = "완료";
			shpSTBNoDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_STBTrans::None)
		tmrSTBNo->Enabled = true;
}
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineSTBDouble(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_STBTrans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblSTBDoubleDone->Caption = "진행중";
			shpSTBDoubleDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 2)
			{
				tmpTesting = 0;

				lblSTBDoubleDone->Caption = "중지";
				shpSTBDoubleDone->Brush->Color	=  COLOR_SILVER;
				tmrSTBDouble->Enabled = false;
				iStatus = Sim_Step_STBTrans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrSTBDouble->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_STBTrans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_STBTrans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS 확인
		case Sim_Step_STBTrans::CheckError:
		{
			Log("Load 명령을 인가해주세요.",Log_Step_Check);

			if(((chkSTBDirection->Checked == false) && (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 850))//ERR_STB_FOUP_CHECK_NOEXIST))
			|| ((chkSTBDirection->Checked == true) && (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 850)))//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdSTBDetect, 1, true);
				Log("Error 발생 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdSTBDetect, 1, false);
					String strMsg = "이적재 동작 또는 공출하 없이 5분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_STBTrans::None;
						tmrSTBDouble->Enabled = false;
						lblSTBDoubleDone->Caption = "실패";
						shpSTBDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_STBTrans::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_STBDETECT,1,2);

			tmrSTBDouble->Enabled = false;
			iStatus = Sim_Step_STBTrans::None;

			lblSTBDoubleDone->Caption = "완료";
			shpSTBDoubleDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_STBTrans::None)
		tmrSTBDouble->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBNoCarrierClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "현재 이적재 위치가 아닙니다. 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT에 재하가 존재합니다. 재하를 빼고 재시도하세요.");
		return;
	}

	if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == true)
	  || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == true))
	{
		MainForm->Warnning_Display(COLOR_RED, "STB에 재하가 존재합니다. 재하를 빼고 재시도하세요.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" STB 공출하 TEST를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 1;
		StatusMachineSTBNo(Sim_Cmd_ST::TEST_START);
		tmrSTBNo->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBDoubleCarrierClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "현재 이적재 위치가 아닙니다. 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT에 재하가 존재하지 않습니다. 재하를 들고  재시도하세요.");
		return;
	}

	if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == false)
	  || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == false))
	{
		MainForm->Warnning_Display(COLOR_RED, "STB에 재하가 없습니다. 재하를 넣고 재시도하세요.");
		return;
	}


	String strMsg = NULL;
	strMsg.sprintf(L" STB이중적재 TEST를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 2;
		StatusMachineSTBDouble(Sim_Cmd_ST::TEST_START);
		tmrSTBDouble->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBDetectEndClick(TObject *Sender)
{
	StatusMachineSTBNo(Sim_Cmd_ST::TEST_STOP);
	StatusMachineSTBDouble(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSTBNoTimer(TObject *Sender)
{
	StatusMachineSTBNo(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSTBDoubleTimer(TObject *Sender)
{
	StatusMachineSTBDouble(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
■ UNITCHECKER 8)EQ Detect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEQNo(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Trans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEQNoDone->Caption = "진행중";
			shpEQNoDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 1)
			{
				tmpTesting = 0;

				lblEQNoDone->Caption = "중지";
				shpEQNoDone->Brush->Color	=  COLOR_SILVER;
				tmrEQNo->Enabled = false;
				iStatus = Sim_Step_Trans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrEQNo->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Trans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_Trans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS 확인
		case Sim_Step_Trans::CheckLoadUnloadCmd:
		{
			Log("Unload 명령을 인가해주세요.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_UNLOADING )//|| chkTest->Checked == true)
			{
				PassOrNot(strngrdEQ, 1, true);
				Log("이적재 명령 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdEQ, 1, false);
					String strMsg = "이적재 동작 없이 5분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Trans::None;
						tmrEQNo->Enabled = false;
						lblEQNoDone->Caption = "실패";
						shpEQNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS 해제 확인
		case Sim_Step_Trans::CheckError:
		{
			Log("Error 발생 여부를 확인합니다.",Log_Step_Check);

			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEQ, 2, true);
				Log("Error 발생 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEQ, 2, false);
					String strMsg = "감지 및 Error 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Trans::None;
						tmrEQNo->Enabled = false;
						lblEQNoDone->Caption = "실패";
						shpEQNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_Trans::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_EQPORT,1,1);

			tmrEQNo->Enabled = false;
			iStatus = Sim_Step_Trans::None;

			lblEQNoDone->Caption = "완료";
			shpEQNoDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Trans::None)
		tmrEQNo->Enabled = true;
}
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEQDouble(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Trans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEQDoubleDone->Caption = "진행중";
			shpEQDoubleDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 2)
			{
				tmpTesting = 0;

				lblEQDoubleDone->Caption = "중지";
				shpEQDoubleDone->Brush->Color	=  COLOR_SILVER;
				tmrEQDouble->Enabled = false;
				iStatus = Sim_Step_Trans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrEQDouble->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Trans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_Trans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS 확인
		case Sim_Step_Trans::CheckLoadUnloadCmd:
		{
			Log("Load 명령을 인가해주세요.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_LOADING )//|| chkTest->Checked == true)
			{
				PassOrNot(strngrdEQ, 1, true);
				Log("이적재 명령 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdEQ, 1, false);
					String strMsg = "이적재 동작 없이 5분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Trans::None;
						tmrEQDouble->Enabled = false;
						lblEQDoubleDone->Caption = "실패";
						shpEQDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS 해제 확인
		case Sim_Step_Trans::CheckError:
		{
			Log("Error 발생 여부를 확인합니다.",Log_Step_Check);

			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEQ, 2, true);
				Log("Error 발생 확인 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEQ, 2, false);
					String strMsg = "감지 및 Error 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_Trans::None;
						tmrEQDouble->Enabled = false;
						lblEQDoubleDone->Caption = "실패";
						shpEQDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_Trans::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_EQPORT,1,2);

			tmrSTBDouble->Enabled = false;
			iStatus = Sim_Step_Trans::None;

			lblEQDoubleDone->Caption = "완료";
			shpEQDoubleDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Trans::None)
		tmrEQDouble->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnEQNoClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "현재 이적재 위치가 아닙니다. 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT에 재하가 존재합니다. 재하를 빼고 재시도하세요.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" EQ 공출하 TEST를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 1;
		StatusMachineEQNo(Sim_Cmd_ST::TEST_START);
		tmrEQNo->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::btnEQDoneClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "현재 이적재 위치가 아닙니다. 재시도하세요.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT에 재하가 존재하지 않습니다. 재하를 들고  재시도하세요.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" EQ 이중적재 TEST를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 2;
		StatusMachineEQDouble(Sim_Cmd_ST::TEST_START);
		tmrEQDouble->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEQStopClick(TObject *Sender)
{
	StatusMachineEQNo(Sim_Cmd_ST::TEST_STOP);
	StatusMachineEQDouble(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrEQNoTimer(TObject *Sender)
{
	StatusMachineEQNo(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrEQDoubleTimer(TObject *Sender)
{
	StatusMachineEQDouble(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
■ UNITCHECKER 9)FoupDetect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineFoupDetect(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static int	nErrorCount = 0;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_FoupDetect::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblFoupDetectDone->Caption = "진행중";
			shpFoupDetectDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblFoupDetectDone->Caption = "중지";
			shpFoupDetectDone->Brush->Color	=  COLOR_SILVER;
			tmrFoupDetect->Enabled = false;
			iStatus = Sim_Step_FoupDetect::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrFoupDetect->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_FoupDetect::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_FoupDetect::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

		case Sim_Step_FoupDetect::CheckFoupDetect:
		{
			Log("Foup Detect Sensor 를 가려주세요.",Log_Step_Check);
			//21.03.27 JHJ False 인지 True 인지 확인 필요
			if(((gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true))//1230))
			)// || (chkTest->Checked == false))
			{
				PassOrNot(strngrdFoupDetect, 1, true);
				Log("FoupDetect 감지  완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupDetect, 1, false);
					String strMsg = "감지 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "실패";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (2) Hoist 하강 확인
		case Sim_Step_FoupDetect::CheckUnload:
		{
			Log("감지한 상태로 Unload 명령을 인가합니다.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 858)
			{
				PassOrNot(strngrdFoupDetect, 2, true);
				Log("Error 발생 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdFoupDetect, 2, false);
					String strMsg = "5분 이상 Error가 발생하지 않았습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "실패";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation 감지 확인
// === (5) Error 해제 확인
		case Sim_Step_FoupDetect::CheckResetError:
		{
			Log("Sensor 가림막을 치우고 Error 를 Reset해주세요.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			{
				PassOrNot(strngrdFoupDetect, 3, true);
				Log("Error Reset 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupDetect, 3, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "실패";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_FoupDetect::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_FOUPDETECT,1,1);

			tmrFoupDetect->Enabled = false;
			iStatus = Sim_Step_FoupDetect::None;

			lblFoupDetectDone->Caption = "완료";
			shpFoupDetectDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_FoupDetect::None)
		tmrFoupDetect->Enabled = true;
}
void __fastcall TUnitChecker::btnFoupDetectStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}
	//풉을 들고 있습니다.


	String strMsg = NULL;
	strMsg.sprintf(L" FoupDetect Check를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineFoupDetect(Sim_Cmd_ST::TEST_START);
		tmrFoupDetect->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrFoupDetectTimer(TObject *Sender)
{
	StatusMachineFoupDetect(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnFoupDetectStopClick(TObject *Sender)
{
	StatusMachineFoupDetect(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrFoupCoverTimer(TObject *Sender)
{
	StatusMachineFoupCover(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
■ UNITCHECKER 9)FoupDetect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineFoupCover(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static int	nErrorCount = 0;

	 // 커맨드 처리
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_FoupCover::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblFoupCoverDone->Caption = "진행중";
			shpFoupCoverDone->Brush->Color	=  COLOR_YELLOW;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblFoupCoverDone->Caption = "중지";
			shpFoupCoverDone->Brush->Color	=  COLOR_SILVER;
			tmrFoupCover->Enabled = false;
			iStatus = Sim_Step_FoupCover::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrFoupCover->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_FoupCover::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check 시작
		case Sim_Step_FoupCover::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

		case Sim_Step_FoupCover::CheckFoupCover:
		{
			Log("Foup Cover Detect Sensor 를 가려주세요.",Log_Step_Check);
			//21.03.27 JHJ False 인지 True 인지 확인 필요
			if((gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 1230))
			{
				PassOrNot(strngrdFoupCover, 1, true);
				Log("Error 발생 완료",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdFoupCover, 1, false);
					String strMsg = "5분 이상 Error가 발생하지 않았습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_FoupCover::None;
						tmrFoupCover->Enabled = false;
						lblFoupCoverDone->Caption = "실패";
						shpFoupCoverDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation 감지 확인
// === (5) Error 해제 확인
		case Sim_Step_FoupCover::CheckResetError:
		{
			Log("Sensor 가림막을 치우고 Error 를 Reset해주세요.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			{
				PassOrNot(strngrdFoupCover, 2, true);
				Log("Error Reset 확인 완료",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupCover, 2, false);
					String strMsg = "해제 없이 1분 이상 경과되었습니다. 확인 후 재시도해주세요.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS로 실패 내용 전달
						//변경변경 Fail 처리
						iStatus = Sim_Step_FoupCover::None;
						tmrFoupCover->Enabled = false;
						lblFoupCoverDone->Caption = "실패";
						shpFoupCoverDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK 완료 확인
		case Sim_Step_FoupCover::SensorCheckComplete:
		{
			//OACIS로 성공 내용 전달
			OACISDataSendTP2OHT(CATEGORY_FOUPCOVER,1,1);

			tmrFoupCover->Enabled = false;
			iStatus = Sim_Step_FoupCover::None;

			lblFoupCoverDone->Caption = "완료";
			shpFoupCoverDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_FoupCover::None)
		tmrFoupCover->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnFoupCoverStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "현재 다른 TEST가 진행중입니다. 완료 후 재시도하세요. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error 가 발생했습니다. 해제후 재시도하세요");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" FOUP Cover Detect Check를 시작하시겠습니까?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineFoupCover(Sim_Cmd_ST::TEST_START);
		tmrFoupCover->Enabled = true;
	}
	else
		return;         //전역번수 사용 가능 여부 확인 필요
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnFoupCoverStopClick(TObject *Sender)
{
	StatusMachineFoupCover(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnUBGOBSClick(TObject *Sender)
{
	OBS_UBG_Form->Show();
	OBS_UBG_Form->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnCIDCheckClick(TObject *Sender)
{
	frmCidChecker->Show();
	frmCidChecker->BringToFront();
}
//---------------------------------------------------------------------------


