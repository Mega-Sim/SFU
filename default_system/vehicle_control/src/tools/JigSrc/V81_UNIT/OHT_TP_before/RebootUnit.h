//---------------------------------------------------------------------------

#ifndef RebootUnitH
#define RebootUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TRebootForm : public TForm
{
__published:	// IDE-managed Components
   TLabel *Label1;
   TEdit *edtPassword;
   TButton *btnOK;
   TButton *btnCancel;
   void __fastcall btnOKClick(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TRebootForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRebootForm *RebootForm;
//---------------------------------------------------------------------------
#endif
