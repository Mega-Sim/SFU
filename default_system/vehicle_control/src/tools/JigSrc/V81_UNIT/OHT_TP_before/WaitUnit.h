//---------------------------------------------------------------------------

#ifndef WaitUnitH
#define WaitUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TWaitForm : public TForm
{
__published:	// IDE-managed Components
   TMemo *DisplayMemo;
   void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TWaitForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWaitForm *WaitForm;
//---------------------------------------------------------------------------
#endif
