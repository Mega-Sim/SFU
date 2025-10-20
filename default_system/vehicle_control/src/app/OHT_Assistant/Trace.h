//---------------------------------------------------------------------------

#ifndef TraceH
#define TraceH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmWork : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TButton *btnClear;
	void __fastcall btnClearClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmWork(TComponent* Owner);
	void DisplayLog(String strLog);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWork *frmWork;
//---------------------------------------------------------------------------
#endif
