// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHTInfoUnit.h"
#include "ConnectUnit.h"
#include "MainUnit.h"
#include "SocketUnit.h"
#include "FileUnit.h"
#include "WarnningUnit.h"
#include "Utility.h"
#include "Define.h"
#include "Def_TP.h"
#include "OHT_TLV.h"
#include "AssistantUnit.h"

#include "UDP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConnectForm *ConnectForm;

// ---------------------------------------------------------------------------
__fastcall TConnectForm::TConnectForm(TComponent *Owner)
    :
    TForm(Owner)
{

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectNameClick(TObject *Sender)
{
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[ConnectName->ItemIndex];
	gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
}
// ---------------------------------------------------------------------------

/**
@brief   TP에서 OHT에 Connect버튼 클릭시 발생하는 함수
@author  puting
@date    2012.12.13
@param   SocketForm으로부터 연결 시도
*/
void __fastcall TConnectForm::btnConnectClick(TObject *Sender)
{
	// SocketForm->Assistant_DisConnect();

	MainForm->StatusBarVersionInfo->Panels->Items[0]->Text = "";

	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
	String strLog;
	strLog.sprintf(L"====> [CONNECT BUTTON] Click...%s [%s]", ConnectName->Text.Trim(), ConnectIP->Text.Trim());
	m_LogFile->WriteLogFile("Communication", strLog);
	delete m_LogFile;

//	이 부분을 새로운 UDP코드를 이용한다
	 // 기존 : SocketForm->ConnectOHT(ConnectIP->Text.Trim(), ConnectName->Text.Trim());

 ///////////////// Remote.ini 에 없는 호기 시에 TP 접속 불가///////////.
	int nItemCnt = ConnectName->Items->Count;     // 콤보박스 아이템 개수
	bool bSame = false;
	AnsiString    strInputTxt = ConnectName->Text.UpperCase();    // 사용자가 입력한 문자. 대소문자 비교 안하도록 대문자로 변경

    // 콤보박스 리스트들을 모두 검색하면서 사용자가 입력한 값과 일치하는게 있는지 비교
    // 사용자가 입력한 값이 콤보박스에 없다면, 의도한 호기명-IP 매칭이 아닐수 있기 때문에
	for(int i=0; i<nItemCnt ; i++)
    {
        // 콤보박스 리스트 중 1개 텍스트
        AnsiString strItemTxt = ConnectName->Items->Strings[i].UpperCase(); // 대소문자비교X
        if(strInputTxt == strItemTxt)
		{
           bSame = true;    // 일치하는 항목  찾음
           break;
		}
    }

    // 입력한 값과 일치하는 아이템이 없으면 Connect 하지 않고 에러메시지 만 띄운다.
    if(bSame == false)
    {
       MainForm->DataDisplay(COLOR_RED, "OHT 호기명 확인 필요");
	   return;
	}

   ///////////호기명 6글자 아니면 에러메세지 띄우면서 TP 접속 불가  ////////////

   int nTextLen = ConnectName->Text.Length();

   if(nTextLen !=6 )      // 호기명 글자 수 제한  6글자
   {
			//SocketForm->DisConnect();	// 아직 연결 안된상태기 때문에 Disconnect 탈 필요 없음
			Warnning_Display(COLOR_RED, "OHT 호기명을 확인해주세요..");
			return ;
   }
   else
   {
		MainForm->m_bIgnoreJungBokConnect = false;
		SocketForm->ConnectOHT(ConnectIP->Text.Trim(), OHT_S_TP_LISTEN_PORT_NUM ,
								TP_FIX_PORT_NUM, ConnectName->Text.Trim());




// ------- 여기부터 아래까지 추가 ----------------------------------------
 /*
	int iResult = frmUDP->pUdpObject->UnicastBind_TP(ConnectIP->Text.Trim(),
		 StrToInt(ConnectPort->Text.Trim()), TP_FIX_PORT_NUM);	// 새로운 통신모듈을 동작시킨다.

		if(iResult == -3)
			ShowMessage("TP용 UDP 수신포트가 이미 열려 있습니다!");
		else if(iResult == 0)
		{
			char buf[MAX_PACKET_SIZE+1];
			int iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);
			int iReturn, iTemp;
	
			PKT_STATUS_REPORT_ID_03	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어
	
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			SEND_PACKET.TAG.Value = TpCmdID::TP_ADDRESS_REGISTER;
	
			iReturn = frmUDP->pUdpObject->UnicastSend((char*)&SEND_PACKET, iPacketSize);  // TP주소 등록요청
	
			ShowMessage("TP용 UDP 수신포트 bind 성공");
		}
		else
			ShowMessage("TP용 UDP 수신포트 bind 실패! 포트를 확인해 주세요");
	
		MainForm->tmrUdpListen->Enabled = true;		// UDP 통신포트 수신시작
	*/
	
	// -------------------------------------------------------------------------
	
		if(SocketForm->AssistantSocket->Active == true)
		{
			SocketForm->AssistantSocket->Active = false;
		}
	
		int nError = 0;
	

	
		//SocketForm->AssistantConnect(ConnectIP->Text.Trim(), ConnectName->Text.Trim());
	
		nError = FileForm->FileControlStart(ConnectIP->Text.Trim());
	
	

	}

	//2022.05.11 sseung 처음 연결시 Default가 운영자모드
	gm_OHTStatus->nProgram_Mode = MODE_WORKER;
	MainForm->panUser->Caption = "운영자";
}
// ---------------------------------------------------------------------------

/**
@brief   TP에서 OHT에 Cancel버튼 클릭시 발생하는 함수
@author  puting
@date    2012.12.13
*/
void __fastcall TConnectForm::btnCancelClick(TObject *Sender)
{
    SocketForm->DisConnect();
    
    Close();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuAddYESClick(TObject *Sender)
{
    int i;

    gm_OHTConnectInfo->nCount++;
    gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nCount - 1] = ConnectName->Text;
    // Label1->Caption = ConnectName->Items->Strings[0];//ConnectName->ItemIndex];
    // Label1->Caption = ConnectName->Text;
    gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1] = ConnectIP->Text;
    gm_OHTConnectInfo->RemoteFileSave();
    ConnectName->Clear();
    for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
    {
        ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);
    }
    ConnectName->ItemIndex = gm_OHTConnectInfo->nCount - 1;
    gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
    ConnectIP->Text           = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1];

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuEditYESClick(TObject *Sender)
{
	int i;

	gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nIndex] = ConnectName->Text;
	gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex]   = ConnectIP->Text;
	gm_OHTConnectInfo->RemoteFileSave();
	ConnectName->Clear();


	for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
	 {
	 ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);

	  }
	  ConnectName->ItemIndex = gm_OHTConnectInfo->nIndex;
	  ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex];


}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuDeleteYESClick(TObject *Sender)
{
	int i;
    for(i = gm_OHTConnectInfo->nIndex; i < gm_OHTConnectInfo->nCount; i++)
    {
		gm_OHTConnectInfo->strOHTName[i] = gm_OHTConnectInfo->strOHTName[i + 1];
        gm_OHTConnectInfo->strOHTIP[i]   = gm_OHTConnectInfo->strOHTIP[i + 1];
    }
    gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nCount - 1] = "";
    gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1] = "";
    gm_OHTConnectInfo->nCount--;

    gm_OHTConnectInfo->RemoteFileSave();
	ConnectName->Clear();
    for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
    {
		ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);
    }
	ConnectName->ItemIndex = gm_OHTConnectInfo->nIndex;
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex];

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::FormCreate(TObject *Sender)
{
    gm_OHTConnectInfo->ConnectStart();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::btnConnectEditMouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint P, Q;
    if(Button == mbLeft)
    {
        P.x = X; // P is the TPoint record for the form
        P.y = Y;
        Q   = btnConnectEdit->ClientToScreen(P); // Q is the TPoint for the screen
        pmOHTInfo->Popup(Q.x, Q.y);
    }

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::FormShow(TObject *Sender)
{
    ConnectForm->Top   = MainForm->Top + MainForm->Height;
    ConnectForm->Left  = MainForm->Left;
    ConnectForm->Width = MainForm->Width;

    // sanghwa
    LinePickupColor->Color = (TColor)gm_OHTConnectInfo->g_iLineColor;
}

// ---------------------------------------------------------------------------
void __fastcall TConnectForm::btnConnectAssistantClick(TObject *Sender)
{
//	int nError = 0;
//
//
//
//	if(SocketForm->AssistantSocket->Socket->Connected == true)
//	{
//		SocketForm->AssistantSocket->Active = false;
//	}
//
//	SocketForm->AssistantConnect(ConnectIP->Text.Trim(), ConnectName->Text.Trim());
//
//	nError = FileForm->FileControlStart(ConnectIP->Text.Trim());

}

// ---------------------------------------------------------------------------
void TConnectForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::LinePickupColorClick(TObject *Sender)
{
    // Choose Color
    TColorDialog *pDlg = new TColorDialog(this);
    if(!pDlg)
        return;

    if(!pDlg->Execute())
        return;

    MainForm->panLineName->Color    = pDlg->Color;
    gm_OHTConnectInfo->g_iLineColor = pDlg->Color;
    LinePickupColor->Color          = pDlg->Color;
}

// ---------------------------------------------------------------------------
/**
 @brief   Remote.ini을 Edit하기 위한 함수
 @author  puting
 @date    2012.12.14
 @param   Remote.ini(IP, OHT 버젼 정보) Edit
 */
void __fastcall TConnectForm::btnIPEditClick(TObject *Sender)
{
	String strTmpPath = ExtractFilePath(Application->ExeName) + "Remote.ini";
	ShellExecute(Handle, L"open", strTmpPath.w_str(), NULL, NULL, SW_SHOW);
}

// ---------------------------------------------------------------------------
/**
 @brief   Remote.ini을 Load하기 위한 함수
 @author  puting
 @date    2012.12.14
 @param   Remote.ini(IP, OHT 버젼 정보) load
 */
void __fastcall TConnectForm::btnIPLoadClick(TObject *Sender)
{
	gm_OHTConnectInfo->ConnectStart();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectNameEnter(TObject *Sender)
{
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[ConnectName->ItemIndex];
	gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectIPKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	for(int i =0; i< ConnectName->Items->Count;i++)
	{
		if(StrComp(gm_OHTConnectInfo->strOHTIP[i].c_str(),ConnectIP->Text.c_str())==0)
		{
			ConnectName->ItemIndex = i;
			gm_OHTConnectInfo->nIndex = i;
			break;
		}
	}
}
//---------------------------------------------------------------------------

