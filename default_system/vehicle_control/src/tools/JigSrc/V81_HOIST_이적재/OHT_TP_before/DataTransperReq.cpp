//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DataTransperReq.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "StatusUnit.h"
#include "OHTInfoUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDataTransperReq *frmDataTransperReq;
//---------------------------------------------------------------------------
__fastcall TfrmDataTransperReq::TfrmDataTransperReq(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmDataTransperReq::btnSendClick(TObject *Sender)
{

	// 로그 보여주기 추가함. 진행상황을 표시해주자
	Reset_Display();				// 진행상태 막대를 클리어
	panCmd_Start->Color = clLime;   // 전송했음을 표시하자

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 전송지시  명령 구조체
	iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);
	PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	SEND_PACKET.TAG.Value = TagID::REQ_DATATRANSFER_OHT_TP;		// 데이터 전송 요구


	// OHTREQUEST
	if(rdoSendParam->ItemIndex == 0)	// Ftp -> OHT 로 전송
	{
		iTemp = 0;

	   if(((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust==ON)) &&
			((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust_Modify == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust_Modify ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust_Modify ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust_Modify==ON)))
			{

				String strMsg = NULL;
				strMsg.sprintf(L"조정용 Map/수정 상태에서 MapUpload를 진행하시겠습니까?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OKCANCEL) == IDOK )
				{
				  //EOF
				}else
				{
					return;
                }
            }

/*
		2017.03.13. 이전의 CheckBox 방식 코드임
		if(chkFtpOht_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkFtpOht_Req_1->Checked)   iTemp = iTemp + bit_1;
		if(chkFtpOht_Req_2->Checked)   iTemp = iTemp + bit_2;
		if(chkFtpOht_Req_3->Checked)   iTemp = iTemp + bit_3;

		if(chkFtpOht_Req_4->Checked)   iTemp = iTemp + bit_4;
		if(chkFtpOht_Req_5->Checked)   iTemp = iTemp + bit_5;
		if(chkFtpOht_Req_6->Checked)   iTemp = iTemp + bit_6;
*/

		// 2017.03.13. 새로운 라디오버튼 방식 코드로 바꿈
		if(rdoFtpOht_Req_0->Checked)   iTemp = iTemp + bit_0;	// 조정용 (다운로드)
		if(rdoFtpOht_Req_1->Checked)   iTemp = iTemp + bit_1; 	// 운용용 (다운로드)
		if(rdoFtpOht_Req_4->Checked)   iTemp = iTemp + bit_4; 	// 펌웨어 (다운로드)
		// ---------------------------------------------------


		memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

		//memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
		// ------

		// 마지막에 상위 비트를 써 넣는다. (먼저 쓰면 덮어써 질 것이므로)
		//SEND_PACKET.BODY.Request.TestMode = 0x00;
	}
	else
	{
		iTemp = 0;

		if((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust==ON))
			{
			// 2017.03.13. 새로운 라디오버튼 방식 코드로 바꿈
				if(rdoOhtFtp_Req_1->Checked)   iTemp = iTemp + bit_1;
				// ---------------------------------------------------

				iTemp = iTemp + bit_7;
				memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

				// memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
				// ------

				// 마지막에 상위 비트를 써 넣는다. (먼저 쓰면 덮어써 질 것이므로)
				//SEND_PACKET.BODY.Request.TestMode = 0x01;
				DataTranaper_Display("명령 전송 성공!");
			}
			else
			{
				DataTranaper_Display("조정용Map이 아니므로 Map 정보 UpLoad Fail");
				return;
            }
/*		2017.03.13. 이전의 CheckBox 방식 코드임
		if(chkOhtFtp_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkOhtFtp_Req_1->Checked)   iTemp = iTemp + bit_1;
*/

	}

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void TfrmDataTransperReq::Reset_Display(void)
{
	panCmd_Start->Color = clWhite;

	panCmd_0->Color = clWhite;
	panCmd_1->Color = clWhite;
	panCmd_2->Color = clWhite;
	panCmd_3->Color = clWhite;
}

void TfrmDataTransperReq::DataTranaper_Display(AnsiString strMsg)
{
	mmLog->Lines->Add(strMsg);
	Parse_Result(strMsg);
}


void __fastcall TfrmDataTransperReq::btnLogClearClick(TObject *Sender)
{
	mmLog->Lines->Clear();

	Reset_Display();
}
//---------------------------------------------------------------------------

void TfrmDataTransperReq::Parse_Result(AnsiString strMsg)
{

	AnsiString strCmd = "";		// 파싱하여 수신명령어 부분만 넣는다
	AnsiString strParam = "";	// 파싱하여 응답 파라미터 부분만 넣는다

	int iParam_Position = 0;	// 파라미터
	int iTrans_Response = 0;	// 전송 지시 응답
	int iTrans_Progress = 0;    // 전송 진행중 응답

	// Step 1 : 파라미터 추출
	strMsg = strMsg.Trim();
	iParam_Position = strMsg.Pos("0x");				// 0xAB의 시작 지점을 찾는다

	if(iParam_Position != 0)	   // 파라미터가 있으면
		strParam = strMsg.SubString(iParam_Position, 4); 	// 파라미터 획득

	// Step 2 : 명령 종류 추출
	iTrans_Response = strMsg.Pos("전송 지시 응답");
	iTrans_Progress = strMsg.Pos("전송 진척 보고");

	// Step 3 : 화면 표시
	if(iTrans_Response != 0)
	{
		panCmd_0->Color = clLime;
	}

	if(iTrans_Progress != 0)
	{
		if(strParam == "0x01")
			panCmd_1->Color = clLime;
		else if(strParam == "0x02")
			panCmd_2->Color = clLime;
		else
			panCmd_3->Color = clLime;;
	}
}
