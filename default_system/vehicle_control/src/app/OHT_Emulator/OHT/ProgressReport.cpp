//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ProgressReport.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmProgressReport *frmProgressReport;
//---------------------------------------------------------------------------
__fastcall TfrmProgressReport::TfrmProgressReport(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmProgressReport::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmProgressReport::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmProgressReport::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmProgressReport::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_JOB_PROGRESS_REPORT_ID_103);
	int iReturn, iTemp;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_JOB_PROGRESS_REPORT_ID_103	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = frmOption->iOpt_MachineType;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strMachineID.c_str(), frmOption->strMachineID.Length());

	// ---- Tag -------------------------------------------
	SEND_PACKET.TAG.Value = TagID::JOB_PROGRESS_REPORT;	// 작업지시

	// --- Status -----------------------------------------
	// SEND_PACKET.STATUS.WorkingStatus = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	iTemp = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.WorkingStatus, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboRunningStatus->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.RunningStatus, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboCarriage->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.Carriage, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboMaintananceState->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.MaintananceState, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboCarryType->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.CarryType, &iTemp, 1);

	// ---
	SEND_PACKET.STATUS.CurrentAddr[HIGH_BYTE] = StrToInt(edtCurAddr_H->Text);
	SEND_PACKET.STATUS.CurrentAddr[MID_BYTE] = StrToInt(edtCurAddr_M->Text);
	SEND_PACKET.STATUS.CurrentAddr[LOW_BYTE] = StrToInt(edtCurAddr_L->Text);

	SEND_PACKET.STATUS.StopNowAddr[HIGH_BYTE] = StrToInt(edtStopAddr_H->Text);
	SEND_PACKET.STATUS.StopNowAddr[MID_BYTE] = StrToInt(edtStopAddr_M->Text);
	SEND_PACKET.STATUS.StopNowAddr[LOW_BYTE] = StrToInt(edtStopAddr_L->Text);

	SEND_PACKET.STATUS.DestinationAddr[HIGH_BYTE] = StrToInt(edtDestAddr_H->Text);
	SEND_PACKET.STATUS.DestinationAddr[MID_BYTE] = StrToInt(edtDestAddr_M->Text);
	SEND_PACKET.STATUS.DestinationAddr[LOW_BYTE] = StrToInt(edtDestAddr_L->Text);

	iTemp = CboIndex_ToHex(cboTunDirection->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.TunDirection, &iTemp, 1);
	SEND_PACKET.STATUS.RunningDistance = StrToInt(edtRunningDistance->Text);

	// --- 실행중 사이클 처리
	iTemp = cboRunningCycle->ItemIndex;

	// (1) 콤보인덱스 0 ~ 9까지 처리
	if (iTemp < 10)
		;	// do nothing
	// (2) 콤보 인덱스 10 ~ 13까지. 인덱스 10이면 0x2C(44) 결과값이 나와야 함. 따라서 34 더한다
	else if((iTemp > 9) && (iTemp < 14))
		iTemp = iTemp + 34;		// 0x2C ~ 0x2F까지 처리를 위함
	// (3) 콤보 인덱스 14 ~ 끝까지. 인덱스 14가 0x21 나와야 함. 따라서 19 더한다
	else if (iTemp > 13)
		iTemp = iTemp + 19;		// 0x2C ~ 0x2F까지 처리를 위함

	memcpy(&SEND_PACKET.STATUS.RunningCycle, &iTemp, 1);

	// --- 대기 사이클 처리
	iTemp = cboWaitingCycle->ItemIndex;

	// (1) 콤보인덱스 0 ~ 9까지 처리
	if (iTemp < 10)
		;	// do nothing
	// (2) 콤보 인덱스 10 ~ 13까지. 인덱스 10이면 0x2C(44) 결과값이 나와야 함. 따라서 34 더한다
	else if((iTemp > 9) && (iTemp < 14))
		iTemp = iTemp + 34;		// 0x2C ~ 0x2F까지 처리를 위함
	// (3) 콤보 인덱스 14 ~ 끝까지. 인덱스 14가 0x21 나와야 함. 따라서 19 더한다
	else if (iTemp > 13)
		iTemp = iTemp + 19;		// 0x2C ~ 0x2F까지 처리를 위함

	// memcpy(&SEND_PACKET.STATUS.WaitingCycle, &iTemp, 1);
	// ---
	SEND_PACKET.STATUS.WaitingCycle = (unsigned char) iTemp;
	SEND_PACKET.STATUS.ErrorCode = StrToInt(edtErrorCode->Text);
	SEND_PACKET.STATUS.APSinalLevel = StrToInt(edtRadioWaveLevel->Text);
	// SEND_PACKET.STATUS.EmStatus = StrToInt(edtEmStatus->Text);

	iTemp = StrToInt(edtDataTransStatus->Text);
	memcpy(&SEND_PACKET.STATUS.DataTransStatus, &iTemp, 1);

	// --- Body -----------------------------------------
	SEND_PACKET.BODY.RcvCmdID = StrToInt(edtOrderID->Text);				// 수신한 Command ID
	SEND_PACKET.BODY.ProgressType = StrToInt(cboProgressSep->ItemIndex) + 1;	// 진척구분

	// 주행거리
	/*
	if(chkNotRun->Checked == true)
		SEND_PACKET.BODY.MoveDist = 0;
	else
		SEND_PACKET.BODY.MoveDist = StrToInt(edtPriority->Text);
	*/

	// ------ 패킷 전송 ---------------------------------
	Conv_PKT_JOB_PROGRESS_REPORT_ID_103(SEND_PACKET);
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
