//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AutoJob.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"


#include "ResponsePasspermitOCS.h"
#include "OcsPassConfirmResponse.h"
#include "RunningStatus.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmAutoJob *frmAutoJob;

TfrmResponsePasspermitOCS 	*frmPasspermitOCS;
TfrmOcsPassConfirmResponse 	*frmOcsPassConfirm;

TfrmRunningStatus			*frmViewLog;

#define GRD_NUM				0	// No
#define GRD_MAIN_CMD		1	// 명령종류
#define GRD_SUB_CMD			2	// 상세동작
#define GRD_CMD_ID			3	// 지시ID
#define GRD_TARGET_NODE		4	// 목적노드

#define GRD_STOP_ORDER		5	// 정지지시
#define GRD_PATH_DATA		6	// 경로데이터
#define GRD_START_NODE		7	// 시작노드
#define GRD_TARGET_STATION	8	// 목적지 STA
#define GRD_STOP_TAG		9	// Stop Tag

#define GRD_RUN_DIST		10	// Rundist	
#define GRD_NEXT_NODE_ID	11	// NextNodeID
#define GRD_HOIST			12	// Hoist
#define GRD_SHIFT			13	// Shift
#define GRD_ROTATE			14	// Rotate

#define GRD_DRIVING			15
#define GRD_RESERVED_16		16
#define GRD_RESERVED_17		17

#define GRID_MAX_COL		18	// 그리드의 컬럼 갯수

//---------------------------------------------------------------------------
__fastcall TfrmAutoJob::TfrmAutoJob(TComponent* Owner)
	: TForm(Owner)
{
	// 그리드에 레시피 항목을 표시
	FeederGrid->Cells[GRD_NUM][0] = "No";
	FeederGrid->Cells[GRD_MAIN_CMD][0] = "명령종류";
	FeederGrid->Cells[GRD_SUB_CMD][0] = "상세동작";
	FeederGrid->Cells[GRD_CMD_ID][0] = "지시ID";
	FeederGrid->Cells[GRD_TARGET_NODE][0] = "목적노드";
	
	FeederGrid->Cells[GRD_STOP_ORDER][0] = "정지지시";
	FeederGrid->Cells[GRD_PATH_DATA][0] = "경로데이터";
	FeederGrid->Cells[GRD_START_NODE][0] = "시작노드";
	FeederGrid->Cells[GRD_TARGET_STATION][0] = "목적지 STA";
	FeederGrid->Cells[GRD_STOP_TAG][0] = "Stop Tag";
		
	FeederGrid->Cells[GRD_RUN_DIST][0] = "Rundist";
	FeederGrid->Cells[GRD_NEXT_NODE_ID][0] = "Next(삭제)";
	FeederGrid->Cells[GRD_HOIST][0] = "Hoist";
	FeederGrid->Cells[GRD_SHIFT][0] = "Shift";
	FeederGrid->Cells[GRD_ROTATE][0] = "Rotate";

	FeederGrid->Cells[GRD_DRIVING][0] = 	"Driving(0,1)";
	FeederGrid->Cells[GRD_RESERVED_16][0] = "예비2";
	FeederGrid->Cells[GRD_RESERVED_17][0] = "예비3";

	GridClear();

	// ============== 상태 저장소 초기화 ==============
	memset(&PKT_OCS_STATUS, 0, sizeof(PKT_OCS_STATUS_ID_01));	// OCS상태 초기화

	// ---- Header
	PKT_OCS_STATUS.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&PKT_OCS_STATUS.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(PKT_OCS_STATUS.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	PKT_OCS_STATUS.TAG.Value = TagID::MCP_TO_CLW_STATUS;

	// --- Status
	int iTemp = 0x00;	// 기동 OFF
	memcpy(&PKT_OCS_STATUS.STATUS.StatusOrder, &iTemp, 1);

	// ============== 프로그램 핸들링 =============
	CurrentFileName = "UNTITLED.PRG";

	// 하부 폼 생성
	frmPasspermitOCS = new TfrmResponsePasspermitOCS(Application);
	frmPasspermitOCS->SetButtonMode(true);
	frmPasspermitOCS->SetResponseMode(0);	// 항상 Ack가 디폴트
	frmPasspermitOCS->btnHideClick(NULL);

	frmOcsPassConfirm = new TfrmOcsPassConfirmResponse(Application);
	frmOcsPassConfirm->SetButtonMode(true);
	frmOcsPassConfirm->SetResponseMode(0);	// 항상 Ack가 디폴트
	frmOcsPassConfirm->btnHideClick(NULL);

	frmViewLog = new TfrmRunningStatus(Application);
	frmViewLog->btnHideClick(NULL);

	bOHT_NOW_RUNNING = false;
}

//---------------------------------------------------------------------------
void TfrmAutoJob::GridClear()
{
	AnsiString fwidth;
	i_Cmd_Count = 0;
	FeederGrid->RowCount = 1;

	for(int i = 0; i < (i_Cmd_Count + 1); i++)
	{
		FeederGrid->Cells[GRD_NUM][i+1] = IntToStr(i+1);			// No 초기화
	}

	edtCount->Text = IntToStr(i_Cmd_Count);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::FeederGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect)
{
	if(ACol == 5) 	// 정지지시
	{		
		FWidthComboBox->Visible = false;

		FWidthComboBox->Top = FeederGrid->Top + 2 +
			FeederGrid->DefaultRowHeight*(ARow - FeederGrid->TopRow); // +1);
								  // + 1 * (ARow - FeederGrid->TopRow);

		FWidthComboBox->Text = FeederGrid->Cells[ACol][ARow];
		int index = FWidthComboBox->Items->IndexOf(FeederGrid->Cells[ACol][ARow]);
		if(index == -1) index = 0;

		FWidthComboBox->ItemIndex = index;
		FWidthComboBox->Visible = true;
	}
	else
	{
		FWidthComboBox->Visible = false;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::FWidthComboBoxChange(TObject *Sender)
{
	AnsiString fwidth = FWidthComboBox->Items->Strings[FWidthComboBox->ItemIndex].c_str();
	FeederGrid->Cells[GRD_STOP_ORDER][FeederGrid->Row] = fwidth.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::btnNewClick(TObject *Sender)
{
	edtFileName->Text = "UNTITLED.PRG";
	GridClear();
}
//---------------------------------------------------------------------------
void TfrmAutoJob::DisplayBlank(int iGrdIndex)
{
	for(int i = 1; i < GRID_MAX_COL; i++)
		FeederGrid->Cells[i][iGrdIndex] = "-------";
}

// 메인 커맨드를 설정후 이 함수를 호출하면 자동으로 딸려있는 커맨드 Set을 추가한다
void TfrmAutoJob::Minor_Cmd_AutoSet(int iMajorCmd)
{
	int iOrdID; 
	AnsiString fwidth = cboMajorCmd->Items->Strings[iMajorCmd].c_str();	// 설정된 텍스트 얻음

	switch(iMajorCmd)
	{
		// 기동명령
		case Sim_Major_Cmd::MOVE_ON:
			// MCP상태보고 (명령)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "MCP_TO_CLW_STATUS";			

			// CLW상태보고 (응답)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "CLW_TO_MCP_STATUS";			
		break;

		// 정지명령
		case Sim_Major_Cmd::MOVE_OFF:
			// MCP상태보고 (명령)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);

			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "MCP_TO_CLW_STATUS";			
			FeederGrid->Cells[GRD_STOP_ORDER][i_Cmd_Count] 	= "주행감속+이재감속";

			// CLW상태보고 (응답)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);

			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "CLW_TO_MCP_STATUS";							
		break;

		// 작업지시 - 이동 사이클 (정상)
		case Sim_Major_Cmd::ORD_MOVE_NORMAL:
			// ========== 작업지시 (명령)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// 목적노드

			// 5없음!! 주의할 것!!!!														// 5없음!! 주의할 것!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// 경로데이터
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// 시작노드
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// 목적지 STA
			FeederGrid->Cells[GRD_STOP_TAG]		[i_Cmd_Count] = "0";						// Stop Tag

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted!  FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate

			// =========== 작업지시 응답
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);

			// ========= 진척보고 (도착)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;

		// 작업지시 - 올림사이클 (정상)
		case Sim_Major_Cmd::ORD_UP_NORMAL:
			
			// =========== 작업지시 (명령)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// 목적노드

			// 5없음!! 주의할 것!!!!														// 5없음!! 주의할 것!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// 경로데이터
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// 시작노드
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// 목적지 STA			

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted! FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate
			FeederGrid->Cells[GRD_DRIVING]		[i_Cmd_Count] = "1";						// Driving (0, 1)

			// ========= 작업지시 응답
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			// FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";

			// ========= 진척보고 (도착)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[+i_Cmd_Count] 	= "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;

		// 작업지시 - 내림사이클 (정상)
		case Sim_Major_Cmd::ORD_DN_NORMAL:			
			
			// ========= 작업지시 (명령)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// 목적노드

			// 5없음!! 주의할 것!!!!														// 5없음!! 주의할 것!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// 경로데이터
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// 시작노드
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// 목적지 STA			

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted! FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate
			FeederGrid->Cells[GRD_DRIVING]		[i_Cmd_Count] = "1";						// Driving (0, 1)
			
			// ========= 작업지시 응답
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);

			// ========= 진척보고 (도착)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;
	}

	edtCount->Text = IntToStr(i_Cmd_Count);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::btnAddClick(TObject *Sender)
{
	Minor_Cmd_AutoSet(cboMajorCmd->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnDeleteClick(TObject *Sender)
{
	if(FeederGrid->RowCount > 1)
	{
		FeederGrid->RowCount = FeederGrid->RowCount - 1;
		i_Cmd_Count--;

		edtCount->Text = IntToStr(i_Cmd_Count);
	}
}
//---------------------------------------------------------------------------

// i_Run_Position 은 0부터 시작하지만, 이는 실행중이 아닐 시점이다.
// 첫 명령어는 1부터 있다고 가정한다 (리스트와 일치하도록)
void TfrmAutoJob::StatusMachine(int iCmd, PKT_OHT2OCS Pkt, int iTagID, int iPacketSize)
{
	static int i_Run_Position = 0; // 실행한 명령 없음
	static int i_Run_Control = Sim_Run_Cmd::STOP;
	int iMajor_Cmd, iMinor_Cmd;
	unsigned char cVar1, cVar2;
	AnsiString strLog;
	//tmrCmdSend->Enabled = false;

	// ========== 스테이트 머신 핸들링 및 OHT 반응 처리 ==========
	switch(iCmd)
	{
		case Sim_Run_Cmd::RUN:		// 처음부터 실행한다
			i_Run_Position = 0;
			i_Run_Control = Sim_Run_Cmd::CONTINUE;	// 계속 다음 단계로 쭉쭉 가자			
		break;

		case Sim_Run_Cmd::STOP:
			i_Run_Position = 0;
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::PAUSE:
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::CONTINUE:
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::RETRY:	// 명령을 보냈는데 응답이 없어서 5초 대기후 명령 재전송한다
			i_Run_Position--;
			i_Run_Control = Sim_Run_Cmd::CONTINUE;	
		break;
		// ====================== OHT반응 수신처리 ==========================
		case Sim_Run_Cmd::RESPONSE:
			frmViewLog->Log("OHT로부터 패킷 수신");

			switch(iTagID)
			{
				case TagID::CLW_TO_MCP_STATUS: 		  	// OHT상태정보
				{
					frmViewLog->Log("CLW_TO_MCP_STATUS");

					PKT_STATUS_REPORT_ID_03	CmdPkt;
					memcpy(&CmdPkt, &Pkt, iPacketSize);

					// ===== Step 1. Received info process 
					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);
						
						switch(iMajor_Cmd)
						{
							/* 기동 ON 수락조건
							1) 기동중 ON
							2) 자동 ON		*/
							case Sim_Major_Cmd::MOVE_ON:
							{
								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 1)	// 기동중 ON
									&& (CmdPkt.STATUS.WorkingStatus.Auto == 1))		// 자동 ON
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT상태정보 응답 - 기동조건 만족");
								}
								else
								{
									frmViewLog->Log("OHT상태정보 응답 - 기동조건 만족하지 않음!!!!");
								}
							}
							break;

							/* 기동 OFF 수락조건
							1) 기동중 OFF
							2) 주행 OFF		*/
							case Sim_Major_Cmd::MOVE_OFF:
							{
								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 0)	// 기동중 OFF
									&& (CmdPkt.STATUS.RunningStatus.Running == 0))	// 주행 OFF
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT상태정보 응답 - 기동정지조건 만족");
								}
								else
								{
									frmViewLog->Log("OHT상태정보 응답 - 기동정지조건 만족하지 않음!!!!");
                                }
							}
							break;

							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// 작업지시(이동-정상)
							{
							         strLog.sprintf("Status 받음, Major Cmd - 작업지시(이동) :Wanted Run Pos : %d",(i_Run_Position + 1)); 
									 frmViewLog->Log(strLog);

									frmViewLog->Log("Status 받음, Major Cmd - 작업지시(이동)");
									strLog.sprintf("원하는 지시 ID : %d   정위치 : %d  주행 : %d  재하 : %d  HP : %d  Working %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP,
													CmdPkt.STATUS.WorkingStatus.NowRunning);
									frmViewLog->Log(strLog);

								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 1)
									&& (CmdPkt.STATUS.RunningStatus.Running == 1)
									&& (bOHT_NOW_RUNNING == false))
									{
										frmViewLog->Log("트리거 조건 만족하므로 다음 상태 기다림");

										bOHT_NOW_RUNNING = true;
										strLog.sprintf("bOHT_NOW_RUNNING = %d",  bOHT_NOW_RUNNING);
										frmViewLog->Log(strLog);

										break;
									}
								
								if(bOHT_NOW_RUNNING == true)
								{									
									if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// 정위치정지
										&& (CmdPkt.STATUS.RunningStatus.Running == 0))				// 주행
										// && (CmdPkt.STATUS.Carriage.Load1 == 0)
										// && (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))

									{
										FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
										tmrRetry->Enabled = false;
										frmViewLog->Log("OHT상태정보 => 작업지시 (이동-정상) 조건 만족");
										
										bOHT_NOW_RUNNING = false;
										frmViewLog->Log("bOHT_NOW_RUNNING = false");
									}
									else
									{
										frmViewLog->Log("OHT상태정보 => 작업지시 (이동-정상) 조건 만족하지 않음!!!!");
										strLog.sprintf("원하는 지시 ID : %d   정위치 : %d  주행 : %d  재하 : %d  HP : %d",
														StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
														CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
														CmdPkt.STATUS.RunningStatus.Running,
														CmdPkt.STATUS.Carriage.Load1,
														CmdPkt.STATUS.Carriage.LoadUnloadHP);
										frmViewLog->Log(strLog);
	                                }
								}
							}
							break;

							case Sim_Major_Cmd::ORD_UP_NORMAL:		// 작업지시(올림-정상)
							{
								if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// 정위치정지
									&& (CmdPkt.STATUS.RunningStatus.Running == 0)				// 주행 
									&& (CmdPkt.STATUS.Carriage.Load1 == 1)	// 
									&& (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))	// 
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT상태정보 => 작업지시(올림-정상) 만족");
								}
								else
								{
									frmViewLog->Log("OHT상태정보 => 작업지시(올림-정상) 만족하지 않음!!!!");
									strLog.sprintf("원하는 지시 ID : %d   정위치 : %d  주행 : %d  재하 : %d  HP : %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP);
									frmViewLog->Log(strLog);
                                }
							}
							break;

							case Sim_Major_Cmd::ORD_DN_NORMAL:		// 작업지시(내림-정상)
							{
								if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// 정위치정지
									&& (CmdPkt.STATUS.RunningStatus.Running == 0)				// 주행 
									&& (CmdPkt.STATUS.Carriage.Load1 == 0)	// 
									&& (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))	// 

								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT상태정보 => 작업지시(내림-정상) 만족");
								}
								else
								{
									frmViewLog->Log("OHT상태정보 => 작업지시(내림-정상)만족하지 않음!!!!");
									strLog.sprintf("원하는 지시 ID : %d  정위치 : %d  주행 : %d  재하 : %d  HP : %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP);
									frmViewLog->Log(strLog);
                                }
							}
							break;


							default:
							{
								frmViewLog->Log("CLW_TO_MCP_STATUS - default case!");
							}
							break;
						}
					}

					// ===== Step 2. OHT Status Display
					frmViewLog->DisplayOHT(CmdPkt);
					strLog.sprintf("WorkingStatus.NowRunning : %d  RunningStatus.Running : %d",
									CmdPkt.STATUS.WorkingStatus.NowRunning, CmdPkt.STATUS.RunningStatus.Running);
					frmViewLog->Log(strLog);
				}
				break;

				case TagID::JOB_ORDER_RESPONSE: 		// 작업지시 반응
				{
					frmViewLog->Log("JOB_ORDER_RESPONSE");

					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						PKT_JOB_ORDER_RESPONSE_ID_122	CmdPkt;
						memcpy(&CmdPkt, &Pkt, iPacketSize);

						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);

						switch(iMajor_Cmd)
						{
							/* 작업지시 응답(이동/정상) 수락조건
							1) 지시 ID가 일치
							2) 응답구분 = 정상접수
							3) 접수불가구분 = 정상
							4) 지시 구분 = 작업지시
							5) 지시사이클 = 이동 사이클 (포인트) */
							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// 작업지시(이동-정상)
							{
								strLog.sprintf("작업지시 반응(이동) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
								frmViewLog->Log(strLog);

							    strLog.sprintf("작업지시 응답 - 조건불만족, 응답구분 : %d  접수불가구분 : %d 지시 구분 : %d 지시사이클 : %d 지시 ID : %d",
														CmdPkt.BODY.ResponseAck.PresionCode,
														CmdPkt.BODY.ResponseAck.Accept_Error,
														CmdPkt.BODY.ResponseDiv.CMDType,
														CmdPkt.BODY.CmdCycle,
														CmdPkt.BODY.CmdID);
								frmViewLog->Log(strLog);
									  
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// 응답구분
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// 접수불가구분
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)				// 지시 구분 = 작업지시
								  && (CmdPkt.BODY.CmdCycle == CYCLE::MovingToStation)) 						// 지시사이클 = 이동 사이클
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("작업지시 응답 - 이동/정상 : 조건 만족");
								  }
								  else
								  {
								  	  strLog.sprintf("작업지시 응답 - 이동/정상 :Wanted Run Pos : %d",(i_Run_Position + 1)); 
									  frmViewLog->Log(strLog);
									  
									  strLog.sprintf("작업지시 응답 - 조건불만족, 응답구분 : %d  접수불가구분 : %d 지시 구분 : %d 지시사이클 : %d 지시 ID : %d",
														CmdPkt.BODY.ResponseAck.PresionCode,
														CmdPkt.BODY.ResponseAck.Accept_Error,
														CmdPkt.BODY.ResponseDiv.CMDType,
														CmdPkt.BODY.CmdCycle,
														CmdPkt.BODY.CmdID);
									  frmViewLog->Log(strLog);
                                  }
							}
							break;

							/* 작업지시 응답(올림/정상) 수락조건
							1) 지시 ID가 일치
							2) 응답구분 = 정상접수
							3) 접수불가구분 = 정상
							4) 지시 구분 = 작업지시
							5) 지시사이클 = 올림 사이클 (포인트) */
							case Sim_Major_Cmd::ORD_UP_NORMAL:		// 작업지시(올림-정상)
							{
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// 응답구분
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// 접수불가구분
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)				// 지시 구분 = 작업지시
								  && (CmdPkt.BODY.CmdCycle == CYCLE::CraneUp)) 								// 지시사이클 = 올림 사이클
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("작업지시 응답 -  올림/정상 : 조건 만족");
								  }
								  else
								  {
								  	strLog.sprintf("Wanted Run Pos : %d",(i_Run_Position + 1)); 
									frmViewLog->Log(strLog);
								  	strLog.sprintf("작업지시 응답 -  올림/정상 불만족 \r\n 지시 ID : %d  응답구분 : %d  접수불가구분 : %d  지시 구분 : %d  지시사이클 : %d",
												CmdPkt.BODY.CmdID, 						CmdPkt.BODY.ResponseAck.PresionCode,
												CmdPkt.BODY.ResponseAck.Accept_Error,	CmdPkt.BODY.ResponseDiv.CMDType,
												CmdPkt.BODY.CmdCycle);

									frmViewLog->Log(strLog);
                                  }
							}
							break;


							/* 작업지시 응답(내림/정상) 수락조건
							1) 지시 ID가 일치
							2) 응답구분 = 정상접수
							3) 접수불가구분 = 정상
							4) 지시 구분 = 작업지시
							5) 지시사이클 = 내림 사이클 (포인트)	*/
							case Sim_Major_Cmd::ORD_DN_NORMAL:		// 작업지시(내림-정상)
							{
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// 응답구분
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// 접수불가구분
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)   				// 지시 구분 = 작업지시
								  && (CmdPkt.BODY.CmdCycle == CYCLE::CraneDown)) 							// 지시사이클 = 내림 사이클
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("작업지시 응답 -  내림/정상 : 조건 만족");
								  }
								  else
								  {
								  	  strLog.sprintf("작업지시 응답 -  내림/정상 : Wanted Run Pos : %d",(i_Run_Position + 1)); 
									  frmViewLog->Log(strLog);
								  	// strLog.sprintf("
									// frmViewLog->Log("작업지시 응답 -  내림/정상 : 조건 만족하지 않음!!!!");
								  	strLog.sprintf("작업지시 응답 -  내림/정상 불만족 \r\n 지시 ID : %d  응답구분 : %d  접수불가구분 : %d  지시 구분 : %d  지시사이클 : %d",
												CmdPkt.BODY.CmdID, 						CmdPkt.BODY.ResponseAck.PresionCode,
												CmdPkt.BODY.ResponseAck.Accept_Error,	CmdPkt.BODY.ResponseDiv.CMDType,
												CmdPkt.BODY.CmdCycle);
									frmViewLog->Log(strLog);
								  }
							}
							break;
							
							default:
							{
								frmViewLog->Log("JOB_ORDER_RESPONSE - default case!");
							}
							break;
						}
					}						
				}
				break;

				case TagID::JOB_PROGRESS_REPORT: 		// 진척보고 반응
				{
					frmViewLog->Log("JOB_PROGRESS_REPORT");

					strLog.sprintf("진척보고 반응 : Wanted Run Pos : %d",(i_Run_Position + 1)); 
					frmViewLog->Log(strLog);


								
					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						PKT_JOB_PROGRESS_REPORT_ID_103	CmdPkt;
						memcpy(&CmdPkt, &Pkt, iPacketSize);

						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);

						strLog.sprintf("작업지시 응답 - 조건불만족, ProgressType : %d  CorrectPositionStop :  %d",
														CmdPkt.BODY.ProgressType,
														CmdPkt.STATUS.RunningStatus.CorrectPositionStop);
						frmViewLog->Log(strLog);

						switch(iMajor_Cmd)
						{
							/* 진척보고 응답(이동/정상) 수락조건
							1) 지시 ID가 일치
							2) 진척구분 = 도착
							3) 정위치 정지 = ON
							4) 주행중 = OFF  	*/
							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// 진척보고 (이동-정상)
							{

								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::Arrived) 					   		// 진척구분 = 도착
								  && (CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)) 					// 정위치 정지 = ON, 주행중 = OFF
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("진척보고 응답 -  이동/정상 : 조건 만족");
								  }
								  else
								  {
									  frmViewLog->Log("진척보고 응답 -  이동/정상 : 조건 만족하지 않음!!!!");
                                  }
							}
							break;

							/* 진척보고 응답(올림/정상) 수락조건
							1) 지시 ID가 일치
							2) 진척구분 = 이재완료
							3) 재고 = ON 		*/
							case Sim_Major_Cmd::ORD_UP_NORMAL:		// 진척보고 (올림-정상)
							{
								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::TransCompleted) 					// 진척구분 = 이재완료
								  && (CmdPkt.STATUS.Carriage.Load1 == 1))										// 재고 = ON
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("진척보고 응답 -  올림/정상 : 조건 만족");
								  }
								  else
								  {
									frmViewLog->Log("진척보고 응답 -  올림/정상 : 조건 만족하지 않음!!!!");
									strLog.sprintf("지시 ID : %d  진척구분 : %d  재고 : %d",
											CmdPkt.BODY.RcvCmdID, CmdPkt.BODY.ProgressType, CmdPkt.STATUS.Carriage.Load1);
									frmViewLog->Log(strLog);
                                  }
							}
							break;

							/* 진척보고 응답(내림/정상) 수락조건
							1) 지시 ID가 일치
							2) 진척구분 = 이재완료
							3) 재고 = OFF		*/
							case Sim_Major_Cmd::ORD_DN_NORMAL:		// 진척보고 (내림-정상)
							{
								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// 지시 ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::TransCompleted) 					// 진척구분 = 이재완료
								  && (CmdPkt.STATUS.Carriage.Load1 == 0))										// 재고 = OFF
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("진척보고 응답 -  내림/정상 : 조건 만족");
								  }
								  else
								  {
									frmViewLog->Log("진척보고 응답 -  내림/정상 : 조건 만족하지 않음!!!!");
									strLog.sprintf("지시 ID : %d  진척구분 : %d  재고 : %d",
											CmdPkt.BODY.RcvCmdID, CmdPkt.BODY.ProgressType, CmdPkt.STATUS.Carriage.Load1);
									frmViewLog->Log(strLog);
                                  }
							}
							break;

							default:
							{
								frmViewLog->Log("JOB_PROGRESS_REPORT - default case!");
							}
							break;

                        }
					}					
				}
				break;

				case TagID::CMD_PASSPERMIT_OHT2OCS:		// 통과허가 요구
				{
					frmViewLog->Log("통과허가 요구 수신 - 자동 핸들링 실행");
					PKT_PASSPERMIT_OHT2OCS_ID_241			CmdPkt_241;
					memcpy(&CmdPkt_241, &Pkt, iPacketSize);
					frmPasspermitOCS->Trigger(CmdPkt_241);
				}

				break;

				case TagID::CMD_PASSPERMITOPEN_OHT2OCS:	// 통과허가 개방요구
				{
					frmViewLog->Log("통과허가 개방요구 수신 - 자동 핸들링 실행");
					PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243	CmdPkt_243;
					memcpy(&CmdPkt_243, &Pkt, iPacketSize);
					frmOcsPassConfirm->Trigger(CmdPkt_243);
				}
				break;

				default: 	// 무시하면 되는 경우
				{
					frmViewLog->Log("Undefined Scenario case!!! Check please");
					// ShowMessage(L"Undefined Scenario case!");
				}
				break;
			}
		break;
 	}

	// ========== Status Machine에서 프로그램에 따라 자동으로 커맨드 전송 ==========
	if(i_Run_Control == Sim_Run_Cmd::CONTINUE)
	{
		// 다음 라인의 명령이 커맨드를 보내줘야 하는 경우
		if(IsSendCmd(i_Run_Position + 1) == true)
		{
			strLog.sprintf("(명령전송루틴 (보내야 하는 경우임) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
			frmViewLog->Log(strLog);
			
			if(Send2OHT(i_Run_Position + 1) > 0) 	// 오류없이 전송 OK
			{
				FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// 해당명령 실행했음을 알려준다
			}
		}
		else
		{
			strLog.sprintf("(명령전송루틴 (보내면 안됨) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
			frmViewLog->Log(strLog);		
		}

		// 무한 반복처리 (트리거 타이머 활성화)
		// if(((i_Run_Position + 1) == FeederGrid->RowCount)
		// 	&& (FeederGrid->Cells[GRD_NUM][i_Run_Position + 1] == "R"))
		strLog.sprintf("Run Pos : %d   RowCount : %d", (i_Run_Position + 1), FeederGrid->RowCount);
		frmViewLog->Log(strLog);
		
		if ((i_Run_Position + 1) == FeederGrid->RowCount)
		{
			
			frmViewLog->Log("(실행라인 초기화)");
			for(int i = 0; i < i_Cmd_Count; i++)
			{
				FeederGrid->Cells[GRD_NUM][i+1] = IntToStr(i+1);			// No 초기화
			}

			tmrCmdSend->Enabled = true;								// 다시 시작버튼을 누르도록 한다
		}

	}
	edtRunCount->Text = IntToStr(i_Run_Position + 1);
}

//---------------------------------------------------------------------------
// 이 함수는 명령을 보내는 것이다. 따라서 Major만 생각하면 된다.
// 그리드도 0부터 인덱스가 시작하며, 명령은 1부터 존재하므로 햇갈리지 않도록 한다

// 주의 ! PKT_OCS_STATUS 는 OHT에 계속 상태를 보낼때 사용하는 용도이므로
//        직접 초기화 하지 말것
int TfrmAutoJob::Send2OHT(int iGrdIndex)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	// int iStatusSize = sizeof(PROTOCOL_STATUS_OCS);
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;

	GetRunningCmd(iGrdIndex, iMajor_Cmd, iMinor_Cmd);

	strLog.sprintf("Send2OHT() %d", iGrdIndex);
	frmViewLog->Log(strLog);
	
	switch(iMajor_Cmd)
	{
		// --------------- 기동
		case Sim_Major_Cmd::MOVE_ON:
		{
			iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			PKT_OCS_STATUS_ID_01	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS상태 초기화 - 이전값을 읽어온다
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::MCP_TO_CLW_STATUS;

			// --- Status 수정
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;	// 기동 ON

			// 데이터 전송
			Conv_PKT_OCS_STATUS_ID_01(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
				return UNICAST_SEND_ADDRESS_NOT_RESISTED;
			}

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다
			tmrRetry->Enabled = true;
			frmViewLog->Log("기동명령 송신");
		}
		break;

		// --------------- 정지
		case Sim_Major_Cmd::MOVE_OFF:
		{
			iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			PKT_OCS_STATUS_ID_01	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS상태 초기화 - 이전값을 읽어온다
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::MCP_TO_CLW_STATUS;

			// --- Status 수정
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 0;	// 기동 OFF

			// 정지지시
			if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 		== "주행감속+이재감속")
				iTemp = bit_0 + bit_4;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "주행감속+이재즉")
				iTemp = bit_0 + bit_5;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "주행즉+이재감속")
				iTemp = bit_1 + bit_4;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "주행즉+이재즉")
				iTemp = bit_1 + bit_5;
			else
			{
				ShowMessage(L"감속 옵션에 예기치 못한 값이 존재합니다!!");
				return -1;
			}

			memcpy(&SEND_PACKET.STATUS.StopOrder, &iTemp, 1);

			// 데이터 전송
			Conv_PKT_OCS_STATUS_ID_01(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
				return UNICAST_SEND_ADDRESS_NOT_RESISTED;
			}

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다
			tmrRetry->Enabled = true;
			frmViewLog->Log("정지명령 송신");
		}
		break;

		// --------------- 작업지시 - 이동사이클 (정상)
		case Sim_Major_Cmd::ORD_MOVE_NORMAL:
		{
			bOHT_NOW_RUNNING = false;
				
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS상태 초기화 - 이전값을 읽어온다
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;

			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// 지시 ID

			iTemp = ORDERDIV::JobOrder;						// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

			iTemp = CYCLE::MovingToStation;  				// 지시사이클 - 이동 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// 좌우 우선방향
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

			// 시작 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// 목적지 STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// 모두 OFF 이면 주행Tag로 이동
			SEND_PACKET.BODY.MarkType = StrToInt(FeederGrid->Cells[GRD_STOP_TAG][iGrdIndex]);	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (item deleted!!)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 1;	// TeachingData_FinalPortType - EQ set  
																// ver 29-5 : bit field, ver 37 : unsigned char (Changed)			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_Left = 1;
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_RF = 1;
			SEND_PACKET.BODY.TeachingData_PIOInfo.PIO_CS0 = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;

			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(",");					// 콤마 위치탐색
					strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
					strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim();					// 마지막 원소 입력
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
             // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("작업지시22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//				SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			//frmViewLog->Log(strLog);
			// 다이후쿠 문서에 있는 실행모드 = 통상모드 는 어느건지 모르겠다

			// 데이터 전송
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다
			tmrRetry->Enabled = true;

			frmViewLog->Log("작업지시 - 이동사이클 (정상) 명령 송신");
		}
		break;

		// --------------- 작업지시 - 올림사이클 (정상)
		case Sim_Major_Cmd::ORD_UP_NORMAL:
		{
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);
		
			// OCS상태 초기화 - 이전값을 읽어온다
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 1;	// 올림 이재 허가
			SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 0;

			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// 지시 ID

			iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대

			iTemp = CYCLE::CraneUp;  						// 지시사이클 - 올림 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);
		
			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// 좌우 우선방향
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = StrToInt(FeederGrid->Cells[GRD_DRIVING][iGrdIndex]);		// Driving flag

			// 시작 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// 목적지 STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// 모두 OFF 이면 주행Tag로 이동
			SEND_PACKET.BODY.MarkType = 1;	// fixed value // StopTag = 1 Setting


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (Protocol deleted)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 2; // TeachingData_FinalPortType - EQ set
															 // = PortType_2 = 1 Set Value!!
			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// = TeachingData_LookDownLevel.LKDown_1 = 1;
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// PIOTimeLevel.PIOTime_1 = 1;
			
			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;
		
			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(","); 				// 콤마 위치탐색
					strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
					strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim(); 				// 마지막 원소 입력
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
			 // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("작업지시22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//	SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			//frmViewLog->Log(strLog);
			// 다이후쿠 문서에 있는 실행모드 = 통상모드 는 어느건지 모르겠다

			// 데이터 전송
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다

			frmViewLog->Log("작업지시 - 올림사이클 (정상) 명령 송신");
		}		
		break;

		// --------------- 작업지시 - 내림사이클 (정상)
		case Sim_Major_Cmd::ORD_DN_NORMAL:
		{
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);
		
			// OCS상태 초기화 - 이전값을 읽어온다
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// 작업지시

			// --- Status
			// 상태지시 - 기동 ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 0;
			SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 1;	// 내림 이재 허가

			// 정지지시, 정지요인, 유지보수지시, E/M지시, 데이터 전송지시  - 설정 생략

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// 지시 ID

			iTemp = ORDERDIV::JobOrder; 					// 지시구분 - 작업지시
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// 지시구분

			// 이적재 Buffer 위치
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// 제1 수하대	// unsigned char -> bit field changed!

			iTemp = CYCLE::CraneDown; 						// 지시사이클 - 내림 사이클
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);
		
			// 우선도
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7비트니까
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// 좌우 우선방향
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;
			
			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = StrToInt(FeederGrid->Cells[GRD_DRIVING][iGrdIndex]);		// Driving flag

			// 시작 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// 목적 NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// 목적지 STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// 모두 OFF 이면 주행Tag로 이동
			SEND_PACKET.BODY.MarkType = 1;	// MarkType.StopTag = 1;// fixed value


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (protocol deleted)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 2; // FinalPortType.PortType_2 = 1; // TeachingData_FinalPortType - EQ set
			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// LookDownLevel.LKDown_1 = 1
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// PIOTimeLevel.PIOTime_1 = 1
			
			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;
		
			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(","); 				// 콤마 위치탐색
					strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
					strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim(); 				// 마지막 원소 입력
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
			 // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("작업지시22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//	SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			frmViewLog->Log(strLog);
			// 다이후쿠 문서에 있는 실행모드 = 통상모드 는 어느건지 모르겠다

			// 데이터 전송
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// 상태값을 저장해둔다

			frmViewLog->Log("작업지시 - 내림사이클 (정상) 명령 송신");
		}
		break;
    }

	return iReturn;
}

//---------------------------------------------------------------------------
// 이 함수는 명령을 보내야 하는 것인지, 받아야 하는 것인지 판단한다
// OCS가 보내야 하는 경우 true 리턴, 아니면 false 리턴
bool TfrmAutoJob::IsSendCmd(int iGrdIndex)
{
	if((FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "MCP_TO_CLW_STATUS")
	   || (FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER"))
		return true;
	else
		return false;
}
//---------------------------------------------------------------------------

void TfrmAutoJob::GetRunningCmd(int iGrdIndex, int& iMajor, int& iMinor)
{
	// Major Cmd
	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "MOVE_ON")
		iMajor = Sim_Major_Cmd::MOVE_ON;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "MOVE_OFF")
		iMajor = Sim_Major_Cmd::MOVE_OFF;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_MOVE_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_MOVE_NORMAL;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_UP_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_UP_NORMAL;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_DN_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_DN_NORMAL;

	// Minor Cmd
	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "MCP_TO_CLW_STATUS")
		iMinor = TagID::MCP_TO_CLW_STATUS;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "CLW_TO_MCP_STATUS")
		iMinor = TagID::CLW_TO_MCP_STATUS;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER")
		iMinor = TagID::JOB_ORDER;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER_RESPONSE")
		iMinor = TagID::JOB_ORDER_RESPONSE;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_PROGRESS_REPORT")
		iMinor = TagID::JOB_PROGRESS_REPORT;
}


//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnRunClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= true;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RUN, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btbPauseClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= false;
	btnResume->Enabled	= true;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::PAUSE, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnResumeClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= true;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::CONTINUE, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnStopClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= true;
	btbPause->Enabled 	= false;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= false;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::STOP, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::Button1Click(TObject *Sender)
{
	// AnsiString strTmp = " 12,3,456,2,AC,B";
	AnsiString strTmp = " 12";
	AnsiString strDim[100];
	int iCnt = 0;

	if(strTmp.Trim().Length() != 0)
	{
		AnsiString strWork;
		int iPos = 0;

		while(strTmp.AnsiPos(",") != 0)
		{
			iPos = strTmp.AnsiPos(",");		// 콤마 위치탐색
			strWork = strTmp.SubString(0, (iPos - 1));	// 토큰 1개 추출
			strDim[iCnt++] = strWork.Trim();			// 정리하여 수납
			strTmp = strTmp.Delete(1,iPos);
		}

		strDim[iCnt++] = strTmp.Trim();					// 마지막 원소 입력
	}
}

//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrCmdSendTimer(TObject *Sender)
{
	tmrCmdSend->Enabled = false;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RUN, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::chkAutoOcsStatusClick(TObject *Sender)
{
	if(chkAutoOcsStatus->Checked == true)
		tmrOcsStatus->Enabled = true;
	else
		tmrOcsStatus->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrOcsStatusTimer(TObject *Sender)
{
	static int iCount = 0;
	int iReturn;

	if((btnRun->Enabled == false) && (btbPause->Enabled == true))
	{
		iCount++;

		if(iCount == 10)
		{
			iCount = 0;

			// 데이터 전송
			int iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);	// (1) 일단 Endian 변환을 해 놓고
			iReturn = frmUDP->UnicastSend((char*)&PKT_OCS_STATUS, iPacketSize);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);	// (2) 사용후 다시 되돌려 놔야 한다

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
			}
		}

		prgAutoSend->Position = iCount;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnSaveClick(TObject *Sender)
{
	JobSaveAsDialog->FileName = CurrentFileName.c_str();
    JobSaveAsDialog->Options.Clear();
    JobSaveAsDialog->Options << ofOverwritePrompt;
    JobSaveAsDialog->DefaultExt = "prg";
    JobSaveAsDialog->Filter = "Job Program files (*.prg)|*.prg|All files (*.*)|*.*";
    JobSaveAsDialog->FilterIndex = 1; // start the dialog showing all files

	JobSaveAsDialog->InitialDir = CurrentFileName.SubString(1, CurrentFileName.LastDelimiter("\\")-1).c_str();

	if(JobSaveAsDialog->Execute())
	{
		FILE *stream;
		CurrentFileName = JobSaveAsDialog->FileName.c_str();

		// 파일 확장자 추가
		if(CurrentFileName.Pos(".") == 0) CurrentFileName = CurrentFileName + ".prg";

		// 이전 파일명과 같은 경우 자동백업
		if(FileExists(CurrentFileName.c_str()))
		{
			UnicodeString BackUpFile = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "bak";
			UnicodeString OrgFile = CurrentFileName;
			CopyFile(OrgFile.c_str(), BackUpFile.c_str(), false);
		}

		// 하위 폼에게 파일 저장 지시
		UnicodeString strPasspermit  = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "pmt";
		frmPasspermitOCS->SaveFile(strPasspermit);

		UnicodeString strPassConfirm = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "con";
		frmOcsPassConfirm->SaveFile(strPassConfirm);

		// 파일에 저장
		try
		{
			stream = fopen(CurrentFileName.c_str(), "w+");
			if(stream == NULL)
			{
				ShowMessage(L"저장할 파일의 Open실패!");
				return;
			}

			edtFileName->Text = CurrentFileName;	// 현재 파일명 표시

			AnsiString strTmp[GRID_MAX_COL];

			// 헤더 기록 (분기방향, 통과허가, 개방요구, 아이템 갯수)
			fprintf(stream, "%d %d %d %d\r\n", rdoTrunDefault->ItemIndex, rdoGrpPass->ItemIndex, rdoGrpOpenReq->ItemIndex, StrToInt(edtCount->Text));
			// OCS 상태보고
			if(chkAutoOcsStatus->Checked)
				fprintf(stream, "%d\r\n", 1);
			else
				fprintf(stream, "%d\r\n", 0);

			// strTmp[0] = 0;
			for(int iRowCnt = 0;  iRowCnt < i_Cmd_Count; iRowCnt++)
			{
				for(int idx1 = 0; idx1 < GRID_MAX_COL; idx1++)
					strTmp[idx1] = FeederGrid->Cells[idx1][iRowCnt + 1];
				// strTmp[0] = strTmp[0] + 1;

				fprintf(stream,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\r\n"
								,strTmp[0],strTmp[1],strTmp[2],strTmp[3],strTmp[4]
								,strTmp[5],strTmp[6],strTmp[7],strTmp[8],strTmp[9]
								,strTmp[10],strTmp[11],strTmp[12],strTmp[13],strTmp[14]
								,strTmp[15],strTmp[16],strTmp[17]);
			}

			if(stream)
				fclose(stream);
			stream = NULL;
		}
		catch(...)
		{
			if(stream)
				fclose(stream);
			stream = NULL;
		}
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnLoadClick(TObject *Sender)
{
	FILE *fi;
	int iTmp[4];
	int iOcsSend;

	char strTmp[GRID_MAX_COL][1024];
	AnsiString m_strFileName;

	JobOpenDialog->InitialDir = GetCurrentDir(); // ProgramfilesPath.c_str();
	JobOpenDialog->Title = "Open a Job Program";
	JobOpenDialog->FileName = "";
	JobOpenDialog->Options.Clear();
	JobOpenDialog->Options << ofFileMustExist;
	JobOpenDialog->Filter = "Job program files (*.prg)|*.prg|All files (*.*)|*.*";
	JobOpenDialog->FilterIndex = 1; // start the dialog showing all files

	if (JobOpenDialog->Execute())
	{
		// 파일이 있나 확인
		if(!FileExists(JobOpenDialog->FileName.c_str()) || JobOpenDialog->FileName.Length() < 1)
		{
			Application->MessageBox(L"File not exist", L"File Error", MB_OK);
			return;
		}


		try
		{
			// 파일 오픈
			m_strFileName = JobOpenDialog->FileName;
			if( (fi = fopen( m_strFileName.c_str(), "r" )) == NULL )
			{
				ShowMessage(L"파일 열기가 실패했습니다");
				return;
			}

			edtFileName->Text = m_strFileName;	// 현재 파일명 표시

			GridClear();

			// 헤더 읽기
			fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);

			rdoTrunDefault->ItemIndex 	= iTmp[0];
			rdoGrpPass->ItemIndex 		= iTmp[1];
			rdoGrpOpenReq->ItemIndex 	= iTmp[2];

			// 명령어 갯수
			edtCount->Text 				= IntToStr(iTmp[3]);
			i_Cmd_Count					= iTmp[3];

			fscanf(fi,"%d \n", &iOcsSend);
			
			if(iOcsSend)
				chkAutoOcsStatus->Checked = true;
			else
				chkAutoOcsStatus->Checked = false;

			for(int iRowCnt = 0;  iRowCnt < iTmp[3]; iRowCnt++)
			{
				// 1 Line Read
				for (int iColCnt = 0; iColCnt < GRID_MAX_COL; iColCnt++)
				{
					fscanf(fi,"%s", &strTmp[iColCnt]); 
				}
				
				// fscanf(fi,"%s %s %s %s %s %s %s %s %s %s", &strTmp[0], &strTmp[1],&strTmp[2],&strTmp[3],&strTmp[4],&strTmp[5],&strTmp[6]);

				FeederGrid->RowCount = FeederGrid->RowCount + 1;

				for(int idx1 = 0; idx1 < GRID_MAX_COL; idx1++)
					FeederGrid->Cells[idx1][iRowCnt + 1] = strTmp[idx1];
			}

			fclose(fi);

			// --------- 다른 파일들 처리
			CurrentFileName = m_strFileName;

			UnicodeString strPasspermit  = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "pmt";
			frmPasspermitOCS->LoadFile(strPasspermit);

			UnicodeString strPassConfirm = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "con";
			frmOcsPassConfirm->LoadFile(strPassConfirm);
		}
		catch(...)
		{
			if(fi)
				fclose(fi);
			fi = NULL;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::FormClose(TObject *Sender, TCloseAction &Action)
{
	delete frmPasspermitOCS;
	delete frmOcsPassConfirm;

	delete frmViewLog;

	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnPassConfirmSetClick(TObject *Sender)
{
	ShowWindow(frmPasspermitOCS->Handle,SW_SHOW);
	frmPasspermitOCS->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnOpenResponseSetClick(TObject *Sender)
{
	ShowWindow(frmOcsPassConfirm->Handle,SW_SHOW);
	frmOcsPassConfirm->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::rdoGrpPassClick(TObject *Sender)
{
	frmPasspermitOCS->SetResponseMode(rdoGrpPass->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::rdoGrpOpenReqClick(TObject *Sender)
{
	frmOcsPassConfirm->SetResponseMode(rdoGrpOpenReq->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnViewStatusClick(TObject *Sender)
{
	ShowWindow(frmViewLog->Handle,SW_SHOW);
	frmViewLog->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::Button2Click(TObject *Sender)
{
			int iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);
			int iReturn = frmUDP->UnicastSend((char*)&PKT_OCS_STATUS, iPacketSize);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
			}
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrRetryTimer(TObject *Sender)
{
	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RETRY, Pkt, 0, 0);
}
//---------------------------------------------------------------------------


