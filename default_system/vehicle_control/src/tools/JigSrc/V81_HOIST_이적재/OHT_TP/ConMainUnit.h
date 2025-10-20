//---------------------------------------------------------------------------

#ifndef ConMainUnitH
#define ConMainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TConversionForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TLabel *Label1;
   TLabel *Label2;
   TEdit *Edit2;
   TEdit *Edit3;
   TRadioGroup *RadioGroup2;
   TRadioGroup *RadioGroup1;
   TEdit *Edit1;
   TButton *Button1;
   TMemo *Memo1;
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall RadioGroup2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TConversionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConversionForm *ConversionForm;
//---------------------------------------------------------------------------
#endif
