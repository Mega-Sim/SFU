//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JobOrderOCS.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmJobOrderOCS *frmJobOrderOCS;

#define DEF_PATH_INPUT	20 // 시뮬레이션시 넣을수 있는 최대 경로 수
//---------------------------------------------------------------------------
__fastcall TfrmJobOrderOCS::TfrmJobOrderOCS(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);

	FeederGrid->Cells[0][0] = "No";
	FeederGrid->Cells[1][0] = "디폴트 외 경로";

	FeederGrid->RowCount = (DEF_PATH_INPUT + 1);

	for(int i = 0; i < DEF_PATH_INPUT; i++)
		FeederGrid->Cells[0][i + 1] = IntToStr(i + 1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderOCS::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];



	int iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
	int iReturn, iTemp;
	unsigned char cHigh, cMid, cLow;

	// 패킷을 정의한 후 값을 써 넣는다.
	PKT_JOB_ORDER_ID_121	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// 작업지시

        // int iPacketSize2 = sizeof(SEND_PACKET.STATUS);

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
	SEND_PACKET.BODY.CmdID = StrToInt(edtOrderID->Text);

	iTemp = CboIndex_ToHex((unsigned char)cboCmdDiv->ItemIndex);
	memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboCmdCycle->ItemIndex);
	memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

	iTemp = StrToInt(edtCurAddr_H->Text);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = (unsigned char)iTemp;

	// StopNodeID
	Int2Uchar3((unsigned int)StrToInt (edtCurAddr_H->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopNodeID[MID_BYTE] = cMid;
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE] = cLow;

	// StopStationID
	Int2Uchar3((unsigned int)StrToInt (edtStopAddr_H->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.StopStationID[MID_BYTE] = cMid;
	SEND_PACKET.BODY.StopStationID[LOW_BYTE] = cLow;

	// NextNodeID
	/*
	iTemp = StrToInt(edtDestAddr_H->Text);
	SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = (unsigned char)iTemp;
	iTemp = StrToInt(edtDestAddr_M->Text);
	SEND_PACKET.BODY.NextNodeID[MID_BYTE] = (unsigned char)iTemp;
	iTemp = StrToInt(edtDestAddr_L->Text);
	SEND_PACKET.BODY.NextNodeID[LOW_BYTE] = (unsigned char)iTemp;
	*/

	// WorkCarryType
	// iTemp = CboIndex_ToHex((unsigned char)cboWorkCarryType->ItemIndex);
	// memcpy(&SEND_PACKET.BODY.WorkCarryType, &iTemp, 1);

	// 우선도
	iTemp = CboIndex_ToHex((unsigned char)cboRiv->ItemIndex);
	if(chkHotLot->Checked)
		iTemp = iTemp + bit_7;
	memcpy(&SEND_PACKET.BODY.Riv, &iTemp, 1);

	// 변환번지 정보
	iTemp = CboIndex_ToHex((unsigned char)cboChangeNode->ItemIndex);
	memcpy(&SEND_PACKET.BODY.ChangeNode, &iTemp, 1);

	// 작업 구분
	//iTemp = CboIndex_ToHex((unsigned char)cboFinalPortType->ItemIndex);
	//memcpy(&SEND_PACKET.BODY.TeachingData_FinalPortType, &iTemp, 1);

	// 선조향을 위한 Node ID
	/*
	iTemp = StrToInt(edtPreSteeringNodeID_H->Text);
	SEND_PACKET.BODY.PreSteeringNodeID[HIGH_BYTE] = (unsigned char)iTemp;
	iTemp = StrToInt(edtPreSteeringNodeID_M->Text);
	SEND_PACKET.BODY.PreSteeringNodeID[MID_BYTE] = (unsigned char)iTemp;
	iTemp = StrToInt(edtPreSteeringNodeID_L->Text);
	SEND_PACKET.BODY.PreSteeringNodeID[LOW_BYTE] = (unsigned char)iTemp; */

	// 이적재 Buffer 위치
	iTemp = (unsigned char)StrToInt (edtBufferType->Text);
	memcpy(&SEND_PACKET.BODY.BufferType, &iTemp, 1);

	// Node로부터 이적재 Tag까지 거리
	SEND_PACKET.BODY.TeachingData_Hoist = (unsigned char)StrToInt (edtTeachingData_Hoist->Text);
	SEND_PACKET.BODY.TeachingData_Shift = (unsigned char)StrToInt (edtTeachingData_Shift->Text);
	SEND_PACKET.BODY.TeachingData_Rotate = (unsigned char)StrToInt (edtTeachingData_Rotate->Text);

	// TEACHINGDATA_LOOKDOWN
	iTemp = CboIndex_ToHex((unsigned char)cboTeachingData_LookDownLevel->ItemIndex);
	memcpy(&SEND_PACKET.BODY.TeachingData_LookDownLevel, &iTemp, 1);

	//iTemp = CboIndex_ToHex((unsigned char)cboTeachingData_PIOInfo->ItemIndex);
	//memcpy(&SEND_PACKET.BODY.TeachingData_PIOInfo, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboTeachingData_PIOTimeLevel->ItemIndex);
	memcpy(&SEND_PACKET.BODY.TeachingData_PIOTimeLevel, &iTemp, 1);

	//iTemp = CboIndex_ToHex((unsigned char)cboTeachingData_Option->ItemIndex);
	//memcpy(&SEND_PACKET.BODY.TeachingData_Option, &iTemp, 1);

	if(rdoPathData_Def_Direction_Left->Checked)
	{
			SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;

	}
	else
	{
		SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;
	}

	if (chkDriving->Checked)
		SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;
	else
		SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 0;

	// 개시, 종료, 기본외 분기번지

	Int2Uchar3((unsigned int)StrToInt (edtPathData_StartNodeID->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

	Int2Uchar3((unsigned int)StrToInt (edtPathData_EndNodeID->Text), cHigh, cMid, cLow);
	SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
	SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
	SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

	SEND_PACKET.BODY.PathData_ExtraNum = (unsigned int)StrToInt (edtPathData_ExtraNum->Text);

	// 분기번지들의 등록
	int iCount = StrToInt (edtPathData_ExtraNum->Text);

	for(int i = 0; i < iCount; i++)
	{
		Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[1][i+1]), cHigh, cMid, cLow);

		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
		SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
	}

	  /*
		for (int j = 0; j < TempGOInfo.PathData_ExtraNum; j++)
		{
				for (int i = 0; i < 3; i++) {

					// Extra Node 비트연산   puting
					int Temp = tmpID121.PathData_ExtraID[i + (j * 3)];

					if (HIGH_BYTE == 0)
						TempGOInfo.PathData_ExtraID[j] += Temp << ((2 - i) * 8);
					else
						TempGOInfo.PathData_ExtraID[j] += Temp << (i * 8);

				}
			}
			*/

	// 데이터 전송
        PKT_OCS2OHT	OhtCmdPkt;
        memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);

	Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);


	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderOCS::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderOCS::FormClose(TObject *Sender, TCloseAction &Action)

{
	if(strCurrentFileName == "frmJotOrderOCS.SYS")
		SaveFile("frmJotOrderOCS.SYS");

	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderOCS::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void TfrmJobOrderOCS::SaveFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "통과허가 요구 Save : " + strFileName;
	ShowMessage(strMsg);  */

	FILE *stream;

	try
	{
		// --------------- 파일 열기 -----------------
		stream = fopen(strFileName.c_str(), "w+");
		if(stream == NULL)
		{
			ShowMessage(L"저장할 파일의 Open실패! - frmJobOrderOCS");
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
		fprintf(stream, "%d %d %d %d \r\n",
				cboStopReason->ItemIndex, cboMaintananceOrder->ItemIndex, cboEmOder->ItemIndex,	cboDataTransOrder->ItemIndex);

		// Body시작
		// 4) 시퀀스 넘버
		fprintf(stream, "%d \r\n", StrToInt(edtOrderID->Text));

		// 5) 지시 구분
		fprintf(stream, "%d \r\n", cboCmdDiv->ItemIndex);

		// 6) 지시 사이클
		fprintf(stream, "%d \r\n", cboCmdCycle->ItemIndex);

		// 7) 목적지 노드, 목적지 스테이션
		fprintf(stream, "%d %d \r\n", StrToInt(edtCurAddr_H->Text), StrToInt(edtStopAddr_H->Text));

		// 8) 다음 노드
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtDestAddr_H->Text), StrToInt(edtDestAddr_M->Text), StrToInt(edtDestAddr_L->Text));

		// 9) 반송물 타입
		// fprintf(stream, "%d \r\n", cboWorkCarryType->ItemIndex);

		// 10) 우선도
		fprintf(stream, "%d %d \r\n", chkHotLot->Checked, cboRiv->ItemIndex);

		// 11) Change mode
		fprintf(stream, "%d \r\n", cboChangeNode->ItemIndex);

		// 12) final port type
		fprintf(stream, "%d \r\n", cboFinalPortType->ItemIndex);

		// 13) 선조향 노드 ID
		//fprintf(stream, "%d %d %d \r\n", StrToInt(edtPreSteeringNodeID_H->Text)
		//				, StrToInt(edtPreSteeringNodeID_M->Text), StrToInt(edtPreSteeringNodeID_L->Text));

		// 14) 이적재 Buffer 위치
		fprintf(stream, "%d \r\n", StrToInt(edtBufferType->Text));

		// 15) Node로부터 이적재 tag까지
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtTeachingData_Hoist->Text), StrToInt(edtTeachingData_Shift->Text),
										 StrToInt(edtTeachingData_Rotate->Text));

		// 16) Teaching Data 콤보 4개
		fprintf(stream, "%d %d %d %d \r\n",
					cboTeachingData_LookDownLevel->ItemIndex, cboTeachingData_PIOInfo->ItemIndex,
					cboTeachingData_PIOTimeLevel->ItemIndex,	cboTeachingData_Option->ItemIndex);

		// 17) 기본 분기방향
		fprintf(stream, "%d %d %d \r\n", chkDriving->Checked, rdoPathData_Def_Direction_Left->Checked,
										 rdoPathData_Def_Direction_Right->Checked);

		// 18) 개시번지 종료번지
		fprintf(stream, "%d %d \r\n", StrToInt(edtPathData_StartNodeID->Text), StrToInt(edtPathData_EndNodeID->Text));

		// 19) 기본외 분기번지 갯수
		fprintf(stream, "%d \r\n", StrToInt(edtPathData_ExtraNum->Text));

		// 20) 기본외 분기번지 쓰기
		int iExCnt = StrToInt(edtPathData_ExtraNum->Text);
		int iAddr;
		AnsiString strTmp;

		for(int i = 0; i < iExCnt; i++)
		{
			strTmp = FeederGrid->Cells[1][i + 1];
			strTmp = strTmp.Trim();

			if(strTmp == "")
			{
				strTmp.sprintf("기본외 분기번지 %d 행에 숫자가 아닌 문자가 있습니다. 확인해 주세요. \r\n 일단 0으로 변경해 둡니다", (i+1));
				ShowMessage(strTmp);
				iAddr = 0;
			}
			else
			{
				iAddr = StrToInt(FeederGrid->Cells[1][i + 1]);
			}

			fprintf(stream, "%d \r\n", iAddr);
		}

		// -------------------------- 닫기 -------------------------------
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
void TfrmJobOrderOCS::LoadFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "통과허가 요구 Load : " + strFileName;
	ShowMessage(strMsg);  */

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

		// 3) 정지요인, 유지보수 지시, EM 지시, 데이터 전송지시
		fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4]);
		cboStopReason->ItemIndex 		= iTmp[0];
		cboMaintananceOrder->ItemIndex	= iTmp[1];
		cboEmOder->ItemIndex 			= iTmp[2];
		cboDataTransOrder->ItemIndex 	= iTmp[3];

		// Body시작
		// 4) 시퀀스 넘버
		fscanf(fi,"%d \n", &iVar);
		edtOrderID->Text = IntToStr(iVar);

		// 5) 지시 구분
		fscanf(fi,"%d \n", &iVar);
		cboCmdDiv->ItemIndex = iVar;

		// 6) 지시 사이클
		fscanf(fi,"%d \n", &iVar);
		cboCmdCycle->ItemIndex = iVar;

		// 7) 목적지 노드, 목적지 스테이션
		fscanf(fi,"%d %d \n", &iTmp[0], &iTmp[1]);
		edtCurAddr_H->Text = IntToStr(iTmp[0]);
		edtStopAddr_H->Text = IntToStr(iTmp[1]);

		// 8) 다음 노드
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtDestAddr_H->Text = IntToStr(iTmp[0]);
		edtDestAddr_M->Text = IntToStr(iTmp[1]);
		edtDestAddr_L->Text = IntToStr(iTmp[2]);

		// 9) 반송물 타입
		// fscanf(fi,"%d \n", &iVar);
		// cboWorkCarryType->ItemIndex = iVar;

		// 10) 우선도 2개
		fscanf(fi,"%d %d \n", &iTmp[0], &iTmp[1]);
		chkHotLot->Checked = iTmp[0];
		cboRiv->ItemIndex = iTmp[1];

		// 11) Change mode
		fscanf(fi,"%d \n", &iVar);
		cboChangeNode->ItemIndex = iVar;

		// 12) final port type
		fscanf(fi,"%d \n", &iVar);
		cboFinalPortType->ItemIndex = iVar;

		// 13) 선조향 노드 ID
		/*
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtPreSteeringNodeID_H->Text = IntToStr(iTmp[0]);
		edtPreSteeringNodeID_M->Text = IntToStr(iTmp[1]);
		edtPreSteeringNodeID_L->Text = IntToStr(iTmp[2]);
		*/

		// 14) 이적재 Buffer 위치
		fscanf(fi,"%d \n", &iVar);
		edtBufferType->Text = IntToStr(iVar);

		// 15) Node로부터 이적재 tag까지
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtTeachingData_Hoist->Text 	= IntToStr(iTmp[0]);
		edtTeachingData_Shift->Text 	= IntToStr(iTmp[1]);
		edtTeachingData_Rotate->Text 	= IntToStr(iTmp[2]);


		// 16) Teaching Data 콤보 4개
		fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);
		cboTeachingData_LookDownLevel->ItemIndex 	= iTmp[0];
		cboTeachingData_PIOInfo->ItemIndex 			= iTmp[1];
		cboTeachingData_PIOTimeLevel->ItemIndex   	= iTmp[2];
		cboTeachingData_Option->ItemIndex 			= iTmp[3];

		// 17) 기본 분기방향
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		chkDriving->Checked 						= iTmp[0];
		rdoPathData_Def_Direction_Left->Checked 	= iTmp[1];
		rdoPathData_Def_Direction_Right->Checked 	= iTmp[2];

		// 18) 개시번지 종료번지
		fscanf(fi,"%d %d \n", &iTmp[0], &iTmp[1]);
		edtPathData_StartNodeID->Text 	= IntToStr(iTmp[0]);
		edtPathData_EndNodeID->Text 	= IntToStr(iTmp[1]);

		// 19) 기본외 분기번지 갯수
		fscanf(fi,"%d \n", &iVar);
		edtPathData_ExtraNum->Text 	= IntToStr(iVar);

		// 20) 기본외 분기번지 read
		int iAddr;
		for(int i = 0; i < iVar; i++)
		{
			fscanf(fi,"%d \n", &iAddr);
			FeederGrid->Cells[1][i + 1] = IntToStr(iAddr);
		}

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
