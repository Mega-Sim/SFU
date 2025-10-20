//---------------------------------------------------------------------------

#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

// #include "MainUnit.h"
#include "UDP.h"


#ifdef IS_MCP
	#include "McpMain.h"	// 상위를 include 해야 하니까
#endif

#ifdef IS_OHT
	#include "OhtMain.h"
	#include "Option.h"
#endif

#ifdef IS_TP
	#include "MainUnit.h"
	#include "SocketUnit.h"
#endif


#include "PUtily.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmUDP *frmUDP;

//---------------------------------------------------------------------------
__fastcall TfrmUDP::TfrmUDP(TComponent* Owner)
	: TForm(Owner)
{
	pUdpObject = NULL;

	if (pUdpObject == NULL)
		pUdpObject = new UdpCom();

	// Diag
	pDiagObject = NULL;

	if (pDiagObject == NULL)
		pDiagObject = new UdpDiag();
}
//---------------------------------------------------------------------------
void TfrmUDP::DisplayLog(String strLog)
{
	mBSLog->Lines->Add(strLog);
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::FormDestroy(TObject *Sender)
{
	if (pUdpObject != NULL)
		delete(pUdpObject);

	// Diag
	if (pDiagObject != NULL)
		delete(pDiagObject);

}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::btnLogDelClick(TObject *Sender)
{
	mBSLog->Lines->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TfrmUDP::FormCreate(TObject *Sender)
{
	// ShowWindow(this->Handle, SW_HIDE);
#ifdef _UDP_MODULE_CHILD
	frmUDP->FormStyle = Forms::fsMDIChild;
#else
	frmUDP->FormStyle = Forms::fsNormal;
#endif
}

//---------------------------------------------------------------------------

void __fastcall TfrmUDP::btnBCthreadClick(TObject *Sender)
{

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;

#ifdef IS_OHT
	// --------------------- 주소획득 프로토콜 처리 ------------------------
		// 여기의 포트번호는 상대방 포트번호. 1PC 환경에서도 OCS는 변동없음.
		frmUDP->pUdpObject->BroadCast_ThreadCall(frmOption->strMachineID, frmOption->iBroadCastPort);

	// --------------------- 컨트롤  프로토콜 처리 ------------------------
		// frmUDP->UniCast_ThreadCall();
		PKT_OCS2OHT CmdPkt;
		int iResult;

		iResult = frmUDP->pUdpObject->UniCast_ThreadCall(CmdPkt);

		if(iResult > 0)
		{
			memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
			cTemp[MACHINE_ID_LENGTH] = '\0';
			strChk = cTemp;
			strChk.Trim();

			AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;
			strTmp.sprintf("상위 수신 [HEADER] MachineType : %d MachineID : %s Tag : %d ",
						CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value);
			ShowMessage(strTmp);
		}
	// --------------------- TP 프로토콜 처리 ------------------------
		// frmUDP->OHT_TP_ThreadCall();
		iResult = frmUDP->pUdpObject->OHT_TP_ThreadCall(CmdPkt);

		if(iResult > 0)
		{
			AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;

			memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
			cTemp[MACHINE_ID_LENGTH] = '\0';
			strChk = cTemp;
			strChk.Trim();

			strTmp.sprintf("TP 패킷수신 [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s",
						CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
						Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3));
			ShowMessage(strTmp);
		}
#endif

#ifdef IS_MCP
	// --------------------- 주소획득 프로토콜 처리 ------------------------
	#ifdef	_ONE_PC_TEST
		// 여기의 포트번호는 상대방 포트번호이므로, 1PC환경에서는 + 2 해줘야 한다
		// 이 포트번호 인수는 브로드케스트를 받고 바로 응답하기 위해 사용하는 것임
		frmUDP->pUdpObject->BroadCast_ThreadCall("MCP01", (ADDRESS_GET_DEFAULT_PORT + 2));
	#else
		frmUDP->pUdpObject->BroadCast_ThreadCall("MCP01", ADDRESS_GET_DEFAULT_PORT);
	#endif

	// --------------------- 컨트롤  프로토콜 처리 ------------------------
	frmUDP->pUdpObject->UniCast_ThreadCall();
#endif


#ifdef IS_TP
	pUdpObject->OHT_TP_ThreadCall();
#endif

}
//---------------------------------------------------------------------------

int TfrmUDP::UnicastSend(char* buf, int len)
{
	return pUdpObject->UnicastSend(buf, len);
}

//---------------------------------------------------------------------------

int TfrmUDP::DiagSend(char* buf, int len)
{
	return pDiagObject->UnicastSend(buf, len);
}
void __fastcall TfrmUDP::trmAddrRetryTestTimer(TObject *Sender)
{
#ifdef IS_OHT
	if(chkOHT_Addr_GetTest->Checked)
	{
		frmOHTMain->mnuClwFindMcp_Addr_NoClrClick(NULL);
	}
#endif

#ifdef IS_MCP
	if(chkOCS_Addr_GetTest->Checked)
	{
		frmMcpMain->mnuMcpFindClw_AddrClick(NULL);
	}
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::chkOCS_Addr_GetTestClick(TObject *Sender)
{
	if(chkOCS_Addr_GetTest->Checked)
	{

	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::tmrLogClearTimer(TObject *Sender)
{
	if(chkAutoLogClear->Checked)
		mBSLog->Lines->Clear();
}
//---------------------------------------------------------------------------

