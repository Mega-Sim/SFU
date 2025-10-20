//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OcsDataTransperOrder.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOcsDataTransperOrder *frmOcsDataTransperOrder;
//---------------------------------------------------------------------------
__fastcall TfrmOcsDataTransperOrder::TfrmOcsDataTransperOrder(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsDataTransperOrder::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsDataTransperOrder::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsDataTransperOrder::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);
	int iReturn, iTemp;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_DATA_TRANSPER_ORDER_OCS2OHT	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_DATATRANSFER_OCS2OHT;

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

	// ---- Body

	// OHTREQUEST
	if(rdoSendParam->ItemIndex == 0)	// Ftp -> OHT 로 전송
	{
		iTemp = 0;

		if(chkFtpOht_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkFtpOht_Req_1->Checked)   iTemp = iTemp + bit_1;
		if(chkFtpOht_Req_2->Checked)   iTemp = iTemp + bit_2;
		if(chkFtpOht_Req_3->Checked)   iTemp = iTemp + bit_3;

		if(chkFtpOht_Req_4->Checked)   iTemp = iTemp + bit_4;
		if(chkFtpOht_Req_5->Checked)   iTemp = iTemp + bit_5;
		if(chkFtpOht_Req_6->Checked)   iTemp = iTemp + bit_6;

		memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

		//memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
		// ------

		// 마지막에 상위 비트를 써 넣는다. (먼저 쓰면 덮어써 질 것이므로)
		//SEND_PACKET.BODY.Request.TestMode = 0x00;
	}
	else
	{
		iTemp = 0;

		if(chkOhtFtp_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkOhtFtp_Req_1->Checked)   iTemp = iTemp + bit_1;

		iTemp = iTemp + bit_7;
		memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

		// memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
		// ------

		// 마지막에 상위 비트를 써 넣는다. (먼저 쓰면 덮어써 질 것이므로)
		//SEND_PACKET.BODY.Request.TestMode = 0x01;
	}

    // order id
    SEND_PACKET.BODY.OrderID = StrToInt(edtOrderID->Text);

	// ip addr
    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_HIGH] 	= StrToInt(IpAddr_H->Text);
    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M1] 	= StrToInt(IpAddr_M1->Text);
    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M2] 	= StrToInt(IpAddr_M2->Text);
    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_LOW] 	= StrToInt(IpAddr_L->Text);

	AnsiString strTmp;

    // user name
	strTmp = edtUserName->Text;
	memset(&SEND_PACKET.BODY.UserName, ASCII_SPACE, 16);				// 메모리 삭제
    if(strTmp.Length() > 16)
		memcpy(&SEND_PACKET.BODY.UserName, strTmp.c_str(), 16);	  		// 16 바이트만 복사
    else
    	memcpy(&SEND_PACKET.BODY.UserName, strTmp.c_str(), strTmp.Length());

	// password
	strTmp = edtPassword->Text;
	memset(&SEND_PACKET.BODY.PassWord, ASCII_SPACE, 16);				// 메모리 삭제
    if(strTmp.Length() > 16)
		memcpy(&SEND_PACKET.BODY.PassWord, strTmp.c_str(), 16);	  		// 16 바이트만 복사
    else
    	memcpy(&SEND_PACKET.BODY.PassWord, strTmp.c_str(), strTmp.Length());

    // version
	strTmp = edtVersion->Text;
	memset(&SEND_PACKET.BODY.Version, ASCII_SPACE, 32);					// 메모리 삭제
	if(strTmp.Length() > 16)
		memcpy(&SEND_PACKET.BODY.Version , strTmp.c_str(), 32);	  		// 32 바이트만 복사
	else
		memcpy(&SEND_PACKET.BODY.Version, strTmp.c_str(), strTmp.Length());

	// data send to
	strTmp = edtDataSendTo->Text;
	memset(&SEND_PACKET.BODY.DataSendTo, ASCII_SPACE, 256);				// 메모리 삭제
	if(strTmp.Length() > 256)
		memcpy(&SEND_PACKET.BODY.DataSendTo , strTmp.c_str(), 256);	  	// 256 바이트만 복사
	else
		memcpy(&SEND_PACKET.BODY.DataSendTo, strTmp.c_str(), strTmp.Length());

// 데이터 전송
/*
    PKT_OCS2OHT	OhtCmdPkt;
	memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);
*/

	Conv_PKT_DATA_TRANSPER_ORDER_OCS2OHT(SEND_PACKET);


	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsDataTransperOrder::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsDataTransperOrder::rdoSendParamClick(TObject *Sender)
{
	if (rdoSendParam->ItemIndex == 0)
	{
		GroupBox6->Enabled = true;
		GroupBox7->Enabled = false;
	}
	else
	{
		GroupBox6->Enabled = false;
		GroupBox7->Enabled = true;
    }
}
//---------------------------------------------------------------------------

