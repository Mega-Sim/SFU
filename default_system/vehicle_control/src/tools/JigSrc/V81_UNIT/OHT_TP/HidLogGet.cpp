//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HidLogGet.h"
#include "OHTInfoUnit.h"
#include "Def_OHT.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmHidLogGet *frmHidLogGet;
//---------------------------------------------------------------------------
__fastcall TfrmHidLogGet::TfrmHidLogGet(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmHidLogGet::btnStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		ShowMessage(L"메뉴얼 상태에서만 사용 가능합니다!");
		return;
	}
	else
	{
		MainForm->timerStatusRequest->Enabled = false;

		// 여기서 기능 실행
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize;
		int iReturn, iTemp;
		AnsiString strMachineID = "TP001";

		// 이적재 명령 구조체
		iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
		PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

		// 구조체 초기화
		memset(&SEND_PACKET, 0, iPacketSize);

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

		SEND_PACKET.TAG.Value = TagID::CMD_TP_HID_LOG_SAVE_ORDER;
		// SEND_PACKET.BODY.iVar1 = 0x01;	// sound on

		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

		delete m_CMDControl;
	}

    Display_Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmHidLogGet::btnCloseClick(TObject *Sender)
{
	Hide();
}
//---------------------------------------------------------------------------

/*
	진척상황 : 1부터 시작. 1-응답. 2-완료
	에러코드 : 0 : No error.   1 : Serial Open실패
*/
#define HID_RESULT_RESPONSE	1	// 응답을 수신
#define HID_RESULT_COMPLETE 2	// 동작 완료

#define HID_ERROR_NO_ERR				0				// 동작중 에러없이 완료
#define HID_ERROR_SERIAL_CAN_T_OPEN 	420	// ERR_HID_OPEN	// 시리얼포트를 열지 못했음
#define HID_ERROR_SERIAL_WRITE			421	// ERR_HID_WRITE	// 시리얼 쓰기중 에러
#define HID_ERROR_SERIAL_TIMEOUT		333

void TfrmHidLogGet::Display_Result(int iVar1, int iVar2)
{
	// 인수1은 실행 결과
	switch(iVar1)
	{
		case HID_RESULT_RESPONSE:
				panCmd_Response->Color = clLime;
		break;

		case HID_RESULT_COMPLETE:
				panCmd_Response->Color = clLime;
				panCmd_Complete->Color = clLime;
		break;

		default:
				panCmd_Response->Color = clLime;
				panCmd_Complete->Color = clLime;
		break;
    }

	// 인수2는 에러코드
	if (iVar1 == HID_RESULT_COMPLETE)
	{
		switch(iVar2)
		{
			case HID_ERROR_NO_ERR:
				edtHelp->Text = "동작 정상 완료함";
			break;

			case HID_ERROR_SERIAL_CAN_T_OPEN:
				edtHelp->Text = "시리얼 포트 Open Fail";
			break;

			case HID_ERROR_SERIAL_WRITE:
				edtHelp->Text = "시리얼 포트 Write Fail";
			break;

			case HID_ERROR_SERIAL_TIMEOUT:
				edtHelp->Text = "Time Out";
			break;

			default:
				edtHelp->Text = "알수 없는 오류";
			break;
		}

		MainForm->timerStatusRequest->Enabled = true;
	}
}



void TfrmHidLogGet::Display_Clear(void)
{
	panCmd_Start->Color = clLime;
	panCmd_Response->Color = clWhite;
	panCmd_Complete->Color = clWhite;

	edtHelp->Text = "";
}
