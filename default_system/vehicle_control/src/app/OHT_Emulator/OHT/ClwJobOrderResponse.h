//---------------------------------------------------------------------------

#ifndef ClwJobOrderResponseH
#define ClwJobOrderResponseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Protocol_Define_Modify.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmJobOrderResponse : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TComboBox *cboWorkingStatus;
	TComboBox *cboRunningStatus;
	TComboBox *cboCarriage;
	TComboBox *cboMaintananceState;
	TComboBox *cboCarryType;
	TComboBox *cboTunDirection;
	TEdit *edtCurAddr_H;
	TEdit *edtCurAddr_M;
	TEdit *edtCurAddr_L;
	TEdit *edtStopAddr_H;
	TEdit *edtStopAddr_M;
	TEdit *edtStopAddr_L;
	TEdit *edtDestAddr_H;
	TEdit *edtDestAddr_M;
	TEdit *edtDestAddr_L;
	TEdit *edtRunningDistance;
	TEdit *edtErrorCode;
	TEdit *edtRadioWaveLevel;
	TEdit *edtDataTransStatus;
	TMemo *Memo1;
	TComboBox *cboWaitingCycle;
	TMemo *Memo2;
	TComboBox *cboRunningCycle;
	TButton *btnSend;
	TButton *btnClose;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox2;
	TLabel *Label2;
	TEdit *edtOrderID;
	TButton *btnGetOrderID;
	TGroupBox *grpResponseDiv;
	TCheckBox *chkNotAccept;
	TGroupBox *grpNotAcceptReason;
	TRadioButton *rdoVhlError;
	TRadioButton *rdoNotVhlError;
	TComboBox *choVhlError;
	TComboBox *choNotVhlError;
	TGroupBox *GroupBox3;
	TCheckBox *chkTestMode;
	TComboBox *cboOrderSep;
	TGroupBox *GroupBox4;
	TRadioButton *rdoOrderPort1;
	TRadioButton *rdoOrderPort2;
	TComboBox *cboOrderCycle;
	TLabel *Label23;
	TLabel *Label24;
	TEdit *edtTargetNode_H;
	TEdit *edtTargetNode_M;
	TEdit *edtTargetNode_L;
	TEdit *edtTargetStationNode_H;
	TEdit *edtTargetStationNode_M;
	TEdit *edtTargetStationNode_L;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TGroupBox *GroupBox5;
	TCheckBox *chkHotLot;
	TEdit *edtPriority;
	TLabel *Label28;
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall chkNotAcceptClick(TObject *Sender);
	void __fastcall rdoVhlErrorClick(TObject *Sender);
	void __fastcall rdoNotVhlErrorClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmJobOrderResponse(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmJobOrderResponse *frmJobOrderResponse;
//---------------------------------------------------------------------------
#endif
