//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwPassConfirm.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmClwPassConfirm *frmClwPassConfirm;
//---------------------------------------------------------------------------
__fastcall TfrmClwPassConfirm::TfrmClwPassConfirm(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwPassConfirm::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------

void __fastcall TfrmClwPassConfirm::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmClwPassConfirm::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwPassConfirm::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241);
	int iReturn, iTemp;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_PASSPERMIT_OHT2OCS_ID_241	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = frmOption->iOpt_MachineType;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strMachineID.c_str(), frmOption->strMachineID.Length());

	// ---- Tag -------------------------------------------
	SEND_PACKET.TAG.Value = TagID::CMD_PASSPERMIT_OHT2OCS;	// 통과 허가 요구

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
	SEND_PACKET.BODY.PassPermit_CM.SQNum = StrToInt(edtOrderID->Text);				// 수신한 Command ID

	// Stop번지
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[HIGH_BYTE] = StrToInt(edtTargetStop_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[MID_BYTE]  = StrToInt(edtTargetStop_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[LOW_BYTE]  = StrToInt(edtTargetStop_L->Text);

	// Reset번지 Main
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[HIGH_BYTE] = StrToInt(edtResetMain_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[MID_BYTE]  = StrToInt(edtResetMain_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[LOW_BYTE]  = StrToInt(edtResetMain_L->Text);

	// Reset번지 Sub
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[HIGH_BYTE] = StrToInt(edtResetSub_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[MID_BYTE]  = StrToInt(edtResetSub_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[LOW_BYTE]  = StrToInt(edtResetSub_L->Text);

	// 통과대기시간
	if(chkManualPass->Checked == true)
	{
		SEND_PACKET.BODY.PassDelayTime[HIGH_BYTE] = 0;
		SEND_PACKET.BODY.PassDelayTime[MID_BYTE] = 0;
		SEND_PACKET.BODY.PassDelayTime[LOW_BYTE] = 0;
	}
	else
	{
		unsigned char cHigh, cMid, cLow;
		Int2Uchar3(StrToInt(edtPassWaitTime->Text), cHigh, cMid, cLow); 	// int 값을 3바이트로 분해

		SEND_PACKET.BODY.PassDelayTime[HIGH_BYTE] = cHigh;
		SEND_PACKET.BODY.PassDelayTime[MID_BYTE]  = cMid;
		SEND_PACKET.BODY.PassDelayTime[LOW_BYTE]  = cLow;
    }

	// ------ 패킷 전송 ---------------------------------
	Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(SEND_PACKET);
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
