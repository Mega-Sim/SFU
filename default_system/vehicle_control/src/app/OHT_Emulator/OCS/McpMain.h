//---------------------------------------------------------------------------

#ifndef McpMainH
#define McpMainH
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
class TfrmMcpMain : public TForm
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
	TMenuItem *Window1;
	TMenuItem *mnuCascade;
	TMenuItem *mnuTileH;
	TMenuItem *mnuTileV;
	TImage *Image1;
	TMenuItem *mnuCommControl;
	TMenuItem *mnuServerStart;
	TMenuItem *mnuServerStop;
	TMenuItem *mnuProtocol;
	TMenuItem *N1;
	TMenuItem *Scenario1;
	TMenuItem *mnuMcpFindClw_Addr;
	TMenuItem *mnuMcpOrderAddress_Drop;
	TMenuItem *mnuMcpOrder_ControlGetMsg;
	TTimer *tmrUdpReceiveThread;
	TMenuItem *JobOrder_ID121;
	TMenuItem *mnuOcsStatus;
	TMenuItem *mnuOcsResponseOhtPass;
	TMenuItem *mnuOcsResponseOhtPassResponse;
	TMenuItem *Simulation1;
	TMenuItem *mnuAutoJob;
	TMenuItem *mnuOcsVerReq;
	TMenuItem *OCSOHT1;
	TMenuItem *mnuOhtInputRequestResponse;
	TMenuItem *mnuFloorMoveOrder;
	TMenuItem *mnuDiagSvrFindClw_Addr;
	TMenuItem *test1;
	TMenuItem *mnuDiagOrderSend;
	TMenuItem *mnuDiagInfoSet;
	void __fastcall mnuAboutClick(TObject *Sender);
	void __fastcall mnuComLogClick(TObject *Sender);
	void __fastcall mnuConfigClick(TObject *Sender);
	void __fastcall mnuCascadeClick(TObject *Sender);
	void __fastcall mnuTileHClick(TObject *Sender);
	void __fastcall mnuTileVClick(TObject *Sender);
	void __fastcall mnuServerStartClick(TObject *Sender);
	void __fastcall mnuMcpFindClw_AddrClick(TObject *Sender);
	void __fastcall tmrUdpReceiveThreadTimer(TObject *Sender);
	void __fastcall mnuServerStopClick(TObject *Sender);
	void __fastcall mnuMcpOrderAddress_DropClick(TObject *Sender);
	void __fastcall mnuMcpOrder_ControlGetMsgClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall JobOrder_ID121Click(TObject *Sender);
	void __fastcall mnuOcsStatusClick(TObject *Sender);
	void __fastcall mnuExitClick(TObject *Sender);
	void __fastcall mnuOcsResponseOhtPassClick(TObject *Sender);
	void __fastcall mnuOcsResponseOhtPassResponseClick(TObject *Sender);
	void __fastcall mnuAutoJobClick(TObject *Sender);
	void __fastcall mnuOcsVerReqClick(TObject *Sender);
	void __fastcall OCSOHT1Click(TObject *Sender);
	void __fastcall mnuOhtInputRequestResponseClick(TObject *Sender);
	void __fastcall mnuFloorMoveOrderClick(TObject *Sender);
	void __fastcall mnuDiagSvrFindClw_AddrClick(TObject *Sender);
	void __fastcall test1Click(TObject *Sender);
	void __fastcall mnuDiagOrderSendClick(TObject *Sender);
	void __fastcall mnuDiagInfoSetClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
	__fastcall TfrmMcpMain(TComponent* Owner);

public:
	// void Protocol_Receiver_OHT_STATUS(PKT_STATUS_REPORT_ID_03 Pkt);
	void StandardRead(PKT_OHT2OCS Pkt, int iMachineType, int iTagID, int iPacketSize);
	void Protocol_Receiver_OHT_PASSPERMIT(PKT_PASSPERMIT_OHT2OCS_ID_241 Pkt);
	void Protocol_Receiver_OHT_PASS_OPEN_PERMIT(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243 Pkt);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMcpMain *frmMcpMain;
//---------------------------------------------------------------------------
#endif
