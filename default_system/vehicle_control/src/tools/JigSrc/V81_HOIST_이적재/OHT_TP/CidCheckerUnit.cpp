//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CidCheckerUnit.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "OHTInfoUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCidChecker *frmCidChecker;

/*
	테스트 벤치로 가지 않고, OHT만으로 체크하는 기능도 필요
*/
#define IN_TEST_BENCH	0	// 벤치면 1, OHT면 0로 설정

// 동작 데이터
#define DEF_AUTO				   	0
#define DEF_1_BATT_ON_MOVE  		1	// 배터리를 연결하고 테스트벤치로 이동
#define DEF_2_BATT_CUT_IN_BENCH		2	// 테스트 벤치 이동후 베터리 연결 끊음
#define DEF_4_LEFT_SELECT_ON		4   // Left Select check
#define DEF_19_RIGHT_SELECT_ON 		19   // Right Select check
#define DEF_PROCESS_END				27	// 검사 스텝완료  (총 28개이므로 0~27까지 진행)

// 행의 동작
// #define RAW_JOB_ORDER  	0	// 조작자에게 보내는 지시문
// #define RAW_ACTION		1   // 프로그램 내의 식별자
// #define RAW_TITLE		2  	// 항목의 제목 (목적)

// 스트링 데이터
UnicodeString strTitle[28] =
{
	// 1
	L"VHL Output I/O O9(EXT_PWR) 확인 (1)",
	L"VHL Output I/O O9(EXT_PWR) 확인 (2)",
	L"O6(Operating Mode) On",
	L"VHL Input I/O I1(Link) 확인",
	L"VHL Input I/O I2(Interlock) 확인 (1)",

	// 6
	L"VHL Input I/O I2(Interlock) 확인 (2)",
	L"VHL Input I/O I3(통과완료 Ack) 확인 (1)",
	L"VHL Input I/O I3(통과완료 Ack) 확인 (1)",
	L"VHL Input I/O I4(Status) 확인",
	L"VHL Input I/O I4(Status) 확인",

	// 11
	L"VHL Input I/O I1(Link) 확인",
	L"각각의 송신 세기 값 230 이상",
	L"Common 출력 체크",
	L"Common 출력 체크",
	L"VHL Input I/O I1(Link) 확인",

	// 16
	L"O1(Left Select) & O6(Operating Mode) Off",
	L"VHL Input I/O I1(Link) 확인",
	L"O6(Operating Mode) On확인",
	L"VHL Input I/O I1(Link) 확인",
	L"VHL Input I/O I2(Interlock) 확인",

	// 21
	L"VHL Input I/O I2(Interlock) 확인",
	L"VHL Input I/O I3(통과완료 Ack) 확인",
	L"VHL Input I/O I3(통과완료 Ack) 확인",
	L"VHL Input I/O I4(Status) 확인",
	L"VHL Input I/O I4(Status) 확인",

	// 26
	L"VHL Input I/O I1(Link) 확인",
	L"각각의 송신 세기 값 230 이상",
	L"검사 종료",
};


int iChkAction[28] =
{
	// 01 - 10
	DEF_1_BATT_ON_MOVE,		DEF_2_BATT_CUT_IN_BENCH,	DEF_AUTO,			DEF_4_LEFT_SELECT_ON,		DEF_AUTO,
	DEF_AUTO,				DEF_AUTO,					DEF_AUTO,			DEF_AUTO,					DEF_AUTO,

	// 11 - 20
	DEF_AUTO,				DEF_AUTO,					DEF_AUTO,			DEF_AUTO,					DEF_AUTO,
	DEF_AUTO,				DEF_AUTO,					DEF_AUTO,			DEF_19_RIGHT_SELECT_ON,		DEF_AUTO,

	// 21 - 28
	DEF_AUTO,				DEF_AUTO,					DEF_AUTO,			DEF_AUTO,					DEF_AUTO,
	DEF_AUTO,				DEF_AUTO,					DEF_PROCESS_END,
};


UnicodeString strOperationOrder[28] =
{
	// 01 - 05
	L"VHL 외부베터리 연결 후 테스트 벤치로 이동하세요,",	
	L"VHL 테스트 벤치로 이동 후 외부 베터리 해제",			
	L"",													
	L"O1(Left Select) On을 육안으로 확인해 주십시오",		
	L"",													

	// 06 - 10
	L"",	L"",	L"",	L"",	L"",
	// 11 - 15
	L"",	L"",	L"",	L"",	L"",													

	// 16 - 20
	L"",	L"",	L"",													
	L"VHL의 Left CID-O-S Sensor에 빛이 들어 오는지 육안 확인, O2/O6 ON, O7 OFF확인",															
	L"",													

	// 21 - 25
	L"",	L"",	L"",	L"",	L"",
	// 26 - 28
	L"",	L"",	L"검사 종료",												
};


#define CID_ON	1
#define CID_OFF 0

#define CID_IO_DELAY	1000
//---------------------------------------------------------------------------
__fastcall TfrmCidChecker::TfrmCidChecker(TComponent* Owner)
	: TForm(Owner)
{
	for(int i = 0; i < 28; i++)
	{
		strGridCheckList->Cells[0][i + 1] = IntToStr(i + 1);
		strGridCheckList->Cells[1][i + 1] = strTitle[i];
	}

	strGridCheckList->Cells[0][0] = L"순서";
	strGridCheckList->Cells[1][0] = L"검사내용";
	strGridCheckList->Cells[2][0] = L"Pass/Fail";
}
//---------------------------------------------------------------------------
void __fastcall TfrmCidChecker::btnStartClick(TObject *Sender)
{
	// 검사결과 클리어
	for(int i = 0; i < 28; i++)
	{
		strGridCheckList->Cells[2][i + 1] = "";
	}

	mmInspectLog->Lines->Clear();
	StatusMachine(Sim_Cmd::TEST_START);
	tmrSimulation->Enabled = true;
}
//---------------------------------------------------------------------------
void TfrmCidChecker::StatusMachine(int iCmd)
{
	 static int iStatus = Sim_Cmd::TEST_NONE;
	 int iRet;

	 int iI1, iI2, iI3, iI4;
	 int iO1, iO2, iO3, iO4, iO5, iO6, iO7, iO8;

     AnsiString strWanted_Result;

	 // 커맨드 처리
	 switch(iCmd)
	 {
		case Sim_Cmd::TEST_START:
		{
			iStatus = Sim_Step::BatteryAndMove;
			return;
		}
		break;

		case Sim_Cmd::TEST_GOING:
		{
			;	// 아무것도 안하고 그대로 진행
		}
		break;

		case Sim_Cmd::TEST_STOP:
		{
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step::None;
			return;
		}
		break;
	 }

	 tmrSimulation->Enabled = false;	// 중복 실행방지

	 // Status Machine 진행
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step::None: 	// (0) Not work
		 {
			;
		 }
		 break;

		 // === (1) VHL 외부베터리를 연결
		 case Sim_Step::BatteryAndMove:
		 {
			Log("(1) VHL 외부베터리를 연결");
			ShowMessage("VHL 외부베터리를 연결하고, CID-O-S 양쪽이 깜빡이는지 확인하고, 테스트 벤치로 이동후 OK버튼을 눌러 주세요");

			iStatus++;
		 }
		 break;

		 case Sim_Step::BatteryAndMove_Confirm:
		 {
			iRet = MessageBox(Handle, L"CID-O-S 양쪽이 깜박거리고 있었습니까? (그림참고)",
							  L"확인해 주세요", MB_YESNO | MB_ICONQUESTION);
			if(iRet == IDYES)
			{ 	// 성공
				Log("Step02 - 양쪽 깜박임 육안확인 성공");
				Pass(1, true);
			}
			else
			{ 	// 실패
				Log("Step02 - 양쪽 깜박임 육안확인 실패");
				Log("한쪽의 CID-O-S 만 깜빡일 경우 CID-O와 CID-O-S 연결 확인 필요합니다");
				Pass(1, false);
			}

			iStatus++;
		 }
		 break;

		 // (2) 외부 베터리를 해제
		 case Sim_Step::BatteryCut:
		 {
			Log("(2) 외부 베터리를 해제");
			ShowMessage("(1)외부 베터리를 해제하고 \r(2)OHT에 전원을 넣고\r(3)TP를 OHT접속후\r\r OK버튼을 눌러 주세요");
			iStatus++;
		 }
		 break;

		 case Sim_Step::BatteryCut_Confirm:
		 {
			iRet = MessageBox(Handle, L"CID-O-S 양쪽이 깜박거리고 않고 있습니까? (그림참고)",
							  L"확인해 주세요", MB_YESNO | MB_ICONQUESTION);

			if(iRet == IDYES)
			{
				Log("Test2 - 양쪽 깜박임 Off Pass");
				Pass(2, true);
			}
			else
			{
				Log("Test2 - 양쪽 깜박임 Off Fail");
				Pass(2, false);
			}

			iStatus++;
		 }
		 break;

		 // (3) O6(Operating Mode) On
		 case Sim_Step::OperatingModeOnCheck:
		 {
			Log("(3-1) Output All Clear");
			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);
			Sleep(100);

			Log("(3-2) O6(Operating Mode) On");
			
			iStatus++;

		 }
		 break;

		 case Sim_Step::OperatingModeOnCheck_Sleep:
		 {			ReadCidInput(iI1, iI2, iI3, iI4);

			if((iI1 == 0) && (iI2 == 0) && (iI3 == 0) && (iI4 == 0))
			{
				Log("InputCheck Pass");
				Pass(3, true);
			}
			else
			{
				Log("InputCheck Fail");
				Pass(3, false);
			}

			iStatus++;		 	
		 }
		 break;

		 // ### (4) O1(Left Select) On
		 case Sim_Step::LeftCidLampOn:
		 {
			Log("(4) O1(Left Select) On");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::LeftCidLampOn_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 
			 if((iI1 == 1) && (iI2 == 0) && (iI3 == 0) && (iI4 == 0))
			 {
				 Log("I1(Lilk)확인 Pass");
			 }
			 else
			 {
				 Log("I1(Lilk)확인 Fail");
			 }
			 
			 ReadCidOutput(iO1, iO2, iO3, iO4, iO5, iO6, iO7, iO8);
			 
			 if((iO1 == 1) && (iO6 == 1))
			 {
				 Log("O1, O6 확인 Pass");
			 }
			 else
			 {
				 Log("O1, O6 확인 Fail");
			 }
			 
			 iStatus++;
		 }

		 case Sim_Step::LeftCidLampOn_Confirm:
		 {
			iRet = MessageBox(Handle, L"왼쪽 CID-O-S Sensor에 불이 들어왔습니까? (그림참고)",
							  L"확인해 주세요", MB_YESNO | MB_ICONQUESTION);

			if(iRet == IDYES)
			{
				Log("Left Select 육안 확인 Pass");
			}
			else
			{
				Log("Left Select 육안 확인 Fail");
			}

			if((iI1 == 1) && (iO1 == 1) && (iO6 == 1) && (iRet == IDYES))
				Pass(4, true);
			else
				Pass(4, false);

			iStatus++;
		 }
		 break;

		 // (5) O3(합류점유) On
		 case Sim_Step::InputInterlockOnCheck:
		 {
			Log("O3(합류점유) On");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_ON);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::InputInterlockOnCheck_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 // if(iI2 == 1)
			 if((iI1 == 1) && (iI2 == 1) && (iI3 == 0) && (iI4 == 0))
			 {
				 Log("Interlock On Pass");
				 Pass(5, true);
			 }
			 else
			 {
				 Log("Interlock On Fail");
				 Pass(5, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (6) O3(합류점유) Off
		 case Sim_Step::InputInterlockOffCheck:
		 {
			Log("(6) O3(합류점유) Off");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::InputInterlockOffCheck_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI2 == 0)
			 {
				 Log("Interlock Off Pass");
				 Pass(6, true);
			 }
			 else
			 {
				 Log("Interlock Off Fail");
				 Pass(6, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (7) O4(통과완료) On
		 case Sim_Step::CidPassAckOn:
		 {
			Log("(7) O4(통과완료) On");
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_ON);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidPassAckOn_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI3 == 1)
			 {
				 Log("PassAck On Pass");
				 Pass(7, true);
			 }
			 else
			 {
				 Log("PassAck On Fail");
				 Pass(7, false);
			 }
			 
			 iStatus++;
		 }
		 break;
		 
		 // (8) O4(통과완료) Off
		 case Sim_Step::CidPassAckOff:
		 {
			Log("(8) O4(통과완료) Off");

			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidPassAckOff_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
        // CID-O 버전에 따른 사양 변경
             // V1.03 이상 - O4(통과완료) On 시 On, O1(Left Select) Off 시 Off
             // V1.03 미만 - O4(통과완료) On 시 On, O4(통과완료) Off 시 Off
#if 1
             if(iI3 == 1)
             {
                 Log("PassAck Off Pass");
                 Pass(8, true);
             }
             else
             {
                 Log("PassAck Off Fail");
                 Pass(8, false);
             }
#else
             if(iI3 == 0)
             {
                 Log("PassAck Off Pass");
                 Pass(8, true);
             }
             else
             {
                 Log("PassAck Off Fail");
                 Pass(8, false);
             }
#endif

			 
			 iStatus++;
		 }
		 break;

		 // (9) O5(Abnormal) On
		 case Sim_Step::CidAbnormalOn_1:
		 {
			Log("(9) O5(Abnormal) On");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_ON);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidAbnormalOn_1_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI4 == 1)
			 {
				 Log("Abnormal On Pass");
				 Pass(9, true);
			 }
			 else
			 {
				 Log("Abnormal On Fail");
				 Pass(9, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (10) O5(Abnormal) Off
		 case Sim_Step::CidAbnormalOff_1:
		 {
			Log("(10) O5(Abnormal) Off");

			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidAbnormalOff_1_Sleep:
	 	 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI4 == 0)
			 {
				 Log("Abnormal Off Pass");
				 Pass(10, true);
			 }
			 else
			 {
				 Log("Abnormal Off Fail");
				 Pass(10, false);
			 }
			 
			 iStatus++;
	 	 }
		 break;

		// ====== Step 10 ~ 19
		 // (11) O1(Left Select) Off
		 case Sim_Step::CidLeftSelectOff:
		 {
			Log("(11) O1(Left Select) Off");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidLeftSelectOff_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI1 == 0)
			 {
				 Log("O1(Left Select) Off Pass");
				 Pass(11, true);
			 }
			 else
			 {
				 Log("O1(Left Select) Off Fail");
				 Pass(11, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (12) 시리얼 명령 'M' 입력
		 case Sim_Step::CidLightCheck1:
		 {
			Log("(12) 시리얼 명령 'M' 입력");

			if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
			{
				FoupOnlineCMDForm->btnCidMonitoringClick(NULL);
			}
			else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
			{
				EUVOnlineCMDForm->btnCidMonitoringClick(NULL);
			}
			else
			{
				ShowMessage("Unsupported OHT");
			}

			/*
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

			// right off, left on
			m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL_OFF);
			m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL);
			delete m_CMDControl;
			*/

			iStatus++;
		 }
		 break;



		 case Sim_Step::CidLightCheck1_Confirm:
		 {
			 Log("(12) 시리얼 명령 'M' 입력 - Result Check => Left");

			 int iCidO, iCidR;

			 if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
			 {
				iCidO = StrToInt(FoupOnlineCMDForm->edtCid_O_Link->Text);
				iCidR = StrToInt(FoupOnlineCMDForm->edtCid_R_Link->Text);
			 }
			 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
			 {
				iCidO = StrToInt(EUVOnlineCMDForm->edtCid_O_Link->Text);
				iCidR = StrToInt(EUVOnlineCMDForm->edtCid_R_Link->Text);
			 }

			 if((iCidO < 230) || (iCidR < 230))
			 {
				Log("Light < 230, Test Fail");
				Log(IntToStr(iCidO));
				Log(IntToStr(iCidR));
				Pass(12, false);
			 }
			 else
			 {
				Log("Light Test Pass");
				Pass(12, true);
			 }

			 iStatus++;
		 }
		 break;

#if 1
		 // (13) O7(Common Tag) On
		 case Sim_Step::CidCommonTagOn:
		 {
			Log("(13) O7(Common Tag) On");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_ON);
			
			Pass(13, true);
			iStatus++;
		 }
		 break;

		 // (14) O7(Common Tag) Off
		 case Sim_Step::CidCommonTagOff:
		 {
			Log("(14) O7(Common Tag) Off");
			
			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			Pass(14, true);
			iStatus++;
		 }
		 break;
#else
		// (13) O7(Common Tag) Off
		case Sim_Step::CidCommonTagOff:
		{
		   Log("(13) O7(Common Tag) Off");
		   SetCidOutput(7, CID_OFF);
		   Pass(13, true);
		   iStatus++;
		}
		break;

		// (13) O7(Common Tag) On
		case Sim_Step::CidCommonTagOn:
		{
		   Log("(14) O7(Common Tag) On");
		   SetCidOutput(7, CID_ON);
		   Pass(14, true);
		   iStatus++;
		}
		break;
#endif

		 // (15) O1(Left Select) On
		 case Sim_Step::CidLeftSelOn:
		 {
			Log("(15) O1(Left Select) On");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

#if 1
		 case Sim_Step::CidLeftSelOn_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI1 == 0)
			 // old if(iI1 == 1)
			 {
				 Log("O1(Left Select) Off => Pass");
				 Pass(15, true);
			 }
			 else
			 {
				 Log("O1(Left Select) On => Fail");
				 Pass(15, false);
			 }
			 
			 iStatus++;
		 }
		 break;
#else
		case Sim_Step::CidLeftSelOn_Sleep:
		{
			ReadCidInput(iI1, iI2, iI3, iI4);
			//temp.. if(iI1 == 0)
			if(iI1 == 1)
			{
				Log("O1(Left Select) On. Fail");
				Pass(15, false);
			}
			else
			{
				Log("O1(Left Select) On. Pass");
				Pass(15, true);
			}
			
			iStatus++;
		}
		break;
#endif

		 // (16) O1(Left Select) & O6(Operating Mode) Off
		 case Sim_Step::CidLeftSelOff_And_OpModeOff:
		 {
			Log("(16) O1(Left Select) & O6(Operating Mode) Off");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_OFF);
			SetCidOutput(7, CID_OFF);

			Pass(16, true);
			iStatus++;
		 }
		 break;

		 // (17) O1(Left Select) On & Off
		 case Sim_Step::CidLeftSelOn_And_Off_1:
		 {
			Log("(17) O1(Left Select) On");
			
			SetCidOutput(1, CID_ON);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_OFF);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidLeftSelOn_And_Off_1_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI1 == 0)
			 {
				 Log("I1(Link) = 0 Pass");
			 }
			 else
			 {
				 Log("I1(Link) = 0 Fail");
			 }
			 
			 iStatus++;
		 }
		 break;

		 case Sim_Step::CidLeftSelOn_And_Off_2:
		 {
			Log("(17) O1(Left Select)Off");
			
			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_OFF);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidLeftSelOn_And_Off_2_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI1 == 0)
			 {
				 Log("I1(Link) = 0 Pass");
				 Pass(17, true);
			 }
			 else
			 {
				 Log("I1(Link) = 0 Fail");
				 Pass(17, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (18) O6(Operating Mode) On
		 case Sim_Step::CidOpModeOn:
		 {
			Log("(18) O6(Operating Mode) On");
			
			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_OFF);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			Pass(18, true);
			iStatus++;
		 }
		 break;

		 // ### (19) O2(Right Select) On
		 case Sim_Step::CidRightSelectOn:
		 {
			Log("(19) O2(Right Select) On");
			
			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_ON);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 // Right CID-O-S Sensor에 불이 들어왔습니까? (그림참고)
		 case Sim_Step::CidRightSelectOn_Confirm:
		 {
			iRet = MessageBox(Handle, L"오른쪽 CID-O-S Sensor에 불이 들어왔습니까? (그림참고)",
							  L"확인해 주세요", MB_YESNO | MB_ICONQUESTION);

			if(iRet == IDYES)
			{
				Log("Right Select 육안 확인 Pass");
			}
			else
			{
				Log("Right Select 육안 확인 Fail");
			}

			ReadCidInput(iI1, iI2, iI3, iI4);
			if(iI1 == 1)
			{
				Log("I1(Link) = 1 Pass");
			}
			else
			{
				Log("I1(Link) = 1 Fail");
			}

			if((iI1 == 1) && (iRet == IDYES))
				Pass(19, true);
			else
				Pass(19, false);

			iStatus++;
		 }
		 break;

		// ====== Step 20 ~ 29
		// (20) O3(합류점유) On
		 case Sim_Step::CidInterlockOn:
		 {
			Log("(20) O3(합류점유) On");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_ON);
			SetCidOutput(3, CID_ON);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidInterlockOn_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI2 == 1)
			 {
				 Log("I2(Interlock) = 1 Pass");
				 Pass(20, true);
			 }
			 else
			 {
				 Log("I2(Interlock) = 1 Fail");
				 Pass(20, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (21) O3(합류점유) Off
		 case Sim_Step::CidInterlockOff:
		 {
			Log("(21) O3(합류점유) Off");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_ON);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidInterlockOff_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI2 == 0)
			 {
				 Log("I2(Interlock) = 0 Pass");
				 Pass(21, true);
			 }
			 else
			 {
				 Log("I2(Interlock) = 0 Fail");
				 Pass(21, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (22) O4(통과완료) On
		 case Sim_Step::CidPassOkOn:
		 {
			Log("(22) O4(통과완료) On");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_ON);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_ON);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidPassOkOn_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI3 == 1)
			 {
				 Log("I3(PassOk) = 1 Pass");
				 Pass(22, true);
			 }
			 else
			 {
				 Log("I3(PassOk) = 1 Fail");
				 Pass(22, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (23) O4(통과완료) Off
		 case Sim_Step::CidPassOkOff:
		 {
			Log("(23) O4(통과완료) Off");

			SetCidOutput(1, CID_OFF);
			SetCidOutput(2, CID_ON);
			SetCidOutput(3, CID_OFF);
			SetCidOutput(4, CID_OFF);
			SetCidOutput(5, CID_OFF);
			SetCidOutput(6, CID_ON);
			SetCidOutput(7, CID_OFF);

			iStatus++;
		 }
		 break;

		 case Sim_Step::CidPassOkOff_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
           // CID-O 버전에 따른 사양 변경
             // V1.03 이상 - O4(통과완료) On 시 On, O1(Left Select) Off 시 Off
             // V1.03 미만 - O4(통과완료) On 시 On, O4(통과완료) Off 시 Off
#if 1
             if(iI3 == 1)
             {
                 Log("I3(PassOk) = 1 Pass");
                 Pass(23, true);
             }
             else
             {
                 Log("I3(PassOk) = 0 Fail");
                 Pass(23, false);
             }
#else
             if(iI3 == 0)
             {
                 Log("I3(PassOk) = 0 Pass");
                 Pass(23, true);
             }
             else
             {
                 Log("I3(PassOk) = 1 Fail");
                 Pass(23, false);
             }
#endif

			 iStatus++;
		 }
		 break;

		 // (24) O5(Abnormal) On
		 case Sim_Step::CidAbnormalOn_2:
		 {
			Log("(24) O5(Abnormal) On");

			SetCidOutput(5, CID_ON);
			iStatus++;
		 }
		 break;

		 case Sim_Step::CidAbnormalOn_2_Sleep:
	 	 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI4 == 1)
			 {
				 Log("I4(Abnormal On) = 1 Pass");
				 Pass(24, true);
			 }
			 else
			 {
				 Log("I4(Abnormal On) = 1 Fail");
				 Pass(24, false);
			 }
			 
			 iStatus++;
	 	 }
		 break;

		 // (25) O5(Abnormal) Off
		 case Sim_Step::CidAbnormalOff_2:
		 {
			Log("(25) O5(Abnormal) Off");

			SetCidOutput(5, CID_OFF);
			iStatus++;
		 }
		 break;

		 case Sim_Step::CidAbnormalOff_2_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI4 == 0)
			 {
				 Log("I4(Abnormal On) = 0 Pass");
				 Pass(25, true);
			 }
			 else
			 {
				 Log("I4(Abnormal On) = 0 Fail");
				 Pass(25, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (26) O2(Left Select) Off & O6(Operating Mode) Off
		 case Sim_Step::CidLeftSelOn_And_OpModeOff:
		 {
			Log("(26) O2(Left Select) Off & O6(Operating Mode) Off");

			SetCidOutput(2, CID_OFF);
			SetCidOutput(6, CID_OFF);
			iStatus++;
		 }
		 break;

		 case Sim_Step::CidLeftSelOn_And_OpModeOff_Sleep:
		 {
			 ReadCidInput(iI1, iI2, iI3, iI4);
			 if(iI1 == 0)
			 {
				 Log("I1(Link) = 0 Pass");
				 Pass(26, true);
			 }
			 else
			 {
				 Log("I1(Link) = 0 Fail");
				 Pass(26, false);
			 }
			 
			 iStatus++;
		 }
		 break;

		 // (27) 시리얼 명령 'M' 입력
		 case Sim_Step::CidLightCheck2:
		 {
			Log("(27) 시리얼 명령 'M' 입력 => Right");


			if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
			{
				FoupOnlineCMDForm->btnCidMonitoring_RightClick(NULL);
			}
			else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
			{
				EUVOnlineCMDForm->btnCidMonitoring_RightClick(NULL);
			}
			else
			{
				ShowMessage("Unsupported OHT");
			}


			/*
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

			// left off, right on
			m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL_OFF);
			m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL);
			delete m_CMDControl;
			*/

			iStatus++;
		 }
		 break;


		 case Sim_Step::CidLightCheck2_Confirm:
		 {
			 Log("(27) 시리얼 명령 'M' 입력 - Result Check");

			 int iCidO, iCidR;

			 if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
			 {
				iCidO = StrToInt(FoupOnlineCMDForm->edtCid_O_Right->Text);
				iCidR = StrToInt(FoupOnlineCMDForm->edtCid_R_Right->Text);
			 }
			 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
			 {
				iCidO = StrToInt(EUVOnlineCMDForm->edtCid_O_Right->Text);
				iCidR = StrToInt(EUVOnlineCMDForm->edtCid_R_Right->Text);
			 }

			 if((iCidO < 230) || (iCidR < 230))
			 {
				Log("Right < 230, Test Fail");
				Pass(27, false);
			 }
			 else
			 {
				Log("Right Test Pass");
				Pass(27, true);
			 }

			 iStatus++;
		 }
		 break;


		 // (28) Cid Check Complete
		 case Sim_Step::CidCheckComplete:
		 {
			Log("(28) Cid Check Complete");
			Pass(28, true);
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step::None;
		 }
		 break;
	 }

	 if(iStatus != Sim_Step::None)
		tmrSimulation->Enabled = true;
}

//---------------------------------------------------------------------------
void TfrmCidChecker::Pass(int iIndex, bool bPass)
{
	if(bPass)
		strGridCheckList->Cells[2][iIndex] = L"Pass";
	else
		strGridCheckList->Cells[2][iIndex] = L"Fail";
}
//---------------------------------------------------------------------------
void TfrmCidChecker::Log(UnicodeString strLog)
{
	mmInspectLog->Lines->Add(strLog);
}

//---------------------------------------------------------------------------
void __fastcall TfrmCidChecker::tmrCidDisplayTimer(TObject *Sender)
{
	// CID input

		shpCID_Link_Compt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN == ON) ? COLOR_LIME : COLOR_WHITE;
		shpCID_Interlock->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
		shpCID_Pass_OK->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;

		shpCID_Status->Brush->Color		= (gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN 	== ON) ? COLOR_LIME : COLOR_WHITE;
		shpCID_Reserved_1->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1 	== ON) ? COLOR_LIME : COLOR_WHITE;
		shpCID_Reserved_2->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2 	== ON) ? COLOR_LIME : COLOR_WHITE;

		// CID output
		shpOut_LeftSel->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_RightSel->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_OccReq->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_Passed->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion == ON) ? COLOR_LIME : COLOR_WHITE;

		shpOut_Manual->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_Operation->Brush->Color 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_Rev1->Brush->Color 		= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag == ON) ? COLOR_LIME : COLOR_WHITE;
		shpOut_Rev2->Brush->Color 		= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2 == ON) ? COLOR_LIME : COLOR_WHITE;

}
//---------------------------------------------------------------------------

// Input값을 읽어온다 (인수는 1~4이며, I1 ~ I4값 확인)
void TfrmCidChecker::ReadCidInput(int &I1, int &I2, int &I3, int &I4)
{
	AnsiString strInputLog;

		I1	= (gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN == ON) ? 1 : 0;
		I2	= (gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN 	== ON) ? 1 : 0;
		I3	= (gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN 	== ON) ? 1 : 0;
		I4	= (gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN 	== ON) ? 1 : 0;



	strInputLog.sprintf("CID Input Status => I1 : %d I2 : %d I3 : %d I4 : %d", I1, I2, I3, I4);
	Log(strInputLog);
}
//---------------------------------------------------------------------------

// Output값을 읽어온다 (인수는 1~8이며, O1 ~ O8값 확인)
void TfrmCidChecker::ReadCidOutput(int &O1, int &O2, int &O3, int &O4, int &O5, int &O6, int &O7, int &O8)
{
	AnsiString strOutputLog;

	O1 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON == ON) 	? 1 : 0;
	O2 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON == ON) 	? 1 : 0;
	O3 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy == ON) 		? 1 : 0;
	O4 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion == ON) 		? 1 : 0;

	O5 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal == ON) 			? 1 : 0;
	O6 	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode == ON) 		? 1 : 0;
	O7	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag == ON) 		? 1 : 0;
	O8	= (gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2 == ON) 		? 1 : 0;


	strOutputLog.sprintf("CID Output Status => O1 : %d O2 : %d O3 : %d O4 : %d O5 : %d O6 : %d O7 : %d O8 : %d", O1, O2, O3, O4, O5, O6, O7, O8);
	Log(strOutputLog);
}
//---------------------------------------------------------------------------

// CID Output을 Set
// iCidNo 인수는 1 ~ 7,  iOnOff는 0또는 1
void TfrmCidChecker::SetCidOutput(int iCidNo, int iOnOff)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	AnsiString strOutputLog;

	strOutputLog.sprintf("CID 설정값 - 포트 : %d  Set : %d", iCidNo, iOnOff);
	Log(strOutputLog);

	switch(iCidNo)
	{
		case 1:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL_OFF);
		break;

		case 2:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL_OFF);
		break;

		case 3:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ_OFF);
		break;

		case 4:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT_OFF);
		break;

		case 5:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL_OFF);
		break;

		case 6:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION_OFF);
		break;

		case 7:
			if(iOnOff)	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1);
			else		m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1_OFF);
		break;
	}

	delete m_CMDControl;
}

void __fastcall TfrmCidChecker::btnCidLeftSelClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidLeftSel_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_LEFT_SEL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRightSelClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRightSel_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RIGHT_SEL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidOccupReqClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidOccupReq_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OCCUPREQ_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidPassComptClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidPassCompt_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_PASSCOMPT_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidManualClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidManual_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_MANUAL_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidOperationClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidOperation_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_OPERATION_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRev1Click(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRev1_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_1_OFF);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRev2Click(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCidRev2_OffClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_CID_RESERVE_2_OFF);
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::tmrSimulationTimer(TObject *Sender)
{
	StatusMachine(Sim_Cmd::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnCloseClick(TObject *Sender)
{
	tmrSimulation->Enabled = false;
	this->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCidChecker::btnLogClearClick(TObject *Sender)
{
	mmInspectLog->Lines->Clear();
}
//---------------------------------------------------------------------------

