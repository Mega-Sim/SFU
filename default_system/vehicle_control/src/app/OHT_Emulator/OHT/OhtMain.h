//---------------------------------------------------------------------------

#ifndef OhtMainH
#define OhtMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <jpeg.hpp>

#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TfrmOHTMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *mnuFile;
	TMenuItem *mnuTool;
	TMenuItem *mnuHelp;
	TMenuItem *mnuAbout;
	TMenuItem *mnuConfig;
	TMenuItem *mnuExit;
	TCoolBar *CoolBar1;
	TSpeedButton *SButtonIpAddr;
	TBevel *Bevel6;
	TMenuItem *mnuView;
	TMenuItem *mnuComLog;
	TImage *Image1;
	TMenuItem *Window1;
	TMenuItem *mnuCascade;
	TMenuItem *mnuTileH;
	TMenuItem *mnuTileV;
	TMenuItem *mnuCommControl;
	TMenuItem *mnuServerStart;
	TMenuItem *mnuServerStop;
	TMenuItem *mnuProtocol;
	TMenuItem *mnuWorkSend;
	TMenuItem *Senario1;
	TMenuItem *mnuClwFindMcp_Addr;
	TTimer *tmrUdpReceiveThread;
	TMenuItem *mnuSendStatus_OHT_to_OCS;
	TMenuItem *JobResponseID122;
	TMenuItem *mnuSendProgress_OHT_to_OCS;
	TMenuItem *mnuPassRequest_OHT_to_OCS;
	TMenuItem *OHTID1;
	TMenuItem *mnuPcTpTest;
	TMenuItem *mnuVersionResponse;
	TMenuItem *mnuOht2OcsPassConfirmStatusReport;
	TMenuItem *mnuClwFindDiag_Addr;
	TMenuItem *test1;
	TMenuItem *mnuDiagAnswer;
	TMenuItem *mnuOcsFileTransperRun;
	TMenuItem *mnuClwFindMcp_Addr_NoClr;
	TMenuItem *mndDiagInfoSet;
	void __fastcall mnuAboutClick(TObject *Sender);
	void __fastcall mnuComLogClick(TObject *Sender);
	void __fastcall mnuConfigClick(TObject *Sender);
	void __fastcall mnuCascadeClick(TObject *Sender);
	void __fastcall mnuTileHClick(TObject *Sender);
	void __fastcall mnuTileVClick(TObject *Sender);
	void __fastcall mnuServerStartClick(TObject *Sender);
	void __fastcall mnuClwFindMcp_AddrClick(TObject *Sender);
	void __fastcall tmrUdpReceiveThreadTimer(TObject *Sender);
	void __fastcall mnuServerStopClick(TObject *Sender);
	void __fastcall mnuSendStatus_OHT_to_OCSClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall JobResponseID122Click(TObject *Sender);
	void __fastcall mnuSendProgress_OHT_to_OCSClick(TObject *Sender);
	void __fastcall mnuPassRequest_OHT_to_OCSClick(TObject *Sender);
	void __fastcall OHTID1Click(TObject *Sender);
	void __fastcall mnuPcTpTestClick(TObject *Sender);
	void __fastcall mnuVersionResponseClick(TObject *Sender);
	void __fastcall mnuOht2OcsPassConfirmStatusReportClick(TObject *Sender);
	void __fastcall mnuClwFindDiag_AddrClick(TObject *Sender);
	void __fastcall test1Click(TObject *Sender);
	void __fastcall mnuDiagAnswerClick(TObject *Sender);
	void __fastcall mnuOcsFileTransperRunClick(TObject *Sender);
	void __fastcall mnuClwFindMcp_Addr_NoClrClick(TObject *Sender);
	void __fastcall mndDiagInfoSetClick(TObject *Sender);
	// void __fastcall tmrUdpReceiveThreadTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmOHTMain(TComponent* Owner);

public:
	// void Protocol_Receiver_JOB_ORDER(PKT_JOB_ORDER_ID_121 Pkt);
	// void Protocol_Receiver_OCS_STATUS(PKT_OCS_STATUS_ID_01 Pkt);
	void StandardRead(PKT_OCS2OHT Pkt, int iMachineType);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOHTMain *frmOHTMain;
//---------------------------------------------------------------------------
#endif
