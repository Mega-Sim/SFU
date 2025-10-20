//---------------------------------------------------------------------------

#ifndef UDPH
#define UDPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
#include "UdpCom.h"
#include "UdpDiag.h"

//---------------------------------------------------------------------------
class TfrmUDP : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *Log;
	TTabSheet *DirectCommandSend;
	TMemo *mBSLog;
	TButton *btnLogDel;
	TGroupBox *GroupBox1;
	TLabel *Label5;
	TEdit *edtBSPort;
	TEdit *edtBSMessage;
	TGroupBox *GroupBox3;
	TLabel *Label4;
	TLabel *Label6;
	TEdit *edtUSAddr;
	TEdit *edtUSPort;
	TEdit *edtUSMessage;
	TGroupBox *GroupBox2;
	TLabel *Label2;
	TLabel *Label7;
	TEdit *edtBRPort;
	TEdit *Edit6;
	TGroupBox *GroupBox4;
	TLabel *Label1;
	TLabel *Label8;
	TEdit *edtURPort;
	TEdit *Edit10;
	TEdit *Edit1;
	TButton *btnBCthread;
	TCheckBox *chkOHT_Addr_GetTest;
	TCheckBox *chkOCS_Addr_GetTest;
	TTimer *trmAddrRetryTest;
	TCheckBox *chkAutoLogClear;
	TTimer *tmrLogClear;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnLogDelClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnBCthreadClick(TObject *Sender);
	void __fastcall trmAddrRetryTestTimer(TObject *Sender);
	void __fastcall chkOCS_Addr_GetTestClick(TObject *Sender);
	void __fastcall tmrLogClearTimer(TObject *Sender);

public:
	UdpCom	*pUdpObject;
	UdpDiag *pDiagObject;

private:	// User declarations
	// void err_display(char *msg);


public:		// User declarations
	__fastcall TfrmUDP(TComponent* Owner);
	// UdpCom	*pUdpObject;

	void DisplayLog(String strLog);
	int UnicastSend(char* buf, int len);
	int UnicastSend_toMapView(AnsiString strMessage);
	int DiagSend(char* buf, int len);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmUDP *frmUDP;
//---------------------------------------------------------------------------
#endif
