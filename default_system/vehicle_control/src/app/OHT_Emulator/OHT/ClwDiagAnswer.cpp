//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwDiagAnswer.h"
#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDiagAnswer *frmDiagAnswer;
//---------------------------------------------------------------------------
__fastcall TfrmDiagAnswer::TfrmDiagAnswer(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagAnswer::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagAnswer::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagAnswer::btnMaintInfoRequestClick(TObject *Sender)
{
	int iPacketSize = sizeof(DIAG_PKT_INFO_DATA_RESPONSE);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_INFO_DATA_RESPONSE	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
	strTmp = "OHT001";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_INFO_DATA_RESPONSE;	// 통계정보보고 (일반 OHT -> 진단 Agent)

	switch(cbMaint->ItemIndex)
	{
		case 0:
			SEND_PACKET.BODY.Parameter = DiagParamID::RUNNING_DISTANCE;
		break;

		case 1:
			SEND_PACKET.BODY.Parameter = DiagParamID::STEERING_COUNT;
		break;

		case 2:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOAD_UNLOAD_COUNT;
		break;

		case 3:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOAD_UNLOAD_ROT_COUNT;
		break;

		case 4:
			SEND_PACKET.BODY.Parameter = DiagParamID::HOIST_COUNT;
		break;

		case 5:
			SEND_PACKET.BODY.Parameter = DiagParamID::HOIST_ROT_COUNT;
		break;

		case 6:
			SEND_PACKET.BODY.Parameter = DiagParamID::ROTATE_COUNT;
		break;

		case 7:
			SEND_PACKET.BODY.Parameter = DiagParamID::HAND_COUNT;
		break;

		case 8:
			SEND_PACKET.BODY.Parameter = DiagParamID::SHUTTER_COUNT;
		break;

		case 9:
			SEND_PACKET.BODY.Parameter = DiagParamID::RUN_NODE_BARCODE_READ_COUNT;
		break;

		case 10:
			SEND_PACKET.BODY.Parameter = DiagParamID::STATION_NODE_BARCODE_READ_COUNT;
		break;

		case 11:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOOK_DOWN_SENSOR_TIMES;
		break;

		case 12:
			SEND_PACKET.BODY.Parameter = DiagParamID::UBG_SENSOR_TIMES;
		break;

		case 13:
			SEND_PACKET.BODY.Parameter = DiagParamID::PBS_SENSOR_TIMES;
		break;
	}

	SEND_PACKET.BODY.Value = StrToInt(edtSummaryInfo->Text);


	Conv_DIAG_PKT_INFO_DATA_RESPONSE(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void __fastcall TfrmDiagAnswer::btnMaintInfoResetClick(TObject *Sender)
{
	int iPacketSize = sizeof(DIAG_PKT_DATA_RESET_RESPONSE);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_DATA_RESET_RESPONSE	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
	strTmp = "OHT001";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_DATA_RESET_RESPONSE;	// 통계정보 초기화 응답 (일반 OHT -> 진단 Agent)

	switch(cbMaint->ItemIndex)
	{
		case 0:
			SEND_PACKET.BODY.Parameter = DiagParamID::RUNNING_DISTANCE;
		break;

		case 1:
			SEND_PACKET.BODY.Parameter = DiagParamID::STEERING_COUNT;
		break;

		case 2:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOAD_UNLOAD_COUNT;
		break;

		case 3:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOAD_UNLOAD_ROT_COUNT;
		break;

		case 4:
			SEND_PACKET.BODY.Parameter = DiagParamID::HOIST_COUNT;
		break;

		case 5:
			SEND_PACKET.BODY.Parameter = DiagParamID::HOIST_ROT_COUNT;
		break;

		case 6:
			SEND_PACKET.BODY.Parameter = DiagParamID::ROTATE_COUNT;
		break;

		case 7:
			SEND_PACKET.BODY.Parameter = DiagParamID::HAND_COUNT;
		break;

		case 8:
			SEND_PACKET.BODY.Parameter = DiagParamID::SHUTTER_COUNT;
		break;

		case 9:
			SEND_PACKET.BODY.Parameter = DiagParamID::RUN_NODE_BARCODE_READ_COUNT;
		break;

		case 10:
			SEND_PACKET.BODY.Parameter = DiagParamID::STATION_NODE_BARCODE_READ_COUNT;
		break;

		case 11:
			SEND_PACKET.BODY.Parameter = DiagParamID::LOOK_DOWN_SENSOR_TIMES;
		break;

		case 12:
			SEND_PACKET.BODY.Parameter = DiagParamID::UBG_SENSOR_TIMES;
		break;

		case 13:
			SEND_PACKET.BODY.Parameter = DiagParamID::PBS_SENSOR_TIMES;
		break;
	}
	

	SEND_PACKET.BODY.SuccessCode = StrToInt(edtResponse->Text);


	Conv_DIAG_PKT_DATA_RESET_RESPONSE(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void __fastcall TfrmDiagAnswer::btnMaintLevelRequestClick(TObject *Sender)
{
	int iPacketSize = sizeof(DIAG_PKT_DATA_INFO_REQUEST);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_DATA_INFO_REQUEST	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
	strTmp = "OHT020";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_INFO_STATISTICS_REQUEST;	// 통계정보 요구 (일반 OHT -> 진단 Agent)

	Conv_DIAG_PKT_DATA_INFO_REQUEST(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------


void __fastcall TfrmDiagAnswer::btnAbnormalReportClick(TObject *Sender)
{
	int iPacketSize = sizeof(DIAG_PKT_ERROR_REPORT);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_ERROR_REPORT	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
	strTmp = "OHT020";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_INFO_ERROR_REPORT;	// 이상이력 보고 (일반 OHT -> 진단 Agent)

	SEND_PACKET.BODY.ErrorID = StrToInt(edtList->Text);
	SEND_PACKET.BODY.NodeID = StrToInt(edtErrorNode->Text);


	Conv_DIAG_PKT_ERROR_REPORT(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");


}
//---------------------------------------------------------------------------

