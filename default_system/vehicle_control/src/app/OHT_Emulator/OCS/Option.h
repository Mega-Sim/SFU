//---------------------------------------------------------------------------

#ifndef OptionH
#define OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmOption : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *edtMachineID;
	TLabel *Label4;
	TEdit *edtBcPort;
	TLabel *Label3;
	TEdit *edtBcTimeOut;
	TLabel *Label5;
	TEdit *edtCtPort;
	TLabel *Label6;
	TEdit *edtCpTimeOut;
	TLabel *Label7;
	TEdit *edtAutoStatusSendTime;
	TButton *btnClose;
	TRadioButton *rdoOCS;
	TRadioButton *rdoOHT;
	TRadioButton *rdoTP;
	TGroupBox *GroupBox2;
	TLabel *Label8;
	TEdit *edtOhtMachineID;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	void LoadConfig();
public:		// User declarations
	__fastcall TfrmOption(TComponent* Owner);

public:
	unsigned short iOpt_MachineType;
	AnsiString strMachineID;
	AnsiString strOhtMachineID;

	int iBroadCastPort;
	int iBroadCastTimeOut;

	int iControlPort;
	int iControlTimeOut;

	int iAutoStatusSendTime;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOption *frmOption;
//---------------------------------------------------------------------------
#endif
