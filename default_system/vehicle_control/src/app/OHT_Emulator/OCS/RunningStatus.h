//---------------------------------------------------------------------------

#ifndef RunningStatusH
#define RunningStatusH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TfrmRunningStatus : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel2;
	TEdit *Edit2;
	TMemo *mBSLog;
	TButton *btnClear;
	TPanel *Panel1;
	TEdit *Edit1;
	TGroupBox *GroupBox2;
	TEdit *Running_0;
	TEdit *Running_1;
	TEdit *Running_2;
	TEdit *Running_3;
	TEdit *Edit23;
	TEdit *Edit24;
	TEdit *Edit25;
	TEdit *Edit26;
	TEdit *Edit27;
	TEdit *Running_4;
	TEdit *Edit29;
	TEdit *Running_5;
	TEdit *Edit31;
	TEdit *Running_6;
	TEdit *Edit33;
	TEdit *Running_7;
	TGroupBox *GroupBox1;
	TEdit *Work_0;
	TEdit *Work_1;
	TEdit *Work_2;
	TEdit *Work_3;
	TEdit *Edit7;
	TEdit *Edit8;
	TEdit *Edit9;
	TEdit *Edit10;
	TEdit *Edit11;
	TEdit *Work_4;
	TEdit *Edit13;
	TEdit *Work_5;
	TEdit *Edit15;
	TEdit *Work_6;
	TEdit *Edit17;
	TEdit *Work_7;
	TButton *btnHide;
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall btnHideClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
public:
	void DisplayOHT(PKT_STATUS_REPORT_ID_03	CmdPkt);
	void Log(AnsiString strLog);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmRunningStatus(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmRunningStatus *frmRunningStatus;
//---------------------------------------------------------------------------
#endif
