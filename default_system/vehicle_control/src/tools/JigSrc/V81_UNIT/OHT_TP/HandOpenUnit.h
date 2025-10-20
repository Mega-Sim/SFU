//---------------------------------------------------------------------------

#ifndef HandOpenUnitH
#define HandOpenUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class THandOpenForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *GroupBox1;
   TCheckBox *CheckBox1;
   TCheckBox *CheckBox2;
   TCheckBox *CheckBox3;
   TCheckBox *CheckBox4;
   TBitBtn *btnOK;
   TBitBtn *btnCancel;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall CheckBox1Click(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
   void __fastcall btnOKClick(TObject *Sender);
private:	// User declarations
   int Check_OKBtn(void);
   int  Check_Is_HandOpen(void);
public:		// User declarations
   __fastcall THandOpenForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THandOpenForm *HandOpenForm;
//---------------------------------------------------------------------------
#endif
