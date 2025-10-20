//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DiagSvrCmd.h"
#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDiagSvrCmd *frmDiagSvrCmd;
//---------------------------------------------------------------------------
__fastcall TfrmDiagSvrCmd::TfrmDiagSvrCmd(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagSvrCmd::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagSvrCmd::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagSvrCmd::btnConnectCheckRequestClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_REQUEST);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_CONNECT_CHK_REQUEST	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_SVR;
	strTmp = "SDG-01";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::AGENT_TO_CLW_CONNECT_CHK_REQUEST;	// 연결 확인 요청 (진단 Agent -> EQ)

	Conv_DIAG_PKT_CONNECT_CHK_REQUEST(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void __fastcall TfrmDiagSvrCmd::btnMaintInfoRequestClick(TObject *Sender)
{
	// char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(DIAG_PKT_INFO_DATA_REQUEST);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_INFO_DATA_REQUEST	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_SVR;
	strTmp = "SDG-01";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_INFO_DATA_REQUEST;	// 통계정보보고 요청 (진단 Agent -> 일반 OHT)

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


	Conv_DIAG_PKT_INFO_DATA_REQUEST(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------



void __fastcall TfrmDiagSvrCmd::btnMaintInfoResetClick(TObject *Sender)
{
	// char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(DIAG_PKT_DATA_RESET_REQUEST);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_DATA_RESET_REQUEST	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_SVR;
	strTmp = "SDG-01";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_DATA_RESET_REQUEST;	// 통계정보 초기화 요청 (진단 Agent -> 일반 OHT)

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


	Conv_DIAG_PKT_DATA_RESET_REQUEST(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void __fastcall TfrmDiagSvrCmd::btnMaintInfoResponseClick(TObject *Sender)
{
	// char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(DIAG_PKT_DATA_INFO_RESPONSE);
	int iReturn, iTemp;
	AnsiString strTmp;

	// 패킷을 정의한 후 값을 써 넣는다.
	DIAG_PKT_DATA_INFO_RESPONSE	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_DIAG_SVR;
	strTmp = "SDG-01";
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, strTmp.c_str(), strTmp.Length());

	SEND_PACKET.HEADER.CommandID = DiagCmdID::DIAG_INFO_STATISTICS_RESPONSE;	// 통계정보 응답 (진단 Agent -> 일반 OHT)

	// 데이터 임의로 넣기
	SEND_PACKET.BODY.InfoData[0] = 100;
	SEND_PACKET.BODY.InfoData[1] = 200;
	SEND_PACKET.BODY.InfoData[2] = 300;
	SEND_PACKET.BODY.InfoData[3] = 400;
	SEND_PACKET.BODY.InfoData[4] = 500;

	SEND_PACKET.BODY.InfoData[5] = 600;
	SEND_PACKET.BODY.InfoData[6] = 700;
	SEND_PACKET.BODY.InfoData[7] = 800;
	SEND_PACKET.BODY.InfoData[8] = 900;
	SEND_PACKET.BODY.InfoData[9] = 1000;

	SEND_PACKET.BODY.InfoData[10] = 1100;
	SEND_PACKET.BODY.InfoData[11] = 1200;
	SEND_PACKET.BODY.InfoData[12] = 1300;
	SEND_PACKET.BODY.InfoData[13] = 1400;

	Conv_DIAG_PKT_DATA_INFO_RESPONSE(SEND_PACKET);

	iReturn = frmUDP->DiagSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

