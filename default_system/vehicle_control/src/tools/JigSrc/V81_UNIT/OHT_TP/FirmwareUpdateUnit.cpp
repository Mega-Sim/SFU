//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FirmwareUpdateUnit.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFirmwareUpdateForm *FirmwareUpdateForm;
//---------------------------------------------------------------------------
__fastcall TFirmwareUpdateForm::TFirmwareUpdateForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::btnClearClick(TObject *Sender)
{
	mmLog->Lines->Clear();

	Reset_Display();
}

void TFirmwareUpdateForm::CheckBoxVerify(int iCheck)
{
	switch(iCheck)
	{
		case CHK_PROTO:	// FTP -> OHT
		{
			// chkMapSet_Proto->Checked = false;
			chkMapSet_Run->Checked = false;
			chkMapSet_Export->Checked = false;
			chkFirmwareUpdate->Checked = false;
		}
		break;

		case CHK_RUN:
		{
			chkMapSet_Proto->Checked = false;
			// chkMapSet_Run->Checked = false;
			chkMapSet_Export->Checked = false;
			chkFirmwareUpdate->Checked = false;
		}
		break;

		case CHK_EXPORT:
		{
			chkMapSet_Proto->Checked = false;
			chkMapSet_Run->Checked = false;
			// chkMapSet_Export->Checked = false;
			chkFirmwareUpdate->Checked = false;
		}
		break;

		case CHK_FIRMWARE:
		{
			chkMapSet_Proto->Checked = false;
			chkMapSet_Run->Checked = false;
			chkMapSet_Export->Checked = false;
    	}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFirmwareUpdateForm::chkMapSet_ProtoClick(TObject *Sender)
{
	if(chkMapSet_Proto->Checked)
		CheckBoxVerify(CHK_PROTO);
}
//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::chkMapSet_RunClick(TObject *Sender)
{
	if(chkMapSet_Run->Checked)
		CheckBoxVerify(CHK_RUN);
}
//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::chkMapSet_ExportClick(TObject *Sender)
{
	if(chkMapSet_Export->Checked)
		CheckBoxVerify(CHK_EXPORT);
}

void __fastcall TFirmwareUpdateForm::chkFirmwareUpdateClick(TObject *Sender)
{
	if(chkFirmwareUpdate->Checked)
		CheckBoxVerify(CHK_FIRMWARE);
}
//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::btnUpdateClick(TObject *Sender)
{
	Reset_Display();
    panCmd_Start->Color = clLime;   // 전송했음을 표시하자

	if((chkMapSet_Proto->Checked == false) 	&&
	   (chkMapSet_Run->Checked == false) 	&&
	   (chkMapSet_Export->Checked == false) &&
	   (chkFirmwareUpdate->Checked == false))
	{
		ShowMessage("업데이트할 항목을 선택해 주십시오");
	}
	else
	{
		// 여기서 실행
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		
		char buf[MAX_PACKET_SIZE+1];
		int iPacketSize;
		int iReturn, iTemp;
		AnsiString strMachineID = "TP001";

		int iOrderID = GetOrderID();	// make order id
		
		// 이적재void __fastcall TFirmwareUpdateForm::chkFirmwareUpdateClick(TObject *Sender)
	
		iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);
		PKT_DATA_TRANSPER_ORDER_OCS2OHT	SEND_PACKET;
		
		// 구조체 초기화
		memset(&SEND_PACKET, 0, iPacketSize);
		
		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// 메모리 삭제
		memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
		
		
		// 이적재 명령 전송
		SEND_PACKET.TAG.Value = TagID::CMD_DATATRANSFER_TP_OHT;

		// Data Setting

		// (1) request info make 		
		if(chkMapSet_Export->Checked == false)	// FTP -> OHT	
		{
			iTemp = 0;
			
			if(chkMapSet_Proto->Checked == true)	iTemp = iTemp + bit_0;
			if(chkMapSet_Run->Checked == true)		iTemp = iTemp + bit_1;
			// if(chkFirmwareUpdate->Checked == true)			iTemp = iTemp + bit_6;	// FTP_CMDTYPE_AXIS_PARA
			if(chkFirmwareUpdate->Checked == true)			iTemp = iTemp + bit_4;	// FTP_CMDTYPE_APPLICATION

			memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);
		}
		else	// OHT -> FTP
		{
			iTemp = 0;

			if(chkMapSet_Export->Checked == true)	iTemp = iTemp + bit_1;
			iTemp = iTemp + bit_7;
			memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);
		}
		
		// (2) fill body data
	    // order id
	    SEND_PACKET.BODY.OrderID = iOrderID;

		// ip addr
	    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_HIGH] 	= StrToInt(IpAddr_H->Text);
	    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M1] 	= StrToInt(IpAddr_M1->Text);
	    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_M2] 	= StrToInt(IpAddr_M2->Text);
	    SEND_PACKET.BODY.IpAddress[FOUR_BYTE_LOW] 	= StrToInt(IpAddr_L->Text);		

		// addr --------------
		AnsiString strTmp;
		
		// user name
		strTmp = edtUserName->Text;
		memset(&SEND_PACKET.BODY.UserName, ASCII_SPACE, 16);				// 메모리 삭제
		if(strTmp.Length() > 16)
			memcpy(&SEND_PACKET.BODY.UserName, strTmp.c_str(), 16); 		// 16 바이트만 복사
		else
			memcpy(&SEND_PACKET.BODY.UserName, strTmp.c_str(), strTmp.Length());
		
		// password
		strTmp = edtPassword->Text;
		memset(&SEND_PACKET.BODY.PassWord, ASCII_SPACE, 16);				// 메모리 삭제
		if(strTmp.Length() > 16)
			memcpy(&SEND_PACKET.BODY.PassWord, strTmp.c_str(), 16); 		// 16 바이트만 복사
		else
			memcpy(&SEND_PACKET.BODY.PassWord, strTmp.c_str(), strTmp.Length());
		
		// version
		strTmp = edtVersion->Text;
		memset(&SEND_PACKET.BODY.Version, ASCII_SPACE, 32); 				// 메모리 삭제
		if(strTmp.Length() > 16)
			memcpy(&SEND_PACKET.BODY.Version , strTmp.c_str(), 32); 		// 32 바이트만 복사
		else
			memcpy(&SEND_PACKET.BODY.Version, strTmp.c_str(), strTmp.Length());
		
		// data send to
		strTmp = edtDataSendTo->Text;
		memset(&SEND_PACKET.BODY.DataSendTo, ASCII_SPACE, 256); 			// 메모리 삭제
		if(strTmp.Length() > 256)
			memcpy(&SEND_PACKET.BODY.DataSendTo , strTmp.c_str(), 256); 	// 256 바이트만 복사
		else
			memcpy(&SEND_PACKET.BODY.DataSendTo, strTmp.c_str(), strTmp.Length());


		// (3) Send --------------
		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
		
		delete m_CMDControl;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::btnVersionInfoGetClick(TObject *Sender)
{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_FWU_VERSION_REQ);
		delete m_CMDControl;
}
//---------------------------------------------------------------------------
void TFirmwareUpdateForm::FWU_Display(AnsiString strMsg)
{
	mmLog->Lines->Add(strMsg);
	Parse_Result(strMsg);
}


//---------------------------------------------------------------------------
void __fastcall TFirmwareUpdateForm::FormShow(TObject *Sender)
{
	FirmwareUpdateForm->Top = MainForm->Top + MainForm->Height;
	FirmwareUpdateForm->Left = MainForm->Left;
}
//---------------------------------------------------------------------------

void TFirmwareUpdateForm::SaveSetting()
{
	FILE *stream;
	AnsiString CurrentFileName;
	AnsiString strTmp[8];

	CurrentFileName = "AtSet.dat";

	try
	{
		stream = fopen(CurrentFileName.c_str(), "w+");

		if(stream == NULL)
		{
			ShowMessage(L"저장할 파일의 Open실패!");
			return;
		}

		fprintf(stream, "%d %d %d %d\r\n",
			chkMapSet_Proto->Checked, 	chkMapSet_Run->Checked,
			chkMapSet_Export->Checked, 	chkFirmwareUpdate->Checked);

		strTmp[0] =  IpAddr_H->Text;
		strTmp[1] =  IpAddr_M1->Text;
		strTmp[2] =  IpAddr_M2->Text;
		strTmp[3] =  IpAddr_L->Text;

		strTmp[4] =  edtUserName->Text;
		strTmp[5] =  edtPassword->Text;
		strTmp[6] =  edtVersion->Text;
		strTmp[7] =  edtDataSendTo->Text;

		fprintf(stream,"%s %s %s %s %s %s %s %s\r\n",
			strTmp[0], strTmp[1], strTmp[2], strTmp[3],
			strTmp[4], strTmp[5], strTmp[6], strTmp[7]);

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

void TFirmwareUpdateForm::LoadSetting()
{
	FILE *fi;
	char strTmp[8][255];
	int iTmp[4];
	AnsiString CurrentFileName;
	CurrentFileName = "AtSet.dat";

	if(!FileExists(CurrentFileName.c_str()))
	{
		return;
	}

	try
	{
		memset(&strTmp, 0, (8*255));

		if( (fi = fopen( CurrentFileName.c_str(), "r" )) == NULL )
		{
			ShowMessage(L"파일 열기가 실패했습니다");
			return;
		}

		fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);

		chkMapSet_Proto->Checked = iTmp[0];
		chkMapSet_Run->Checked = iTmp[1];
		chkMapSet_Export->Checked = iTmp[2];
		chkFirmwareUpdate->Checked = iTmp[3];

		fscanf(fi,"%s %s %s %s %s %s %s %s\r\n",
			&strTmp[0], &strTmp[1], &strTmp[2], &strTmp[3],
			&strTmp[4], &strTmp[5], &strTmp[6], &strTmp[7]);

		IpAddr_H->Text	= strTmp[0];
		IpAddr_M1->Text = strTmp[1];
		IpAddr_M2->Text = strTmp[2];
		IpAddr_L->Text	= strTmp[3];

		edtUserName->Text	= strTmp[4];
		edtPassword->Text	= strTmp[5];
		edtVersion->Text	= strTmp[6];
		edtDataSendTo->Text = strTmp[7];

		fclose(fi);
	}
	catch(...)
	{
		if(fi)
			fclose(fi);
		fi = NULL;
	}
}

//---------------------------------------------------------------------------


void __fastcall TFirmwareUpdateForm::btnSettingLoadClick(TObject *Sender)
{
	LoadSetting();
}
//---------------------------------------------------------------------------

void __fastcall TFirmwareUpdateForm::btnSettingSaveClick(TObject *Sender)
{
	SaveSetting();
}
//---------------------------------------------------------------------------

void __fastcall TFirmwareUpdateForm::FormCreate(TObject *Sender)
{
	LoadSetting();
}
//---------------------------------------------------------------------------


void TFirmwareUpdateForm::Reset_Display(void)
{
	panCmd_Start->Color = clWhite;

	panCmd_0->Color = clWhite;
	panCmd_1->Color = clWhite;
	panCmd_2->Color = clWhite;
	panCmd_3->Color = clWhite;
}

//---------------------------------------------------------------------------

void TFirmwareUpdateForm::Parse_Result(AnsiString strMsg)
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



//---------------------------------------------------------------------------


