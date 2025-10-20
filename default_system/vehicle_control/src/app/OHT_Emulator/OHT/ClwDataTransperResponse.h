//---------------------------------------------------------------------------

#ifndef ClwDataTransperResponseH
#define ClwDataTransperResponseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmClwDataTransperResponse : public TForm
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
	TTabSheet *TabSheet2;
	TButton *btnClose;
	TGroupBox *GroupBox4;
	TLabel *Label2;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label29;
	TLabel *Label19;
	TLabel *Label25;
	TEdit *edtOrderID;
	TEdit *IpAddr_H;
	TEdit *IpAddr_M1;
	TEdit *IpAddr_M2;
	TEdit *edtUserName;
	TEdit *edtVersion;
	TEdit *IpAddr_L;
	TEdit *edtPassword;
	TEdit *edtDataSendTo;
	TGroupBox *GroupBox5;
	TRadioGroup *rdoSendParam;
	TGroupBox *GroupBox6;
	TCheckBox *chkFtpOht_Req_0;
	TCheckBox *chkFtpOht_Req_1;
	TCheckBox *chkFtpOht_Req_2;
	TCheckBox *chkFtpOht_Req_3;
	TCheckBox *chkFtpOht_Req_4;
	TCheckBox *chkFtpOht_Req_5;
	TCheckBox *chkFtpOht_Req_6;
	TGroupBox *GroupBox7;
	TCheckBox *chkOhtFtp_Req_0;
	TCheckBox *chkOhtFtp_Req_1;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall rdoSendParamClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmClwDataTransperResponse(TComponent* Owner);
	void Trigger(PKT_DATA_TRANSPER_ORDER_OCS2OHT Pkt);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmClwDataTransperResponse *frmClwDataTransperResponse;
//---------------------------------------------------------------------------
#endif
