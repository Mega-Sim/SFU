//---------------------------------------------------------------------------

#ifndef ResponsePasspermitOCSH
#define ResponsePasspermitOCSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TfrmResponsePasspermitOCS : public TForm
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
	TGroupBox *GroupBox1;
	TRadioButton *rdoPassNo;
	TRadioButton *rdoPassNormal;
	TRadioButton *rdoPassUser;
	TLabel *Label8;
	TEdit *edtPassSpeed;
	TGroupBox *GroupBox3;
	TRadioButton *rdoResultOk;
	TRadioButton *rdoResultMcpOther;
	TRadioButton *rdoResultClwConfirmOk;
	TButton *btnSend;
	TButton *btnClose;
	TComboBox *cboStopReason;
	TComboBox *cboMaintananceOrder;
	TComboBox *cboEmOder;
	TComboBox *cboDataTransOrder;
	TCheckBox *chkAutoReply;
	TMemo *mBSLog;
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
	TButton *btnDel;
	TButton *btnHide;
	TRadioButton *rdoResultMcpInStart_NoTouch;
	TRadioButton *rdoResultMcpInStart_CID_CantUse;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnDelClick(TObject *Sender);
	void __fastcall btnHideClick(TObject *Sender);
private:	// User declarations
	AnsiString strCurrentFileName;

public:		// User declarations
	__fastcall TfrmResponsePasspermitOCS(TComponent* Owner);
	void Trigger(PKT_PASSPERMIT_OHT2OCS_ID_241 Pkt);

	void SetButtonMode(bool bAutoModeChild);
	void SetResponseMode(int iResponseType);
	void SaveFile(AnsiString strFileName);
	void LoadFile(AnsiString strFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmResponsePasspermitOCS *frmResponsePasspermitOCS;
//---------------------------------------------------------------------------
#endif
