//---------------------------------------------------------------------------

#ifndef RFTPUnitH
#define RFTPUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TRFTPForm : public TForm
{
__published:	// IDE-managed Components
   TEdit *edtRFTPID;
   TLabel *Label1;
   TButton *Button1;
   TButton *Button2;
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TRFTPForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRFTPForm *RFTPForm;
//---------------------------------------------------------------------------
#endif
