//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OhtMain.h"
#include "About.h"
#include "Option.h"


#include "UDP.h"
#include "ClwStatusReport.h"
#include "ClwJobOrderResponse.h"
#include "ProgressReport.h"
#include "ClwPassConfirm.h"
#include "ClwPassConfirmRequest.h"
#include "ClwVersionResponse.h"
#include "ClwPassConfirmStatusReport.h"
#include "ClwDataTransperResponse.h"

#include "Oht2TP.h"
#include "PUtily.h"
#include "ClwDiagAnswer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOHTMain *frmOHTMain;

// 프로젝트 옵션 - C++ Compiler - Directorys and Conditionals - Conditional defines
// IS_MCP 가 있으면 MCP, 없으면 OHT로 보자
// _ONE_PC_TEST
	// 한 PC에서 테스트하려면 포트 하나를 둘이 열수 없으므로
	// OHT의 Listen 포트는 +2 하도록 하는 Project define이다.

//---------------------------------------------------------------------------
__fastcall TfrmOHTMain::TfrmOHTMain(TComponent* Owner)
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
}
//---------------------------------------------------------------------------
void __fastcall TfrmOHTMain::mnuAboutClick(TObject *Sender)
{
	AboutBox = new TAboutBox(Application);
    AboutBox->ShowModal();
	delete AboutBox;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuComLogClick(TObject *Sender)
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

void __fastcall TfrmOHTMain::mnuConfigClick(TObject *Sender)
{
	frmOption->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuCascadeClick(TObject *Sender)
{
	Cascade();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuTileHClick(TObject *Sender)
{
	TileMode = tbHorizontal;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuTileVClick(TObject *Sender)
{
	TileMode = tbVertical;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuServerStartClick(TObject *Sender)
{
	int iBr, iUr, iTP, iDiagBr, iDiagUr;
	AnsiString strBroadCastMask = BROADCAST_IP;

// ONE PC옵션을 위해 2016.08.11 전까지는 아래와 같이 설정했으나,
// 통신모듈 내에서 처리하는 것이 편리하다고 보고 아래와 같이 변경함
#if 0
	#ifdef	_ONE_PC_TEST
		iBr = frmUDP->pUdpObject->BroadcastBind((frmOption->iBroadCastPort + 2));
		iUr = frmUDP->pUdpObject->UnicastBind((frmOption->iControlPort + 2));

		frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
							 (frmOption->iBroadCastPort + 2),
							 (frmOption->iControlPort + 2));

		frmUDP->pUdpObject->SetBroadCastMask(strBroadCastMask);

		iTP = frmUDP->pUdpObject->OHT_TP_Bind();

		// 진단서버 관련 추가함
		// iDiagBr = frmUDP->pDiagObject->DIAG_BroadcastBind(ADDRESS_DIAG_GET_DEFAULT_PORT + 2);
		iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT + 2);
		//frmUDP->pDiagObject->DiagInfoUpdate(MACHINETYPE_DIAG_NORMAL_OHT, frmOption->strMachineID,
		//						(ADDRESS_DIAG_GET_DEFAULT_PORT + 2), (CONTROL_DIAG_DEFAULT_PORT + 2));

	#else
		iBr = frmUDP->pUdpObject->BroadcastBind(frmOption->iBroadCastPort);
		iUr = frmUDP->pUdpObject->UnicastBind(frmOption->iControlPort);
		iTP = frmUDP->pUdpObject->OHT_TP_Bind();

		frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
							 (frmOption->iBroadCastPort),
							 (frmOption->iControlPort));

		// 진단서버 관련 추가함
		// iDiagBr = frmUDP->pDiagObject->DIAG_BroadcastBind(ADDRESS_DIAG_GET_DEFAULT_PORT);
		iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT);

		//frmUDP->pDiagObject->DiagInfoUpdate(MACHINETYPE_DIAG_NORMAL_OHT, frmOption->strMachineID,
		//						(ADDRESS_DIAG_GET_DEFAULT_PORT), (CONTROL_DIAG_DEFAULT_PORT));
	#endif
#endif

	// 2016.08.11  아래와 같이 통일함
	iBr = frmUDP->pUdpObject->BroadcastBind(frmOption->iBroadCastPort);
	iUr = frmUDP->pUdpObject->UnicastBind(frmOption->iControlPort);
	iTP = frmUDP->pUdpObject->OHT_TP_Bind();

	frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
						 (frmOption->iBroadCastPort),
						 (frmOption->iControlPort));

	iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT);

	// 2016.05.19 서버들이 모두 실패한 경우에 한해 타이머를 켜지 않도록 논리 변경
	if((iBr != 0) && (iUr != 0) && (iTP != 0) && /*(iDiagBr != 0) && */ (iDiagUr != 0))
		ShowMessage(L"UDP 서버 시작이 실패했습니다!");
	else
		tmrUdpReceiveThread->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindMcp_AddrClick(TObject *Sender)
{
	// OCS의 주소를 찾기위해 Broad Cast한다
	frmUDP->pUdpObject->Address_Broadcast(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  frmOption->iBroadCastPort);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::tmrUdpReceiveThreadTimer(TObject *Sender)
{

// --------------------- 주소획득 프로토콜 처리 ------------------------
	// 여기의 포트번호는 상대방 포트번호. 1PC 환경에서도 OCS는 변동없음.
	frmUDP->pUdpObject->BroadCast_ThreadCall(frmOption->strMachineID, frmOption->iBroadCastPort);

	// 진단서버 프로토콜 처리
	// frmUDP->pDiagObject->DIAG_BroadCast_ThreadCall(frmOption->strMachineID, ADDRESS_DIAG_GET_DEFAULT_PORT);

// --------------------- 컨트롤  프로토콜 처리 ------------------------
	// frmUDP->UniCast_ThreadCall();
	PKT_OCS2OHT CmdPkt;
	int iResult;

	iResult = frmUDP->pUdpObject->UniCast_ThreadCall(CmdPkt);
        char cTemp[MACHINE_ID_LENGTH + 1];

	if(iResult > 0)
	{
        	memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
                cTemp[MACHINE_ID_LENGTH] = '\0';

		AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;
		strTmp.sprintf("상위 수신 [HEADER] MachineType : %d MachineID : %s Tag : %d ",
					CmdPkt.HEADER.MachineType, cTemp, CmdPkt.TAG.Value);

		switch(CmdPkt.TAG.Value)
		{
			case TagID::CMD_VERSIONREQ_OCS_OHT:
			{
				AnsiString strTmp;

				for(int i = 0; i < MDIChildCount; i++){
				
					strTmp = MDIChildren[i]->Name;
					if ( MDIChildren[i]->Name == "frmClwVersionRequest" ){
						ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
						MDIChildren[i]->BringToFront();
						return;
					}
				}
				
				frmClwVersionRequest = new TfrmClwVersionRequest(Application);

			}
			break;

			case TagID::CMD_DATATRANSFER_OCS2OHT:
			{
				PKT_DATA_TRANSPER_ORDER_OCS2OHT			CmdPkt_Up;
				memcpy(&CmdPkt_Up, &CmdPkt, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

				AnsiString strTmp;
			
				for(int i = 0; i < MDIChildCount; i++){
				
					strTmp = MDIChildren[i]->Name;
					if ( MDIChildren[i]->Name == "frmClwDataTransperResponse" ){
						ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
						MDIChildren[i]->BringToFront();						
						frmClwDataTransperResponse->Trigger(CmdPkt_Up);
						
						return;
					}
				}
				
				frmClwDataTransperResponse = new TfrmClwDataTransperResponse(Application);
				frmClwDataTransperResponse->Trigger(CmdPkt_Up);
				frmClwDataTransperResponse->BringToFront();
			}

			break;

			default:
			{
				// ShowMessage(strTmp);
			}
			break;
		}
	}
// --------------------- TP 프로토콜 처리 ------------------------
	// frmUDP->OHT_TP_ThreadCall();
	iResult = frmUDP->pUdpObject->OHT_TP_ThreadCall(CmdPkt);

	if(iResult > 0)
	{
		AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;
		strTmp.sprintf("TP 패킷수신 [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s",
					CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
					Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3));
		// ShowMessage(strTmp);
	}

// --------------------- 진단서버 프로토콜 처리 ------------------------
	PKT_OHT2DIAG DiagPkt;
	iResult = frmUDP->pDiagObject->DIAG_UniCast_ThreadCall(DiagPkt);

	if(iResult > 0)
	{
		// ShowMessage(L"진단 프로토콜 수신");
	}

// --------------------- 어시스턴스 프로토콜 처리 ----------------------
	PKT_OHT2OCS AS_PKT;
	iResult = frmUDP->pDiagObject->OHT_AS_ThreadCall(AS_PKT);
	// 필요하면 여기서 추가 처리
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuServerStopClick(TObject *Sender)
{
	tmrUdpReceiveThread->Enabled = false;

	frmUDP->pUdpObject->BroadcastClose();
	frmUDP->pUdpObject->UnicastClose();

	frmUDP->pUdpObject->OHT_TP_Close();

	// frmUDP->pDiagObject->DIAG_BroadcastClose();
	frmUDP->pDiagObject->UnicastClose();
}
//---------------------------------------------------------------------------

// OHT가 OCS로 정보를 보내는 동작.
void __fastcall TfrmOHTMain::mnuSendStatus_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwStatusReport = new TfrmClwStatusReport(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::FormActivate(TObject *Sender)
{
//	static bool bShowOnce = false;
//#ifdef	_ONE_PC_TEST
//	if (bShowOnce == false)
//	{
//		ShowMessage(L"One PC Test 모드입니다!\n\n주의하여 주십시오!");
//		bShowOnce = true;
//	}
//#endif
}

//---------------------------------------------------------------------------
/*
void TfrmOHTMain::Protocol_Receiver_JOB_ORDER(PKT_JOB_ORDER_ID_121 Pkt)
{
	AnsiString strTmp, strDisplay;

	strDisplay = strDisplay + strTmp.sprintf("HDR - Type : %d  ID : %s \n",
								Pkt.HEADER.MachineType, Pkt.HEADER.MachineID);

	strDisplay = strDisplay + strTmp.sprintf("TAG : %d \n", Pkt.TAG.Value);



	ShowMessage(strDisplay);
}
*/
//---------------------------------------------------------------------------
/*
void TfrmOHTMain::Protocol_Receiver_OCS_STATUS(PKT_OCS_STATUS_ID_01 Pkt)
{
	AnsiString strTmp, strDisplay;

	strDisplay = strDisplay + strTmp.sprintf("HDR - Type : %d  ID : %s \n",
								Pkt.HEADER.MachineType, Pkt.HEADER.MachineID);

	strDisplay = strDisplay + strTmp.sprintf("TAG : %d \n", Pkt.TAG.Value);



	ShowMessage(strDisplay);
}
*/
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::JobResponseID122Click(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmJobOrderResponse" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmJobOrderResponse = new TfrmJobOrderResponse(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuSendProgress_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmProgressReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmProgressReport = new TfrmProgressReport(Application);
}
//---------------------------------------------------------------------------

void TfrmOHTMain::StandardRead(PKT_OCS2OHT Pkt, int iMachineType)
{
	AnsiString strTmp;
	strTmp.sprintf("OCS 패킷 수신, 명령타입 : %d  머신타입 : %d" , Pkt.TAG.Value, iMachineType);
    // ShowMessage(strTmp);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuPassRequest_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirm" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirm = new TfrmClwPassConfirm(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::OHTID1Click(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirmRequest" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirmRequest = new TfrmClwPassConfirmRequest(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuPcTpTestClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmOht2TP" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOht2TP = new TfrmOht2TP(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuVersionResponseClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwVersionRequest" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwVersionRequest = new TfrmClwVersionRequest(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuOht2OcsPassConfirmStatusReportClick(TObject *Sender)

{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirmStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirmStatusReport = new TfrmClwPassConfirmStatusReport(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindDiag_AddrClick(TObject *Sender)
{
	// OCS의 주소를 찾기위해 Broad Cast한다
	/*
	frmUDP->pDiagObject->Address_Broadcast(MACHINETYPE_DIAG_NORMAL_OHT,
								  frmOption->strMachineID,
								  ADDRESS_DIAG_GET_DEFAULT_PORT);   */
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::test1Click(TObject *Sender)
{
	tmrUdpReceiveThreadTimer(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuDiagAnswerClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "TfrmDiagAnswer" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmDiagAnswer = new TfrmDiagAnswer(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuOcsFileTransperRunClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwDataTransperResponse" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwDataTransperResponse = new TfrmClwDataTransperResponse(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindMcp_Addr_NoClrClick(TObject *Sender)
{
	frmUDP->pUdpObject->Address_Broadcast_NotClear(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  frmOption->iBroadCastPort);

}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mndDiagInfoSetClick(TObject *Sender)
{
	frmUDP->pDiagObject->SetDiagInfo("127.0.0.1", CONTROL_DIAG_DEFAULT_PORT, frmOption->strMachineID, MACHINETYPE_DIAG_NORMAL_OHT);
	// frmUDP->pDiagObject->SetDiagInfo("192.167.0.20", CONTROL_DIAG_DEFAULT_PORT, frmOption->strMachineID, MACHINETYPE_DIAG_NORMAL_OHT);
}
//---------------------------------------------------------------------------

