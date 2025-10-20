//---------------------------------------------------------------------------

#ifndef ClwPassConfirmH
#define ClwPassConfirmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmClwPassConfirm : public TForm
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
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox2;
	TLabel *Label2;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TEdit *edtOrderID;
	TButton *btnGetOrderID;
	TEdit *edtTargetStop_H;
	TEdit *edtTargetStop_M;
	TEdit *edtTargetStop_L;
	TEdit *edtResetMain_H;
	TEdit *edtResetMain_M;
	TEdit *edtResetMain_L;
	TGroupBox *GroupBox5;
	TEdit *edtPassWaitTime;
	TButton *btnSend;
	TButton *btnClose;
	TLabel *Label29;
	TEdit *edtResetSub_H;
	TEdit *edtResetSub_M;
	TEdit *edtResetSub_L;
	TCheckBox *chkManualPass;
	TLabel *Label28;
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCloseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmClwPassConfirm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmClwPassConfirm *frmClwPassConfirm;
//---------------------------------------------------------------------------
#endif
