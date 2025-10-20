//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OcsInputRequestResponse.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOcsInputRequestResponse *frmOcsInputRequestResponse;
//---------------------------------------------------------------------------
__fastcall TfrmOcsInputRequestResponse::TfrmOcsInputRequestResponse(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsInputRequestResponse::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsInputRequestResponse::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsInputRequestResponse::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OHT_INPUT_REQUEST_RESPONSE);
	int iReturn, iTemp;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_OHT_INPUT_REQUEST_RESPONSE	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::RESPONSE_INSERTREQ_OCS_OHT;

	// --- Status
	iTemp = 0;
	if(chkStatus_0->Checked)   iTemp = iTemp + bit_0;
	if(chkStatus_1->Checked)   iTemp = iTemp + bit_1;
	if(chkStatus_2->Checked)   iTemp = iTemp + bit_2;
	if(chkStatus_3->Checked)   iTemp = iTemp + bit_3;

	if(chkStatus_4->Checked)   iTemp = iTemp + bit_4;
	if(chkStatus_5->Checked)   iTemp = iTemp + bit_5;
	if(chkStatus_6->Checked)   iTemp = iTemp + bit_6;
	if(chkStatus_7->Checked)   iTemp = iTemp + bit_7;

	memcpy(&SEND_PACKET.STATUS.StatusOrder, &iTemp, 1);
	// ----
	iTemp = 0;
	if(chkStopOrder_0->Checked)   iTemp = iTemp + bit_0;
	if(chkStopOrder_1->Checked)   iTemp = iTemp + bit_1;
	if(chkStopOrder_2->Checked)   iTemp = iTemp + bit_2;
	if(chkStopOrder_3->Checked)   iTemp = iTemp + bit_3;

	if(chkStopOrder_4->Checked)   iTemp = iTemp + bit_4;
	if(chkStopOrder_5->Checked)   iTemp = iTemp + bit_5;
	if(chkStopOrder_6->Checked)   iTemp = iTemp + bit_6;
	if(chkStopOrder_7->Checked)   iTemp = iTemp + bit_7;

	memcpy(&SEND_PACKET.STATUS.StopOrder, &iTemp, 1);
	// ------

	SEND_PACKET.STATUS.StopReason = (unsigned char)cboStopReason->ItemIndex;

	iTemp = CboIndex_ToHex((unsigned char)cboMaintananceOrder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.MaintananceOrder, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboEmOder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.EmOder, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboDataTransOrder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.DataTransOrder, &iTemp, 1);

	// BODY
	SEND_PACKET.BODY.RailNumber = StrToInt(edtRailNumber->Text);


	// 데이터 전송
	/*
		PKT_OCS2OHT	OhtCmdPkt;
		memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);
	*/


	Conv_PKT_OHT_INPUT_REQUEST_RESPONSE(SEND_PACKET);


	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
