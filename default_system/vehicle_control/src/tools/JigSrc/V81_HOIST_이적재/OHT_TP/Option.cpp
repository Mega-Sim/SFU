//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>

#pragma hdrstop

#include "Option.h"
#include "Protocol_Define_Modify.h"
#include "OHTInfoUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOption *frmOption;
//---------------------------------------------------------------------------
__fastcall TfrmOption::TfrmOption(TComponent* Owner)
	: TForm(Owner)
{
	// -------------------- 디폴트 결정 --------------------
#ifdef IS_MCP
	iOpt_MachineType = MACHINETYPE_MCP;
	strMachineID = "MCP001";
	strOhtMachineID = "OHT001";
#endif

#ifdef IS_TP
	iOpt_MachineType = MACHINETYPE_MCP;
	strMachineID = "TP01";
#endif

#ifdef IS_OHT
	iOpt_MachineType = MACHINETYPE_VHL;
	strMachineID = "OHT001";
#endif

	iBroadCastPort = ADDRESS_GET_DEFAULT_PORT;
	iBroadCastTimeOut = DEFAULT_BROADCAST_TIMEOUT;

	iControlPort = CONTROL_DEFAULT_PORT;
	iControlTimeOut = DEFAULT_CONTROL_TIMEOUT;

	iAutoStatusSendTime = DEFAULT_STATUS_AUTOSEND_TIME;
	// -----------------------------------------------------
	LoadConfig();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOption::btnCloseClick(TObject *Sender)
{
	// iOpt_MachineType = cboMachineType->ItemIndex;
	if(rdoOCS->Checked == true)
	{
		iOpt_MachineType = MACHINETYPE_MCP;
	}
	else if(rdoOHT->Checked == true)
	{
        if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
    		iOpt_MachineType = MACHINETYPE_VHL_COMMON;
        else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
			iOpt_MachineType = MACHINETYPE_VHL_COMMON;
        else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_SERVICE)
    		iOpt_MachineType = MACHINETYPE_VHL_PATROL;
        else
    		iOpt_MachineType = MACHINETYPE_VHL_COMMON;
	}
	else
	{
		iOpt_MachineType = MACHINETYPE_TP;
	}

	strMachineID = edtMachineID->Text;
	strOhtMachineID = edtOhtMachineID->Text;

	iBroadCastPort = StrToInt(edtBcPort->Text);
	iBroadCastTimeOut = StrToInt(edtBcTimeOut->Text);

	iControlPort = StrToInt(edtCtPort->Text);
	iControlTimeOut = StrToInt(edtCpTimeOut->Text);

	iAutoStatusSendTime  = StrToInt(edtAutoStatusSendTime->Text);

	/* 이 부분에 파일 세이브가 들어가야 한다  */
	FILE *fo;
#ifdef IS_MCP
	AnsiString FileName = "C:\\CommOptionOCS.sys";
#endif

#ifdef IS_TP
	AnsiString FileName = "C:\\CommOptionTP.sys";
#endif

#ifdef IS_OHT
	AnsiString FileName = "C:\\CommOptionOHT.sys";
#endif

	fo = fopen(FileName.c_str(),"w");

	if(!fo)
	{
		ShowMessage("통신설정 파일을 생성할 수 없습니다!");
	}
	else
	{
		fprintf(fo, "%d %s %d %d %d %d %d %s", iOpt_MachineType, strMachineID,
					iBroadCastPort, iBroadCastTimeOut, iControlPort, iControlTimeOut,
					iAutoStatusSendTime, strOhtMachineID);
		fclose(fo);
	}

	Hide();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOption::FormShow(TObject *Sender)
{
	LoadConfig();
}
//---------------------------------------------------------------------------

void TfrmOption::LoadConfig()
{
	FILE *fi;

#ifdef IS_MCP
	AnsiString FileName = "C:\\CommOptionOCS.sys";
#endif

#ifdef IS_TP
	AnsiString FileName = "C:\\CommOptionTP.sys";
#endif

#ifdef IS_OHT
	AnsiString FileName = "C:\\CommOptionOHT.sys";
#endif

	fi = fopen(FileName.c_str(),"r");
	char buffer[MAX_PACKET_SIZE];
	char buffer2[MAX_PACKET_SIZE];

	if(!fi)
	{
		//ShowMessage("통신설정 파일이 없습니다. 디폴트 값이 적용됩니다!");
	}
	else
	{
		fscanf(fi,"%d %s  %d %d %d %d  %d %s", &iOpt_MachineType, buffer,
				&iBroadCastPort, &iBroadCastTimeOut, &iControlPort, &iControlTimeOut,
				&iAutoStatusSendTime, buffer2);
		fclose(fi);

		strMachineID = buffer;
		strOhtMachineID = buffer2;
	}
	// ---------------------------------------------------------------

	if (iOpt_MachineType == MACHINETYPE_MCP)
	{
		rdoOCS->Checked = true;
	}
    // VHL 기종 세분화로 인해 변경
    //	else if(iOpt_MachineType == MACHINETYPE_VHL)
    else if((iOpt_MachineType >= MACHINETYPE_VHL_COMMON) && (iOpt_MachineType <= MACHINETYPE_VHL_PATROL))
	{
		rdoOHT->Checked = true;
	}
	else
	{
		rdoTP->Checked = true;
	}


	// cboMachineType->ItemIndex = iOpt_MachineType;
	edtMachineID->Text = strMachineID;
	edtOhtMachineID->Text = strOhtMachineID;

	edtBcPort->Text = IntToStr(iBroadCastPort);
	edtBcTimeOut->Text = IntToStr(iBroadCastTimeOut);

	edtCtPort->Text = IntToStr(iControlPort);
	edtCpTimeOut->Text = IntToStr(iControlTimeOut);

	edtAutoStatusSendTime->Text = IntToStr(iAutoStatusSendTime);
}

