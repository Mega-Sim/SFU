//---------------------------------------------------------------------------

#ifndef ClwStatusReportH
#define ClwStatusReportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"

//---------------------------------------------------------------------------
class TfrmClwStatusReport : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TButton *btnSend;
	TButton *btnClose;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *cboRunningStatus;
	TLabel *Label3;
	TComboBox *cboCarriage;
	TComboBox *cboMaintananceState;
	TComboBox *cboCarryType;
	TComboBox *cboTunDirection;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TEdit *edtCurAddr_H;
	TEdit *edtCurAddr_M;
	TEdit *edtCurAddr_L;
	TEdit *edtStopAddr_H;
	TEdit *edtStopAddr_M;
	TEdit *edtStopAddr_L;
	TEdit *edtDestAddr_H;
	TEdit *edtDestAddr_M;
	TEdit *edtDestAddr_L;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TEdit *edtRunningDistance;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TEdit *edtErrorCode;
	TEdit *edtRadioWaveLevel;
	TLabel *Label20;
	TEdit *edtDataTransStatus;
	TLabel *Label21;
	TLabel *Label22;
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
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
//private:	// User declarations
public:
	 void DisplayInform(PKT_STATUS_REPORT_ID_03 Pkt);
public:		// User declarations
	__fastcall TfrmClwStatusReport(TComponent* Owner);


};
//---------------------------------------------------------------------------
extern PACKAGE TfrmClwStatusReport *frmClwStatusReport;
//---------------------------------------------------------------------------
#endif
