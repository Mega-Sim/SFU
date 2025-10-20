//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Oht2TP.h"
#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOht2TP *frmOht2TP;
//---------------------------------------------------------------------------
__fastcall TfrmOht2TP::TfrmOht2TP(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmOht2TP::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOht2TP::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOht2TP::Button1Click(TObject *Sender)
{
		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize = sizeof(PKT_OHT2OCS);
		int iReturn, iTemp;

		PKT_OHT2OCS	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TAGCATEGORY::TAG_CATEGORY_OCSCOMMON;

		iReturn = frmUDP->pUdpObject->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			ShowMessage(L"PC TP의 주소가 등록되어 있지 않습니다");
		else if(iReturn == UNICAST_SEND_RESULT_FAIL)
			ShowMessage(L"데이터 전송 실패");
}
//---------------------------------------------------------------------------
void __fastcall TfrmOht2TP::Button2Click(TObject *Sender)
{
		int iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);
		int iReturn, iTemp;

		PKT_OHT2TP_RESPONSE	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TagID::CMD_STATUSREQ_TP_RESPONSE;

		// 임의의 테스트 값
		SEND_PACKET.OHT_STATUS.HID_Erec = StrToInt(Edit1->Text);
		SEND_PACKET.OHT_STATUS.HID_Edc = StrToInt(Edit2->Text);
		SEND_PACKET.OHT_STATUS.HID_Idc = StrToInt(Edit3->Text);
		SEND_PACKET.OHT_STATUS.HID_Ecap = StrToInt(Edit4->Text);
		SEND_PACKET.OHT_STATUS.HID_Kw = StrToInt(Edit5->Text);

		iReturn = frmUDP->pUdpObject->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			ShowMessage(L"PC TP의 주소가 등록되어 있지 않습니다");
		else if(iReturn == UNICAST_SEND_RESULT_FAIL)
			ShowMessage(L"데이터 전송 실패");
}
//---------------------------------------------------------------------------

void __fastcall TfrmOht2TP::Button3Click(TObject *Sender)
{
		int iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);
		int iReturn, iTemp;

		PKT_OHT2TP_RESPONSE	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TagID::CMD_INITREQ_TP_RESPONSE;

		// 임의의 테스트 값
		SEND_PACKET.OHT_STATUS.HID_Erec = StrToInt(Edit1->Text);
		SEND_PACKET.OHT_STATUS.HID_Edc = StrToInt(Edit2->Text);
		SEND_PACKET.OHT_STATUS.HID_Idc = StrToInt(Edit3->Text);
		SEND_PACKET.OHT_STATUS.HID_Ecap = StrToInt(Edit4->Text);
		SEND_PACKET.OHT_STATUS.HID_Kw = StrToInt(Edit5->Text);

		iReturn = frmUDP->pUdpObject->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			ShowMessage(L"PC TP의 주소가 등록되어 있지 않습니다");
		else if(iReturn == UNICAST_SEND_RESULT_FAIL)
			ShowMessage(L"데이터 전송 실패");
}
//---------------------------------------------------------------------------

void __fastcall TfrmOht2TP::tmrAutoStatusTimer(TObject *Sender)
{
	if(chkStatusSendAuto->Checked == true)
	{
		int iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);
		int iReturn, iTemp;

		PKT_OHT2TP_RESPONSE	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TagID::CMD_STATUSREQ_TP_RESPONSE;

		// 임의의 테스트 값
		SEND_PACKET.OHT_STATUS.HID_Erec = StrToInt(Edit1->Text);
		SEND_PACKET.OHT_STATUS.HID_Edc = StrToInt(Edit2->Text);
		SEND_PACKET.OHT_STATUS.HID_Idc = StrToInt(Edit3->Text);
		SEND_PACKET.OHT_STATUS.HID_Ecap = StrToInt(Edit4->Text);
		SEND_PACKET.OHT_STATUS.HID_Kw = StrToInt(Edit5->Text);

		iReturn = frmUDP->pUdpObject->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		{
			chkStatusSendAuto->Checked = false;
			tmrAutoStatus->Enabled = false;

			ShowMessage(L"PC TP의 주소가 등록되어 있지 않습니다");
		}
		else if(iReturn == UNICAST_SEND_RESULT_FAIL)
		{
			chkStatusSendAuto->Checked = false;
			tmrAutoStatus->Enabled = false;

			ShowMessage(L"데이터 전송 실패");
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmOht2TP::chkStatusSendAutoClick(TObject *Sender)
{
	if(chkStatusSendAuto->Checked == true)
		tmrAutoStatus->Enabled = true;
	else
		tmrAutoStatus->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOht2TP::Button4Click(TObject *Sender)
{
		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize = sizeof(PKT_OHT2OCS);
		int iReturn, iTemp;

		PKT_OHT2OCS	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
		SEND_PACKET.TAG.Value = TagID::JOB_ORDER_RESPONSE_TP;

		iReturn = frmUDP->pUdpObject->UnicastSend_OHT_2_TP((char*)&SEND_PACKET, iPacketSize);

		if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			ShowMessage(L"PC TP의 주소가 등록되어 있지 않습니다");
		else if(iReturn == UNICAST_SEND_RESULT_FAIL)
			ShowMessage(L"데이터 전송 실패");
}
//---------------------------------------------------------------------------
