//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OcsPassConfirmResponse.h"

// #include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOcsPassConfirmResponse *frmOcsPassConfirmResponse;
//---------------------------------------------------------------------------
__fastcall TfrmOcsPassConfirmResponse::TfrmOcsPassConfirmResponse(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsPassConfirmResponse::FormClose(TObject *Sender, TCloseAction &Action)
{
	// 현재 설정값 저장
	if(strCurrentFileName == "frmOcsPassConfirmResponse.SYS")
		SaveFile("frmOcsPassConfirmResponse.SYS");

	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsPassConfirmResponse::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsPassConfirmResponse::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsPassConfirmResponse::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244);
	int iReturn, iTemp;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::RESPONSE_PASSPERMITOPEN_OCS2OHT;		// 통과 허가 개방 요구 응답

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

	// --- Body
	SEND_PACKET.BODY.PassPermit_CM.SQNum = StrToInt(edtOrderID->Text);	// 시퀀스 넘버

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

	// 데이터 전송
        PKT_OCS2OHT	OhtCmdPkt;
        memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);


	Conv_PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244(SEND_PACKET);

	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void TfrmOcsPassConfirmResponse::Trigger(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243 Pkt)
{
	// Pkt 243 infomation display (body only)
	edtOrderID->Text = IntToStr(Pkt.BODY.PassPermit_CM.SQNum);

	edtTargetStop_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[HIGH_BYTE]);
	edtTargetStop_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[MID_BYTE]);
	edtTargetStop_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[LOW_BYTE]);

	edtResetMain_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[HIGH_BYTE]);
	edtResetMain_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[MID_BYTE]);
	edtResetMain_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[LOW_BYTE]);

	edtResetSub_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[HIGH_BYTE]);
	edtResetSub_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[MID_BYTE]);
	edtResetSub_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[LOW_BYTE]);


	if(chkAutoReply->Checked)
	{
		btnSendClick(NULL);
		mBSLog->Lines->Add(L"자동 전송되었습니다!");
	}
}

//---------------------------------------------------------------------------

void __fastcall TfrmOcsPassConfirmResponse::btnDelClick(TObject *Sender)
{
	mBSLog->Lines->Clear();
}
//---------------------------------------------------------------------------

void TfrmOcsPassConfirmResponse::SetButtonMode(bool bAutoModeChild)
{
	if(bAutoModeChild == true)
	{
		btnClose->Visible = false;
		btnHide->Visible  = true;
	}
	else
	{
		btnClose->Visible = true;
		btnHide->Visible  = false;
	}
}
//---------------------------------------------------------------------------

void TfrmOcsPassConfirmResponse::SetResponseMode(int iResponseType)
{
	if(iResponseType == 0)		// 항상 Ack
	{
		chkStatus_0->Checked = true;
		chkStatus_6->Checked = true;
		chkStatus_7->Checked = true;

		chkAutoReply->Checked = true;
	}
	else if(iResponseType == 1)	// 항상 Nak
	{
		chkStatus_0->Checked = false;
		chkStatus_6->Checked = false;
		chkStatus_7->Checked = false;

		chkAutoReply->Checked = true;
	}
	else	// 무응답
	{
		chkAutoReply->Checked = false;
    }
}

//---------------------------------------------------------------------------
void TfrmOcsPassConfirmResponse::SaveFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "개방허가 요구 Save : " + strFileName;
	ShowMessage(strMsg);  */

	FILE *stream;

	try
	{
		// --------------- 파일 열기 -----------------
		stream = fopen(strFileName.c_str(), "w+");
		if(stream == NULL)
		{
			ShowMessage(L"저장할 파일의 Open실패! - frmResponsePasspermitOCS");
			return;
		}

		// --------------- 내용 쓰기 -----------------
		// 1) 상태지시
		fprintf(stream, "%d %d %d %d %d %d %d %d \r\n",
				chkStatus_0->Checked, chkStatus_1->Checked, chkStatus_2->Checked, chkStatus_3->Checked,
				chkStatus_4->Checked, chkStatus_5->Checked, chkStatus_6->Checked, chkStatus_7->Checked);
		// 2) 정지지시
		fprintf(stream, "%d %d %d %d %d %d %d %d \r\n",
				chkStopOrder_0->Checked, chkStopOrder_1->Checked, chkStopOrder_2->Checked, chkStopOrder_3->Checked,
				chkStopOrder_4->Checked, chkStopOrder_5->Checked, chkStopOrder_6->Checked, chkStopOrder_7->Checked);

		// 3) 정지요인, 유지보수 지시, EM 지시, 데이터 전송지시, 통과허가 자동반응
		fprintf(stream, "%d %d %d %d %d \r\n",
				cboStopReason->ItemIndex, cboMaintananceOrder->ItemIndex, cboEmOder->ItemIndex,
				cboDataTransOrder->ItemIndex, chkAutoReply->Checked);

		// Body시작
		// 4) 시퀀스 넘버
		fprintf(stream, "%d \r\n", StrToInt(edtOrderID->Text));

		// 5) Stop 번지
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtTargetStop_H->Text), StrToInt(edtTargetStop_M->Text), StrToInt(edtTargetStop_L->Text));

		// 6) Reset번지 (Main)
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtResetMain_H->Text), StrToInt(edtResetMain_M->Text), StrToInt(edtResetMain_L->Text));

		// 7) Reset 번지 (Sub)
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtResetSub_H->Text), StrToInt(edtResetSub_M->Text), StrToInt(edtResetSub_L->Text));

		// 닫기
		if(stream)
			fclose(stream);
		stream = NULL;
	}
	catch(...)
	{
		if(stream)
			fclose(stream);
		stream = NULL;
	}
}

//---------------------------------------------------------------------------
void TfrmOcsPassConfirmResponse::LoadFile(AnsiString strFileName)
{
	/*
	AnsiString strMsg;
	strMsg = "개방허가 요구 Load : " + strFileName;
	ShowMessage(strMsg);
	*/
	int iTmp[8];
	int iVar;
	char strTmp[7][1024];

	FILE *fi;

	strCurrentFileName = strFileName;

	try
	{
		// --------------- 파일 열기 -----------------
		if( (fi = fopen( strFileName.c_str(), "r" )) == NULL )
		{
			// ShowMessage(L"파일 열기가 실패했습니다");
			return;
		}

		// --------------- 내용 읽기 -----------------
		// 1) 상태지시
		fscanf(fi,"%d %d %d %d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4], &iTmp[5], &iTmp[6], &iTmp[7]);
		chkStatus_0->Checked = iTmp[0];
		chkStatus_1->Checked = iTmp[1];
		chkStatus_2->Checked = iTmp[2];
		chkStatus_3->Checked = iTmp[3];
		chkStatus_4->Checked = iTmp[4];
		chkStatus_5->Checked = iTmp[5];
		chkStatus_6->Checked = iTmp[6];
		chkStatus_7->Checked = iTmp[7];

		// 2) 정지지시
		fscanf(fi,"%d %d %d %d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4], &iTmp[5], &iTmp[6], &iTmp[7]);
		chkStopOrder_0->Checked = iTmp[0];
		chkStopOrder_1->Checked = iTmp[1];
		chkStopOrder_2->Checked = iTmp[2];
		chkStopOrder_3->Checked = iTmp[3];
		chkStopOrder_4->Checked = iTmp[4];
		chkStopOrder_5->Checked = iTmp[5];
		chkStopOrder_6->Checked = iTmp[6];
		chkStopOrder_7->Checked = iTmp[7];


		// 3) 정지요인, 유지보수 지시, EM 지시, 데이터 전송지시, 통과허가 자동반응
		fscanf(fi,"%d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4]);
		cboStopReason->ItemIndex 		= iTmp[0];
		cboMaintananceOrder->ItemIndex	= iTmp[1];
		cboEmOder->ItemIndex 			= iTmp[2];
		cboDataTransOrder->ItemIndex 	= iTmp[3];
		chkAutoReply->Checked 			= iTmp[4];

		// Body시작
		// 4) 시퀀스 넘버
		fscanf(fi,"%d \n", &iVar);
		edtOrderID->Text = IntToStr(iVar);


		// 5) Stop 번지
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtTargetStop_H->Text = IntToStr(iTmp[0]);
		edtTargetStop_M->Text = IntToStr(iTmp[1]);
		edtTargetStop_L->Text = IntToStr(iTmp[2]);

		// 6) Reset번지 (Main)
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtResetMain_H->Text = IntToStr(iTmp[0]);
		edtResetMain_M->Text = IntToStr(iTmp[1]);
		edtResetMain_L->Text = IntToStr(iTmp[2]);

		// 7) Reset 번지 (Sub)
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtResetSub_H->Text = IntToStr(iTmp[0]);
		edtResetSub_M->Text = IntToStr(iTmp[1]);
		edtResetSub_L->Text = IntToStr(iTmp[2]);


		// 파일 닫기
		if(fi)
			fclose(fi);
		fi = NULL;

	}
	catch(...)
	{
		if(fi)
			fclose(fi);
		fi = NULL;
	}
}

//---------------------------------------------------------------------------

void __fastcall TfrmOcsPassConfirmResponse::btnHideClick(TObject *Sender)
{
	ShowWindow(this->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

