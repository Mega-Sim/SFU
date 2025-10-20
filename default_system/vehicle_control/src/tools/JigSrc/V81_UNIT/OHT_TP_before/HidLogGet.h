//---------------------------------------------------------------------------

#ifndef HidLogGetH
#define HidLogGetH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmHidLogGet : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TGroupBox *GroupBox4;
	TPanel *panCmd_Response;
	TPanel *panCmd_Complete;
	TPanel *panCmd_Start;
	TButton *btnStart;
	TEdit *edtHelp;
	TButton *btnClose;
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
private:	// User declarations
	void Display_Clear(void);
public:		// User declarations
	__fastcall TfrmHidLogGet(TComponent* Owner);
	void Display_Result(int iVar1, int iVar2);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmHidLogGet *frmHidLogGet;
//---------------------------------------------------------------------------
#endif
