//---------------------------------------------------------------------------

#ifndef Oht2TPH
#define Oht2TPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmOht2TP : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TButton *btnClose;
	TButton *Button1;
	TButton *Button2;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TButton *Button3;
	TCheckBox *chkStatusSendAuto;
	TTimer *tmrAutoStatus;
	TButton *Button4;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall tmrAutoStatusTimer(TObject *Sender);
	void __fastcall chkStatusSendAutoClick(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmOht2TP(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOht2TP *frmOht2TP;
//---------------------------------------------------------------------------
#endif
