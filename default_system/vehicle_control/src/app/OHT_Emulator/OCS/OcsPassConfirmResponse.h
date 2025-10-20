//---------------------------------------------------------------------------

#ifndef OcsPassConfirmResponseH
#define OcsPassConfirmResponseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TfrmOcsPassConfirmResponse : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *grpStatus;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label2;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox2;
	TLabel *Label7;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label29;
	TEdit *edtOrderID;
	TButton *btnGetOrderID;
	TEdit *edtTargetStop_H;
	TEdit *edtTargetStop_M;
	TEdit *edtTargetStop_L;
	TEdit *edtResetMain_H;
	TEdit *edtResetMain_M;
	TEdit *edtResetMain_L;
	TEdit *edtResetSub_H;
	TEdit *edtResetSub_M;
	TEdit *edtResetSub_L;
	TButton *btnSend;
	TButton *btnClose;
	TComboBox *cboStopReason;
	TComboBox *cboMaintananceOrder;
	TComboBox *cboEmOder;
	TComboBox *cboDataTransOrder;
	TGroupBox *GroupBox4;
	TCheckBox *chkStatus_0;
	TCheckBox *chkStatus_1;
	TCheckBox *chkStatus_2;
	TCheckBox *chkStatus_3;
	TCheckBox *chkStatus_4;
	TCheckBox *chkStatus_5;
	TCheckBox *chkStatus_6;
	TCheckBox *chkStatus_7;
	TGroupBox *GroupBox5;
	TCheckBox *chkStopOrder_0;
	TCheckBox *chkStopOrder_1;
	TCheckBox *chkStopOrder_2;
	TCheckBox *chkStopOrder_3;
	TCheckBox *chkStopOrder_4;
	TCheckBox *chkStopOrder_5;
	TCheckBox *chkStopOrder_6;
	TCheckBox *chkStopOrder_7;
	TCheckBox *chkAutoReply;
	TMemo *mBSLog;
	TButton *btnDel;
	TButton *btnHide;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnDelClick(TObject *Sender);
	void __fastcall btnHideClick(TObject *Sender);

private:	// User declarations
	AnsiString strCurrentFileName;

public:		// User declarations
	__fastcall TfrmOcsPassConfirmResponse(TComponent* Owner);
	void Trigger(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243 Pkt);

	void SetButtonMode(bool bAutoModeChild);
	void SetResponseMode(int iResponseType);
	void SaveFile(AnsiString strFileName);
	void LoadFile(AnsiString strFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOcsPassConfirmResponse *frmOcsPassConfirmResponse;
//---------------------------------------------------------------------------
#endif
