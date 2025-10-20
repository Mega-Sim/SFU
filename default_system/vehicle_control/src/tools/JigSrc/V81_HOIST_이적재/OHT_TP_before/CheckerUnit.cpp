//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckerUnit.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "OHTInfoUnit.h"
#include "ChkExecUnit.h"

TCHKOHT TechOHTCheck;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
UnicodeString strTitle[5] =
{
	// 1
	L"Steering Front Right",
	L"Steering Rear Right",
	L"Steering Fornt Left",
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

	for(int i = 0; i < 3; i++)
	{
		strngrdShiftHome->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdShiftHome->Cells[1][i + 1] = strTitleShift[i];
	}

	strngrdShiftHome->Cells[0][0] = L"순서";
	strngrdShiftHome->Cells[1][0] = L"검사내용";
	strngrdShiftHome->Cells[2][0] = L"검사결과";

	for(int i = 0; i < 2; i++)
	{
		strngrdHoist->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdHoist->Cells[1][i + 1] = strTitleHoist[i];
	}

	strngrdHoist->Cells[0][0] = L"순서";
	strngrdHoist->Cells[1][0] = L"검사내용";
	strngrdHoist->Cells[2][0] = L"검사결과";

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
void TUnitChecker::Log(UnicodeString strLog, int Type)
{
	if(Type ==0)
		mmoMemo->Lines->Add(strLog);
	else if(Type ==1)
		mmoShiftHome->Lines->Add(strLog);
	else if(Type ==2)
        mmoHoist->Lines->Add(strLog);
}

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

