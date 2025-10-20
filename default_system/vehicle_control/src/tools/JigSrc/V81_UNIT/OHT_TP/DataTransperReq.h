//---------------------------------------------------------------------------

#ifndef DataTransperReqH
#define DataTransperReqH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmDataTransperReq : public TForm
{
__published:	// IDE-managed Components
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
	TButton *btnSend;
	TGroupBox *GroupBox1;
	TRadioButton *rdoFtpOht_Req_0;
	TRadioButton *rdoFtpOht_Req_1;
	TGroupBox *GroupBox2;
	TRadioButton *rdoOhtFtp_Req_1;
	TGroupBox *GroupBox3;
	TMemo *mmLog;
	TButton *btnLogClear;
	TGroupBox *GroupBox4;
	TPanel *panCmd_0;
	TPanel *panCmd_1;
	TPanel *panCmd_2;
	TPanel *panCmd_3;
	TPanel *panCmd_Start;
	TRadioButton *rdoFtpOht_Req_4;
	TLabel *labMap;
	TLabel *lblMapVersion;
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnLogClearClick(TObject *Sender);

private:	// User declarations
	void Reset_Display(void);
	void Parse_Result(AnsiString strMsg);

public:		// User declarations
	__fastcall TfrmDataTransperReq(TComponent* Owner);
	void DataTranaper_Display(AnsiString strMsg);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDataTransperReq *frmDataTransperReq;
//---------------------------------------------------------------------------
#endif
