//---------------------------------------------------------------------------

#ifndef ClwVersionResponseH
#define ClwVersionResponseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmClwVersionRequest : public TForm
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
	TGroupBox *GroupBox2;
	TCheckBox *chkStatus_0;
	TCheckBox *chkStatus_1;
	TCheckBox *chkStatus_2;
	TCheckBox *chkStatus_4;
	TCheckBox *chkStatus_5;
	TButton *btnSend;
	TButton *btnClose;
	TTabSheet *TabSheet2;
	TLabel *Label2;
	TEdit *edtVersion;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnSendClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmClwVersionRequest(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmClwVersionRequest *frmClwVersionRequest;
//---------------------------------------------------------------------------
#endif
