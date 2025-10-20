//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwDataTransperResponse.h"
#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmClwDataTransperResponse *frmClwDataTransperResponse;
//---------------------------------------------------------------------------
__fastcall TfrmClwDataTransperResponse::TfrmClwDataTransperResponse(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwDataTransperResponse::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwDataTransperResponse::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------

void TfrmClwDataTransperResponse::Trigger(PKT_DATA_TRANSPER_ORDER_OCS2OHT SEND_PACKET)
{

	edtOrderID->Text = IntToStr(SEND_PACKET.BODY.OrderID);

	int iTemp;
	memcpy(&iTemp, &SEND_PACKET.BODY.Request,  1);

	if(SEND_PACKET.BODY.Request.TestMode == 0x00)
	{
		rdoSendParam->ItemIndex = 0;

		if(iTemp & bit_0)	chkFtpOht_Req_0->Checked = true;
		if(iTemp & bit_1)	chkFtpOht_Req_1->Checked = true;
		if(iTemp & bit_2)	chkFtpOht_Req_2->Checked = true;

		if(iTemp & bit_3)	chkFtpOht_Req_3->Checked = true;
		if(iTemp & bit_4)	chkFtpOht_Req_4->Checked = true;
		if(iTemp & bit_5)	chkFtpOht_Req_5->Checked = true;
		if(iTemp & bit_6)	chkFtpOht_Req_6->Checked = true;
	}
	else
	{
		rdoSendParam->ItemIndex = 1;

		if(iTemp & bit_0)	chkOhtFtp_Req_0->Checked = true;
		if(iTemp & bit_1)	chkOhtFtp_Req_1->Checked = true;
    }


	// ip addr
	IpAddr_H->Text 	= IntToStr(SEND_PACKET.BODY.IpAddress[FOUR_BYTE_HIGH]);
	IpAddr_M1->Text = IntToStr(SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M1]);
	IpAddr_M2->Text = IntToStr(SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M2]);
	IpAddr_L->Text  = IntToStr(SEND_PACKET.BODY.IpAddress[FOUR_BYTE_LOW]);

	// user name
	AnsiString strTmp;
	char cTmp[257];

	memset(cTmp, '\0', 257);
	memcpy(cTmp, &SEND_PACKET.BODY.UserName, 16);
	strTmp = cTmp;
	edtUserName->Text = strTmp;

	// password
	memset(cTmp, '\0', 257);
	memcpy(cTmp, &SEND_PACKET.BODY.PassWord, 16);
	strTmp = cTmp;
	edtPassword->Text = strTmp;

	// version
	memset(cTmp, '\0', 257);
	memcpy(cTmp, &SEND_PACKET.BODY.Version, 32);
	strTmp = cTmp;
	edtVersion->Text = strTmp;

	// data send to
	memset(cTmp, '\0', 257);
	memcpy(cTmp, &SEND_PACKET.BODY.DataSendTo, 32);
	strTmp = cTmp;
	edtDataSendTo->Text = strTmp;


	// 화면에 표시후, 어시스턴스로 데이터를 넘겨주자
	int iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);
	frmUDP->pDiagObject->UnicastSend_OHT_2_AS((char*)&SEND_PACKET, iPacketSize);
}

//---------------------------------------------------------------------------


void __fastcall TfrmClwDataTransperResponse::rdoSendParamClick(TObject *Sender)
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

