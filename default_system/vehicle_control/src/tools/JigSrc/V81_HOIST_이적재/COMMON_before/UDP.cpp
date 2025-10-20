//---------------------------------------------------------------------------

#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

// #include "MainUnit.h"
#include "UDP.h"



#include "PUtily.h"

#include "UdpFM.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmUDP *frmUDP;

//---------------------------------------------------------------------------
__fastcall TfrmUDP::TfrmUDP(TComponent* Owner)
	: TForm(Owner)
{
	pUdpFM = NULL;

	if (pUdpFM == NULL)
		pUdpFM = new UdpFM();
}
//---------------------------------------------------------------------------
void TfrmUDP::DisplayLog(String strLog)
{
	mBSLog->Lines->Add(strLog);
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::FormDestroy(TObject *Sender)
{
	if (pUdpFM != NULL)
		delete(pUdpFM);
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::btnLogDelClick(TObject *Sender)
{
	mBSLog->Lines->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TfrmUDP::FormCreate(TObject *Sender)
{
	// ShowWindow(this->Handle, SW_HIDE);
#ifdef _UDP_MODULE_CHILD
	frmUDP->FormStyle = Forms::fsMDIChild;
#else
	frmUDP->FormStyle = Forms::fsNormal;
#endif

}

//---------------------------------------------------------------------------

void __fastcall TfrmUDP::btnBCthreadClick(TObject *Sender)
{

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;
	int iResult;

#ifdef IS_AS
		PKT_OHT2OCS CmdPktAS;

		iResult = pUdpFM->FM_2_AS_ThreadCall(CmdPktAS);
#endif

#ifdef IS_FM
		PKT_OCS2OHT CmdPktFM;

		iResult = pUdpFM->AS_2_FM_ThreadCall(CmdPktFM);
#endif


}

void __fastcall TfrmUDP::chkOCS_Addr_GetTestClick(TObject *Sender)
{
	if(chkOCS_Addr_GetTest->Checked)
	{
	   ;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::tmrLogClearTimer(TObject *Sender)
{
	if(chkAutoLogClear->Checked)
		mBSLog->Lines->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmUDP::trmAddrRetryTestTimer(TObject *Sender)
{
	//btnBCthreadClick(NULL);
}
//---------------------------------------------------------------------------

