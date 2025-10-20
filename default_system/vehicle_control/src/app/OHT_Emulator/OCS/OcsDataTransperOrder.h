//---------------------------------------------------------------------------

#ifndef OcsDataTransperOrderH
#define OcsDataTransperOrderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmOcsDataTransperOrder : public TForm
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
	TComboBox *cboStopReason;
	TComboBox *cboMaintananceOrder;
	TComboBox *cboEmOder;
	TComboBox *cboDataTransOrder;
	TGroupBox *GroupBox1;
	TCheckBox *chkStatus_0;
	TCheckBox *chkStatus_1;
	TCheckBox *chkStatus_2;
	TCheckBox *chkStatus_3;
	TCheckBox *chkStatus_4;
	TCheckBox *chkStatus_5;
	TCheckBox *chkStatus_6;
	TCheckBox *chkStatus_7;
	TGroupBox *GroupBox2;
	TCheckBox *chkStopOrder_0;
	TCheckBox *chkStopOrder_1;
	TCheckBox *chkStopOrder_2;
	TCheckBox *chkStopOrder_3;
	TCheckBox *chkStopOrder_4;
	TCheckBox *chkStopOrder_5;
	TCheckBox *chkStopOrder_6;
	TCheckBox *chkStopOrder_7;
	TButton *btnSend;
	TButton *btnClose;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox4;
	TLabel *Label7;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label29;
	TEdit *edtOrderID;
	TButton *btnGetOrderID;
	TEdit *IpAddr_H;
	TEdit *IpAddr_M1;
	TEdit *IpAddr_M2;
	TEdit *edtUserName;
	TEdit *edtVersion;
	TEdit *IpAddr_L;
	TEdit *edtPassword;
	TEdit *edtDataSendTo;
	TLabel *Label8;
	TLabel *Label9;
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
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall rdoSendParamClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmOcsDataTransperOrder(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOcsDataTransperOrder *frmOcsDataTransperOrder;
//---------------------------------------------------------------------------
#endif
