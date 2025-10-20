//---------------------------------------------------------------------------

#ifndef SpeedHelpUnitH
#define SpeedHelpUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSpeedHelpForm : public TForm
{
__published:	// IDE-managed Components
   TMemo *Memo1;
   TPanel *Panel1;
   TPanel *Panel2;
   TPanel *Panel3;
   TPanel *Panel4;
   TPanel *Panel5;
   TPanel *Panel6;
   TEdit *edtMPerSec;
   TEdit *edtMPerMin;
   TEdit *edtCPerSec;
   TEdit *edtCPerMSec;
   void __fastcall edtMPerSecKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:		// User declarations
   __fastcall TSpeedHelpForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSpeedHelpForm *SpeedHelpForm;
//---------------------------------------------------------------------------
#endif
