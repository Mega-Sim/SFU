//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "McpMain.h"
#include "About.h"
#include "Option.h"

#include "UDP.h"
#include "Protocol_Define_Modify.h"

// #include "ClwStatusReport.h"
#include "JobOrderOCS.h"
#include "OcsStatusReport.h"
#include "ResponsePasspermitOCS.h"
#include "OcsPassConfirmResponse.h"

#include "OcsVersionRequest.h"
#include "OcsDataTransperOrder.h"
#include "OcsInputRequestResponse.h"
#include "OcsFloorMoveOrder.h"

#include "AutoJob.h"
#include "DiagSvrCmd.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMcpMain *frmMcpMain;

// 프로젝트 옵션 - C++ Compiler - Directorys and Conditionals - Conditional defines
// IS_MCP 가 있으면 MCP, 없으면 OHT로 보자
// _ONE_PC_TEST
	// 한 PC에서 테스트하려면 포트 하나를 둘이 열수 없으므로
	// OHT의 Listen 포트는 +2 하도록 하는 Project define이다.

//---------------------------------------------------------------------------
__fastcall TfrmMcpMain::TfrmMcpMain(TComponent* Owner)
	: TForm(Owner)
{
	// 통신 모듈의 인스턴스 생성
/*
	for(int i = 0; i < MDIChildCount; i++){
		if ( MDIChildren[i]->Name == "frmUDP" ){
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmUDP = new TfrmUDP(Application);   */


#ifdef _K_DEMO
		// 이 기능은 교류회에서 사용했던 기능으로, 주소와 포트를 직접 지정하여 동작시키는 것이다.
        AnsiString strIpNum = BROADCAST_IP;
        AnsiString strBrAddr = IntToStr(ADDRESS_GET_DEFAULT_PORT);
        AnsiString strCtArrd = IntToStr(ADDRESS_GET_DEFAULT_PORT + 1);

        AnsiString strTitle;
        strTitle.sprintf("Mcp Emulator LongRun - 주소획득 IP Num : %s  주소획득 Port : %s  컨트롤 Port : %s", strIpNum, strBrAddr, strCtArrd);

		frmMcpMain->Caption = strTitle;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfrmMcpMain::mnuAboutClick(TObject *Sender)
{
	AboutBox = new TAboutBox(Application);
    AboutBox->ShowModal();
	delete AboutBox;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuComLogClick(TObject *Sender)
{
	// 통신모듈은 시작하고 바로 생성하는 것이므로 생성절차를 생략한다
	for(int i = 0; i < MDIChildCount; i++){
		if ( MDIChildren[i]->Name == "frmUDP" ){
			MDIChildren[i]->BringToFront();
			ShowWindow(frmUDP->Handle,SW_SHOW);
			return;
		}
	}

	//frmUDP = new TfrmUDP(Application);
	//frmUDP->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuConfigClick(TObject *Sender)
{
	frmOption->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuCascadeClick(TObject *Sender)
{
	Cascade();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuTileHClick(TObject *Sender)
{
	TileMode = tbHorizontal;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuTileVClick(TObject *Sender)
{
	TileMode = tbVertical;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuServerStartClick(TObject *Sender)
{
	int iBr, iUr, iDiagBr, iDiagUr;
	AnsiString strBroadCastMask = BROADCAST_IP;

	iBr = frmUDP->pUdpObject->BroadcastBind(frmOption->iBroadCastPort);
	iUr = frmUDP->pUdpObject->UnicastBind(frmOption->iControlPort);

	frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
						 (frmOption->iBroadCastPort),
						 (frmOption->iControlPort));


	frmUDP->pUdpObject->SetBroadCastMask(strBroadCastMask);

	// 진단서버 관련 추가함
	// iDiagBr = frmUDP->pDiagObject->DIAG_BroadcastBind(ADDRESS_DIAG_GET_DEFAULT_PORT);
	iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT);

	// frmUDP->pDiagObject->DiagInfoUpdate(MACHINETYPE_DIAG_SVR, frmOption->strMachineID,
	// 							(ADDRESS_DIAG_GET_DEFAULT_PORT), (CONTROL_DIAG_DEFAULT_PORT));

	if((iBr == 0) && (iUr == 0) &&  /* (iDiagBr == 0) && */ (iDiagUr == 0))
		tmrUdpReceiveThread->Enabled = true;
	else
		ShowMessage(L"UDP 서버 시작이 실패했습니다!");
}
//---------------------------------------------------------------------------

// 3.3.1. 	MCP로부터 IP주소를 통지하는 경우
void __fastcall TfrmMcpMain::mnuMcpFindClw_AddrClick(TObject *Sender)
{
#ifdef	_ONE_PC_TEST
	// 한 PC에서 테스트하려면 포트 하나를 둘이 열수 없으므로
	// OHT의 Listen 포트는 +2 하도록 하는 Project define이다.
	frmUDP->pUdpObject->Address_Broadcast(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  (frmOption->iBroadCastPort + 2));
#else
	// 정상 상황인 경우
	frmUDP->pUdpObject->Address_Broadcast(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  frmOption->iBroadCastPort);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::tmrUdpReceiveThreadTimer(TObject *Sender)
{

#ifdef	_ONE_PC_TEST
	// --------------------- 주소획득 프로토콜 처리 ------------------------
	// 여기의 포트번호는 상대방 포트번호이므로, 1PC환경에서는 + 2 해줘야 한다
	// 이 포트번호 인수는 브로드케스트를 받고 바로 응답하기 위해 사용하는 것임
	frmUDP->pUdpObject->BroadCast_ThreadCall("MCP01", (ADDRESS_GET_DEFAULT_PORT + 2));

	// 진단서버 주소획득 처리
	// frmUDP->pDiagObject->DIAG_BroadCast_ThreadCall("SDG-01", (ADDRESS_DIAG_GET_DEFAULT_PORT + 2));
#else
	frmUDP->pUdpObject->BroadCast_ThreadCall("MCP01", ADDRESS_GET_DEFAULT_PORT);

	// 진단서버 주소획득 처리
	// frmUDP->pDiagObject->DIAG_BroadCast_ThreadCall("SDG-01", (ADDRESS_DIAG_GET_DEFAULT_PORT));
#endif

// --------------------- 컨트롤  프로토콜 처리 ------------------------
	frmUDP->pUdpObject->UniCast_ThreadCall();

	// 진단서버 프로토콜 처리
	frmUDP->pDiagObject->DIAG_UniCast_ThreadCall();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuServerStopClick(TObject *Sender)
{
	tmrUdpReceiveThread->Enabled = false;
	frmUDP->pUdpObject->BroadcastClose();
	frmUDP->pUdpObject->UnicastClose();

	// frmUDP->pDiagObject->DIAG_BroadcastClose();
	frmUDP->pDiagObject->UnicastClose();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuMcpOrderAddress_DropClick(TObject *Sender)
{
	frmUDP->pUdpObject->Address_DropOrder_ToClw();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuMcpOrder_ControlGetMsgClick(TObject *Sender)
{
	frmUDP->pUdpObject->Address_DropOrder_AllClw();
}
//---------------------------------------------------------------------------

// OHT가 상태를 올려보내는 패킷을 수신하면 여기로 들어온다
/*
void TfrmMcpMain::Protocol_Receiver_OHT_STATUS(PKT_STATUS_REPORT_ID_03 Pkt)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			frmClwStatusReport->DisplayInform(Pkt);

			return;
		}
	}

	frmClwStatusReport = new TfrmClwStatusReport(Application);
	frmClwStatusReport->DisplayInform(Pkt);
}
*/

// 패킷을 수신하는 것이므로 PKT_OHT2OCS 를 사용해야 한다 (즉 OHT가 보낸 거니까)
void TfrmMcpMain::StandardRead(PKT_OHT2OCS Pkt, int iMachineType, int iTagID, int iPacketSize)
{
	AnsiString strTmp;

	strTmp.sprintf("OHT 패킷 수신, 명령타입 : %d  머신타입 : %d" , Pkt.TAG.Value, iMachineType);
	// ShowMessage(strTmp);

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmAutoJob" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();			
			frmAutoJob->StatusMachine(Sim_Run_Cmd::RESPONSE, Pkt, iTagID, iPacketSize); 
			return;
		}
	}	

	switch(iTagID)
	{
		case TagID::CMD_PASSPERMIT_OHT2OCS:
				PKT_PASSPERMIT_OHT2OCS_ID_241			CmdPkt_241;
				memcpy(&CmdPkt_241, &Pkt, iPacketSize);
				Protocol_Receiver_OHT_PASSPERMIT(CmdPkt_241);
		break;

		case TagID::CMD_PASSPERMITOPEN_OHT2OCS:
				PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243	CmdPkt_243;
				memcpy(&CmdPkt_243, &Pkt, iPacketSize);
				Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt_243);
		break;

		default:
			;	// do nothing
		break;
	}
}
//---------------------------------------------------------------------------

// CLW가 OCS로 통과 허가 요구 -> OCS는 폼을 띄우거나 자동 반응한다  (ID 241)
void TfrmMcpMain::Protocol_Receiver_OHT_PASSPERMIT(PKT_PASSPERMIT_OHT2OCS_ID_241 Pkt)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmResponsePasspermitOCS" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			frmResponsePasspermitOCS->SetButtonMode(false);
			frmResponsePasspermitOCS->Trigger(Pkt);

			return;
		}
	}

	frmResponsePasspermitOCS = new TfrmResponsePasspermitOCS(Application);
	frmResponsePasspermitOCS->SetButtonMode(false);
	frmResponsePasspermitOCS->Trigger(Pkt);
}


//---------------------------------------------------------------------------

// CLW가 OCS로 통과 허가 개방요구 -> OCS는 폼을 띄우거나 자동 반응한다  (ID 243)
void TfrmMcpMain::Protocol_Receiver_OHT_PASS_OPEN_PERMIT(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243 Pkt)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmOcsPassConfirmResponse" ){
			frmOcsPassConfirmResponse->SetButtonMode(false);
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			frmOcsPassConfirmResponse->Trigger(Pkt);

			return;
		}
	}

	frmOcsPassConfirmResponse = new TfrmOcsPassConfirmResponse(Application);
	frmOcsPassConfirmResponse->SetButtonMode(false);
	frmOcsPassConfirmResponse->Trigger(Pkt);
}

//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::FormActivate(TObject *Sender)
{
//#ifdef	_ONE_PC_TEST
//	ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
//#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::JobOrder_ID121Click(TObject *Sender)
{
	AnsiString strTmp;

	// ------------------------------------------------
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmAutoJob" ){
			ShowMessage(L"Auto 메뉴 실행중에는 다른 메뉴를 실행하실 수 없습니다!");
			return;
		}
	}

	// ------------------------------------------------
	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;

		if ( MDIChildren[i]->Name == "frmJobOrderOCS" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmJobOrderOCS = new TfrmJobOrderOCS(Application);
	frmJobOrderOCS->LoadFile("frmJotOrderOCS.SYS");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuOcsStatusClick(TObject *Sender)
{
	AnsiString strTmp;

	// ------------------------------------------------
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmAutoJob" ){
			ShowMessage(L"Auto 메뉴 실행중에는 다른 메뉴를 실행하실 수 없습니다!");
			return;
		}
	}
	// ------------------------------------------------

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;

		if ( MDIChildren[i]->Name == "frmOcsStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsStatusReport = new TfrmOcsStatusReport(Application);
	frmOcsStatusReport->LoadFile("frmOcsStatusReport.SYS");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMcpMain::mnuOcsResponseOhtPassClick(TObject *Sender)
{
	AnsiString strTmp;

	// ------------------------------------------------
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmAutoJob" ){
			ShowMessage(L"Auto 메뉴 실행중에는 다른 메뉴를 실행하실 수 없습니다!");
			return;
		}
	}
	// ------------------------------------------------

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;

		if ( MDIChildren[i]->Name == "frmResponsePasspermitOCS" ){
			frmResponsePasspermitOCS->SetButtonMode(false);
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmResponsePasspermitOCS = new TfrmResponsePasspermitOCS(Application);
	frmResponsePasspermitOCS->SetButtonMode(false);
	frmResponsePasspermitOCS->LoadFile("frmResponsePasspermitOCS.SYS");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuOcsResponseOhtPassResponseClick(TObject *Sender)
{
	AnsiString strTmp;

	// ------------------------------------------------
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmAutoJob" ){
			ShowMessage(L"Auto 메뉴 실행중에는 다른 메뉴를 실행하실 수 없습니다!");
			return;
			}
	}
	// ------------------------------------------------

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;

		if ( MDIChildren[i]->Name == "frmOcsPassConfirmResponse" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsPassConfirmResponse = new TfrmOcsPassConfirmResponse(Application);
	frmOcsPassConfirmResponse->LoadFile("frmOcsPassConfirmResponse.SYS");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuAutoJobClick(TObject *Sender)
{
	AnsiString strTmp;

	// Step 1. 불필요한 창 닫기
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if (MDIChildren[i]->Name == "frmAutoJob")
		{
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmAutoJob = new TfrmAutoJob(Application);
}
//---------------------------------------------------------------------------



void __fastcall TfrmMcpMain::mnuOcsVerReqClick(TObject *Sender)
{
	AnsiString strTmp;

	// Step 1. 불필요한 창 닫기
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if (MDIChildren[i]->Name == "frmOcsVersionRequest")
		{
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsVersionRequest = new TfrmOcsVersionRequest(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::OCSOHT1Click(TObject *Sender)
{
	AnsiString strTmp;

	// Step 1. 불필요한 창 닫기
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if (MDIChildren[i]->Name == "frmOcsDataTransperOrder")
		{
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsDataTransperOrder = new TfrmOcsDataTransperOrder(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuOhtInputRequestResponseClick(TObject *Sender)
{
	AnsiString strTmp;

	// Step 1. 불필요한 창 닫기
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if (MDIChildren[i]->Name == "frmOcsInputRequestResponse")
		{
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsInputRequestResponse = new TfrmOcsInputRequestResponse(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuFloorMoveOrderClick(TObject *Sender)
{
 	AnsiString strTmp;

	// Step 1. 불필요한 창 닫기
	for(int i = 0; i < MDIChildCount; i++){
		strTmp = MDIChildren[i]->Name;
		if (MDIChildren[i]->Name == "frmOcsFloorMoveOrder")
		{
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOcsFloorMoveOrder = new TfrmOcsFloorMoveOrder(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuDiagSvrFindClw_AddrClick(TObject *Sender)
{
#ifdef	_ONE_PC_TEST
	//frmUDP->pDiagObject->Address_Broadcast(MACHINETYPE_DIAG_SVR,
	//							  "SDG-01",
	//							  (ADDRESS_DIAG_GET_DEFAULT_PORT+2));
#else
	//frmUDP->pDiagObject->Address_Broadcast(MACHINETYPE_DIAG_SVR,
	//							  "SDG-01",
	//							  ADDRESS_DIAG_GET_DEFAULT_PORT);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::test1Click(TObject *Sender)
{
	tmrUdpReceiveThreadTimer(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuDiagOrderSendClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmDiagSvrCmd" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmDiagSvrCmd = new TfrmDiagSvrCmd(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMcpMain::mnuDiagInfoSetClick(TObject *Sender)
{
	frmUDP->pDiagObject->SetDiagInfo("127.0.0.1", CONTROL_DIAG_DEFAULT_PORT, "SDG-01", MACHINETYPE_DIAG_SVR);
}
//---------------------------------------------------------------------------

